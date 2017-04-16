# integration.py

import bpy
from bpy.props import BoolProperty
from bpy.types import Operator, Panel
from . import editor_session, type_db, scene_manager, resource_manager, types, ui, static_mesh, operators
import time
import math


class Globals(object):
    # Global variable to prevent scene updates during critical sections
    # I have this rather than adding/removing the app handler because I don't trust how Blender handles it
    should_update = False
    selected_objects = []
    active_object = 0
    last_low_priority_update = 0
    blender_old_undo_steps = 32

    @staticmethod
    def reset():
        Globals.should_update = False
        Globals.selected_objects = []
        Globals.active_object = 0
        Globals.last_low_priority_update = 0
        Globals.blender_old_undo_steps = 32

    @staticmethod
    def enable_update():
        Globals.should_update = True

    @staticmethod
    def disable_update():
        Globals.should_update = False


def create_blender_resource(res_manager, path, res_type, value):
    # Unused arguments
    del res_manager

    if res_type == 'sge::StaticMesh':
        return static_mesh.from_json(path, value)


def new_node_callback(sge_scene, node):
    # Unused arguments
    del sge_scene

    # Disable scene updates
    Globals.disable_update()

    # Create the object
    obj = bpy.data.objects.new(node.name, None)
    bpy.context.scene.objects.link(obj)

    # Set the node id on it
    obj.sge_node_id = node.id
    node.user_data = obj

    # Re-enable scene updates
    Globals.enable_update()


def destroy_node_callback(sge_scene, node):
    # Unused arguments
    del sge_scene

    if node.user_data is None:
        return

    # Disable scene updates
    Globals.disable_update()

    # Delete the object
    bpy.ops.object.select_all(action='DESELECT')
    node.user_data.select = True
    bpy.ops.object.delete()

    # Remove the user data
    node.user_data = None

    # Re-enable scene updates
    Globals.enable_update()


def validate_object_type(sge_scene, node):
    obj = node.user_data
    res = types.SinGEDProps.sge_resource_manager
    data = None

    # Get relevant component containers
    static_mesh_comp_cont = sge_scene.get_component_type('sge::CStaticMesh')
    perspective_camera_comp_cont = sge_scene.get_component_type('sge::CPerspectiveCamera')

    # If the object is supposed to be a mesh
    if node in static_mesh_comp_cont.instances:
        # If it is a mesh
        if obj.type == 'MESH':
            return

        # Get the default mesh datablock
        data = res.get_resource_immediate('BLENDER_DEFAULT_MESH')

    # If the object is supposed to be a camera
    elif node in perspective_camera_comp_cont.instances:
        # If it's a camera
        if obj.type == 'CAMERA':
            return

        # Create a camera datablock
        data = bpy.data.cameras.new(obj.name)

    # If none of the above are true, it's supposed to be an empty
    elif obj.type == 'EMPTY':
        return

    # Get the object's current transform
    rot_mode = obj.rotation_mode
    obj.rotation_mode = 'QUATERNION'
    local_position = tuple(obj.location)
    local_scale = tuple(obj.scale)
    local_rotation = tuple(obj.rotation_quaternion)
    parent = obj.parent

    # Delete the object
    bpy.ops.object.select_all(action='DESELECT')
    obj.select = True
    bpy.ops.object.delete()

    # Create a new object
    obj = bpy.data.objects.new(node.name, data)
    bpy.context.scene.objects.link(obj)
    bpy.context.scene.objects.active = obj
    obj.select = True

    # Restore node data
    obj.sge_node_id = node.id
    node.user_data = obj

    # Restore the transform
    obj.parent = parent
    obj.rotation_mode = 'QUATERNION'
    obj.location = local_position
    obj.scale = local_scale
    obj.rotation_quaternion = local_rotation
    obj.rotation_mode = rot_mode


def update_node_callback(sge_scene, node):
    # Get the node's user data
    obj = node.user_data

    if obj is None:
        return

    # Assign Id and name
    obj.sge_node_id = node.id
    obj.name = node.name

    # Make sure the parent is correct
    if node.root is None:
        obj.parent = None
    else:
        # Get the root node
        obj.parent = node.root.user_data

    # Assign local position (swizzle components)
    obj.location[0] = -node.local_position[0]
    obj.location[2] = node.local_position[1]
    obj.location[1] = node.local_position[2]

    # Assign local scale (swizzle components)
    obj.scale[0] = node.local_scale[0]
    obj.scale[2] = node.local_scale[1]
    obj.scale[1] = node.local_scale[2]

    # Assign local rotation (swizzle components)
    rot_mode = obj.rotation_mode
    obj.rotation_mode = 'QUATERNION'
    obj.rotation_quaternion[1] = -node.local_rotation[0]
    obj.rotation_quaternion[3] = node.local_rotation[1]
    obj.rotation_quaternion[2] = node.local_rotation[2]
    obj.rotation_quaternion[0] = node.local_rotation[3]
    obj.rotation_mode = rot_mode


def update_static_mesh_component_callback(component_instance):
    # Disable scene updates
    Globals.disable_update()

    # Make sure the object is a mesh
    obj = component_instance.node.user_data
    assert(obj.type == 'MESH')

    # Callback to set the mesh on the object
    def set_mesh(res_m, path, mesh):
        del res_m, path
        nonlocal obj
        obj.data = mesh

    # Request the resources
    res = types.SinGEDProps.sge_resource_manager
    component_instance.get_property_async('mesh', lambda mesh: res.get_resource_async(mesh, 'sge::StaticMesh', set_mesh))

    # Re-enable updates
    Globals.enable_update()


def update_node_display_callback(component_instance):
    # Disable scene updates
    Globals.disable_update()

    # Update object type
    validate_object_type(types.SinGEDProps.sge_scene, component_instance.node)

    # If it's a mesh, call the update function
    if component_instance.type.type_name == 'sge::CStaticMesh':
        update_static_mesh_component_callback(component_instance)

    # Re-enable updates
    Globals.enable_update()


def validate_node(sge_scene, obj):
    node_id = obj.sge_node_id

    # If the object is brand new
    if node_id == 0:
        # Validate the parent
        if obj.parent is not None:
            validate_node(sge_scene, obj.parent)

        # Create a new node for it
        node = sge_scene.request_new_node(obj)
        obj.sge_node_id = node.id

        # Set the name and root
        node.name = obj.name
        sge_scene.mark_name_dirty(node)
        node.root = sge_scene.get_node(obj.parent.sge_node_id) if obj.parent is not None else scene_manager.Node.NULL_ID
        sge_scene.mark_root_dirty(node)

        # Set local position (swizzled)
        node.local_position[0] = -obj.location[0]
        node.local_position[1] = obj.location[2]
        node.local_position[2] = obj.location[1]

        # Set local scale (swizzled)
        node.local_scale[0] = obj.scale[0]
        node.local_scale[1] = obj.scale[2]
        node.local_scale[2] = obj.scale[1]

        # Set local rotation (swizzled)
        node.local_rotation[0] = -obj.rotation_quaternion[1]
        node.local_rotation[1] = obj.rotation_quaternion[3]
        node.local_rotation[2] = obj.rotation_quaternion[2]
        node.local_rotation[3] = obj.rotation_quaternion[0]
        sge_scene.mark_local_transform_dirty(node)

        # If it's a mesh
        if obj.type == 'MESH':
            static_mesh_comp_cont = sge_scene.get_component_type('sge::CStaticMesh')
            static_mesh_comp_instance = static_mesh_comp_cont.request_new_instance(node)
            mesh_path = ''

            # If it's a cube
            if obj.name.startswith('Cube'):
                mesh_path = "Content/Meshes/Primitives/cube.sbin"

            # If it's a plane or a grid
            elif obj.name.startswith('Plane') or obj.name.startswith('Grid'):
                mesh_path = "Content/Meshes/Primitives/plane.sbin"

            # If it's a circle
            elif obj.name.startswith('Circle'):
                mesh_path = "Content/Meshes/Primitives/circle.sbin"

            # If it's a sphere or Icosphere
            elif obj.name.startswith('Sphere') or obj.name.startswith('Icosphere'):
                mesh_path = "Content/Meshes/Primitives/sphere.sbin"

            # If it's a cylinder
            elif obj.name.startswith('Cylinder'):
                mesh_path = "Content/Meshes/Primitives/cylinder.sbin"

            # If it's a cone
            elif obj.name.startswith('Cone'):
                mesh_path = "Content/Meshes/Primitives/cone.sbin"

            # If it's a torus
            elif obj.name.startswith('Torus'):
                mesh_path = "Content/Meshes/Primitives/torus.sbin"

            # If it's a monkey :D
            elif obj.name.startswith('Suzanne'):
                mesh_path = "Content/Meshes/Primitives/monkey.sbin"

            # Set the mesh and material
            static_mesh_comp_instance.set_property('mesh', mesh_path)
            static_mesh_comp_instance.set_property('material', "Content/Materials/Misc/checkerboard.json")

            # Create a callback to set the display mesh
            def set_mesh(res_m, path, mesh):
                del res_m, path
                nonlocal obj
                obj.data = mesh

            # Request the resources
            res = types.SinGEDProps.sge_resource_manager
            res.get_resource_async(mesh_path, 'sge::StaticMesh', set_mesh)

        # If it's a camera
        if obj.type == 'CAMERA':
            perspective_camera_comp_cont = sge_scene.get_component_type('sge::CPerspectiveCamera')
            perspective_camera_comp_cont.request_new_instance(node)

        return

    # Get the node associated with this object
    node = sge_scene.get_node(node_id)

    # Check The object is a duplicate, so create a new node and copy everything from the old one
    if node.user_data != obj:
        old_node = node

        # Validate the parent
        if obj.parent is not None:
            validate_node(sge_scene, obj.parent)
            root = sge_scene.get_node(obj.parent.sge_node_id)
        else:
            root = None

        # Create the new node object
        node = sge_scene.request_new_node(obj)
        obj.sge_node_id = node.id

        # Set the name and root
        node.name = obj.name
        sge_scene.mark_name_dirty(node)
        node.root = root
        sge_scene.mark_root_dirty(node)

        # Set local position (swizzled)
        node.local_position[0] = -obj.location[0]
        node.local_position[1] = obj.location[2]
        node.local_position[2] = obj.location[1]

        # Set local scale (swizzled)
        node.local_scale[0] = obj.scale[0]
        node.local_scale[1] = obj.scale[2]
        node.local_scale[2] = obj.scale[1]

        # Set local rotation (swizzled)
        node.local_rotation[0] = -obj.rotation_quaternion[1]
        node.local_rotation[1] = obj.rotation_quaternion[3]
        node.local_rotation[2] = obj.rotation_quaternion[2]
        node.local_rotation[3] = obj.rotation_quaternion[0]

        sge_scene.mark_local_transform_dirty(node)

        # For each component on the original node
        for old_instance in sge_scene.get_node_components(old_node):
            component_type = old_instance.type

            # Create the same type of component on the new node
            new_instance = component_type.request_new_instance(node)

            # Copy the value from the old noe
            old_instance.get_value_async(lambda value: new_instance.set_value(value))
        return

    # Make sure the parent is correct
    if obj.parent is None:
        if node.root is not None:
            node.root = None
            sge_scene.mark_root_dirty(node)
    elif node.root is None or node.root.user_data != obj.parent:
        root_node = sge_scene.get_node(obj.parent.sge_node_id)
        node.root = root_node
        sge_scene.mark_root_dirty(node)
        assert(not root_node.destroyed)


def lightmaps_generated_callback(duration):
    bpy.ops.singed.notification('INVOKE_DEFAULT', message="Lightmap generation completed in {} milliseconds".format(duration))


def blender_update(scene):
    self = types.SinGEDProps

    # Make sure we're not in a critical section
    if not Globals.should_update:
        return

    # Check previously selected objects
    for node_id in Globals.selected_objects:
        node = self.sge_scene.get_node(node_id)
        obj = node.user_data
        # If the current object no longer exists in the scene
        if scene not in obj.users_scene:
            self.sge_scene.request_destroy_node(node)
            continue

    # Validate all objects in current selection
    for obj in bpy.context.selected_objects:
        validate_node(self.sge_scene, obj)

    # Save the current selection
    Globals.selected_objects = [obj.sge_node_id for obj in bpy.context.selected_objects]

    # Check name on active object
    active_obj = bpy.context.active_object
    if active_obj is not None:
        active_obj_node = self.sge_scene.get_node(active_obj.sge_node_id)
        if active_obj_node.name != active_obj.name:
            active_obj_node.name = active_obj.name
            self.sge_scene.mark_name_dirty(active_obj_node)

    # Make sure we don't perform real-time updates too frequently and overload the server
    if time.time() - Globals.last_low_priority_update < bpy.context.scene.singed.sge_realtime_update_delay:
        return

    for obj in bpy.context.selected_objects:
        node_id = obj.sge_node_id
        node = self.sge_scene.get_node(node_id)

        # Update translation properties as necessary (swizzle components)
        updated = False
        if node.local_position[0] != -obj.location[0]:
            node.local_position[0] = -obj.location[0]
            updated = True
        if node.local_position[1] != obj.location[2]:
            node.local_position[1] = obj.location[2]
            updated = True
        if node.local_position[2] != obj.location[1]:
            node.local_position[2] = obj.location[1]
            updated = True

        if node.local_scale[0] != obj.scale[0]:
            node.local_scale[0] = obj.scale[0]
            updated = True
        if node.local_scale[1] != obj.scale[2]:
            node.local_scale[1] = obj.scale[2]
            updated = True
        if node.local_scale[2] != obj.scale[1]:
            node.local_scale[2] = obj.scale[1]
            updated = True

        # Update rotation properties as necessary (swizzle components)
        rot_mode = obj.rotation_mode
        obj.rotation_mode = 'QUATERNION'
        if not math.isclose(node.local_rotation[0], -obj.rotation_quaternion[1], rel_tol=1e-3):
            node.local_rotation[0] = -obj.rotation_quaternion[1]
            updated = True
        if not math.isclose(node.local_rotation[1], obj.rotation_quaternion[3], rel_tol=1e-3):
            node.local_rotation[1] = obj.rotation_quaternion[3]
            updated = True
        if not math.isclose(node.local_rotation[2], obj.rotation_quaternion[2], rel_tol=1e-3):
            node.local_rotation[2] = obj.rotation_quaternion[2]
            updated = True
        if not math.isclose(node.local_rotation[3], obj.rotation_quaternion[0], rel_tol=1e-3):
            node.local_rotation[3] = obj.rotation_quaternion[0]
            updated = True
        obj.rotation_mode = rot_mode

        if updated:
            self.sge_scene.mark_local_transform_dirty(node)

    Globals.last_low_priority_update = time.time()


def open_active_session(host, port):
    self = types.SinGEDProps

    if self.sge_session is not None:
        close_active_session()

    # Reset global variables
    Globals.reset()

    # Open the session
    self.sge_session = editor_session.EditorSession()
    if not self.sge_session.connect(host, port):
        self.sge_session = None
        return 'FAILURE'

    # Delete everything in the scene
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()

    # Set the undo stack size to 0, since supporting undo in blender is too finnicky (not my fault, I swear!)
    Globals.blender_old_undo_steps = bpy.context.user_preferences.edit.undo_steps
    bpy.context.user_preferences.edit.undo_steps = 0

    # Create the type database object
    self.sge_typedb = type_db.TypeDB(types.create_blender_type, ui.create_blender_component)

    # Insert all of the default types
    self.sge_typedb.insert_type('bool', types.SGEBool)
    self.sge_typedb.insert_type('int8', types.SGEInt)
    self.sge_typedb.insert_type('uint8', types.SGEUInt)
    self.sge_typedb.insert_type('int16', types.SGEInt)
    self.sge_typedb.insert_type('uint16', types.SGEUInt)
    self.sge_typedb.insert_type('int32', types.SGEInt)
    self.sge_typedb.insert_type('uint32', types.SGEUInt)
    self.sge_typedb.insert_type('int64', types.SGEInt)
    self.sge_typedb.insert_type('uint64', types.SGEUInt)
    self.sge_typedb.insert_type('float', types.SGEFloat)
    self.sge_typedb.insert_type('double', types.SGEFloat)
    self.sge_typedb.insert_type('sge::String', types.SGEString)
    self.sge_typedb.insert_type('sge::Angle', types.SGEAngle)
    self.sge_typedb.insert_type('sge::color::RGBA8', types.SGEColorRGBA8)
    self.sge_typedb.insert_type('sge::color::RGBF32', types.SGEColorRGBF32)
    self.sge_typedb.insert_type('sge::Vec2', types.SGEVec2)
    self.sge_typedb.insert_type('sge::Vec3', types.SGEVec3)

    # Add the typedb to the session object
    self.sge_typedb.register_handlers(self.sge_session)

    # Create the scene manager object
    self.sge_scene = scene_manager.SceneManager()
    self.sge_scene.set_new_node_callback(new_node_callback)
    self.sge_scene.set_update_node_callback(update_node_callback)
    self.sge_scene.set_destroy_node_callback(destroy_node_callback)
    self.sge_scene.set_new_component_callback('sge::CStaticMesh', update_node_display_callback)
    self.sge_scene.set_update_component_callback('sge::CStaticMesh', update_static_mesh_component_callback)
    self.sge_scene.set_destroy_component_callback('sge::CStaticMesh', update_node_display_callback)
    self.sge_scene.set_new_component_callback('sge::CPerspectiveCamera', update_node_display_callback)
    self.sge_scene.set_destroy_component_callback('sge::CPerspectiveCamera', update_node_display_callback)
    self.sge_scene.set_lightmaps_generated_callback(lightmaps_generated_callback)
    self.sge_scene.register_handlers(self.sge_session)

    # Create the resource manager object
    self.sge_resource_manager = resource_manager.ResourceManager(create_blender_resource)
    self.sge_resource_manager.insert_resource('BLENDER_DEFAULT_MESH', bpy.data.meshes.new('DEFAULT_MESH'))
    self.sge_resource_manager.register_handlers(self.sge_session)

    # Add the app handlers
    bpy.app.handlers.scene_update_pre.append(cycle_session)
    bpy.app.handlers.scene_update_post.append(blender_update)

    # Enable updates
    Globals.enable_update()


def close_active_session():
    self = types.SinGEDProps

    if self.sge_session is None:
        return

    # Disable updates
    Globals.disable_update()

    # Remove the app handlers
    bpy.app.handlers.scene_update_pre.remove(cycle_session)
    bpy.app.handlers.scene_update_post.remove(blender_update)

    # Close the session
    self.sge_session.close()
    self.sge_session = None

    # Unregister types in the TypeDB
    for type_name, sge_type in self.sge_typedb.types.items():
        sge_type.sge_unregister()
    self.sge_typedb = None

    # Destroy the scene manager
    self.sge_scene = None

    # Destroy the resource manager
    self.sge_resource_manager = None

    # Restore undo stack size
    bpy.context.user_preferences.edit.undo_steps = Globals.blender_old_undo_steps


def cycle_session(scene):
    # Unused arguments
    del scene

    # Make sure we're not in a critical section
    if Globals.should_update:
        types.SinGEDProps.sge_session.cycle(editor_session.EditorSession.PRIORITY_ANY)


class SinGEDConnect(Operator):
    bl_idname = 'singed.connect'
    bl_label = 'SinGED Connect'

    establish_connection = BoolProperty(default=True)

    def execute(self, context):
        if not self.establish_connection:
            close_active_session()
            return {'FINISHED'}
        elif open_active_session(context.scene.singed.sge_host, context.scene.singed.sge_port) == 'FAILURE':
            self.report(type={'ERROR'}, message="Could not connect to the server")
            return {'CANCELLED'}

        return {'FINISHED'}


# Classes
class SinGEDConnectPanel(Panel):
    bl_idname = 'singed.connect_panel'
    bl_label = 'SinGED Server Connection'
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'scene'

    def draw(self, context):
        layout = self.layout

        # Draw connect/disconnect button
        if types.SinGEDProps.sge_session is None:
            box = layout.box()
            box.prop(context.scene.singed, 'sge_host')
            box.prop(context.scene.singed, 'sge_port')
            connector = box.operator(SinGEDConnect.bl_idname, text='Connect to server')
            connector.establish_connection = True
        else:
            disconnector = layout.operator(SinGEDConnect.bl_idname, text='Disconnect from server')
            disconnector.establish_connection = False

        # Draw realtime update property
        layout.prop(context.scene.singed, 'sge_realtime_update_delay', text='Realtime Update Interval')

        if types.SinGEDProps.sge_session is not None:
            # Draw save/load scene button
            box = layout.box()
            box.prop(context.scene.singed, 'sge_scene_path')
            save_button = box.operator(operators.SinGEDSaveScene.bl_idname, text='Save Scene')
            save_button.path = context.scene.singed.sge_scene_path

            # Draw generate lightmaps button
            box = layout.box()
            box.prop(context.scene.singed, 'sge_lightmap_light_dir')
            box.prop(context.scene.singed, 'sge_lightmap_light_color')
            box.prop(context.scene.singed, 'sge_lightmap_light_intensity')
            box.prop(context.scene.singed, 'sge_lightmap_num_indirect_sample_sets')
            box.prop(context.scene.singed, 'sge_lightmap_num_accumulation_steps')
            box.prop(context.scene.singed, 'sge_lightmap_num_post_steps')
            box.prop(context.scene.singed, 'sge_lightmap_path')
            gen_lightmaps = box.operator(operators.SinGEDGenerateLightmaps.bl_idname, text='Generate Lightmaps')
            gen_lightmaps.light_dir = context.scene.singed.sge_lightmap_light_dir
            gen_lightmaps.light_color = context.scene.singed.sge_lightmap_light_color
            gen_lightmaps.light_intensity = context.scene.singed.sge_lightmap_light_intensity
            gen_lightmaps.num_indirect_sample_sets = context.scene.singed.sge_lightmap_num_indirect_sample_sets
            gen_lightmaps.num_accumulation_steps = context.scene.singed.sge_lightmap_num_accumulation_steps
            gen_lightmaps.num_post_steps = context.scene.singed.sge_lightmap_num_post_steps
            gen_lightmaps.lightmap_path = context.scene.singed.sge_lightmap_path
