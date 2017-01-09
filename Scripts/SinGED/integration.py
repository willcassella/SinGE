# integration.py

import bpy
from bpy.props import BoolProperty
from bpy.types import Operator, Panel
from . import editor_session, type_db, scene_manager, resource_manager, types, ui, static_mesh, operators
from functools import partial
import time
import socket

# Global variable to prevent scene updates during critical sections
# I have this rather than adding/removing the app handler because I don't trust how Blender handles it
sge_should_update = False

def create_blender_resource(res_manager, path, type, value):
    if type == 'sge::StaticMesh':
        return static_mesh.from_json(path, value)

def new_entity_callback(sge_scene, entity):
    # Disable scene updates
    disable_sge_update()

    # Create the object
    obj = bpy.data.objects.new(entity.name, None)
    bpy.context.scene.objects.link(obj)

    # Set the entity id on it
    obj.sge_entity_id = entity.id
    entity.user_data = obj

    # Re-enable scene updates
    enable_sge_update()

def validate_object_type(entity):
    obj = entity.user_data
    res = types.SinGEDProps.sge_resource_manager
    data = None

    # If the object is supposed to be a mesh
    if 'sge::CStaticMesh' in entity.components:
        # If it is a mesh
        if obj.type == 'MESH':
            return

        # Get the default mesh datablock
        data = res.get_resource_immediate('BLENDER_DEFAULT_MESH')

    # If none of the above are true, it's supposed to be an empty
    elif obj.type == 'EMPTY':
        return

    # Get the object's current transform
    obj.rotation_mode = 'QUATERNION'
    location = tuple(obj.location)
    scale = tuple(obj.scale)
    rotation = tuple(obj.rotation_quaternion)

    # Delete the object
    bpy.ops.object.select_all(action='DESELECT')
    obj.select = True
    bpy.ops.object.delete()

    # Create a new object
    obj = bpy.data.objects.new(entity.name, data)
    bpy.context.scene.objects.link(obj)
    bpy.context.scene.objects.active = obj
    obj.select = True

    # Restore entity data
    obj.sge_entity_id = entity.id
    entity.user_data = obj

    # Restore the transform
    obj.rotation_mode = 'QUATERNION'
    obj.location = location
    obj.scale = scale
    obj.rotation_quaternion = rotation

def update_transform_component_callback(sge_scene, entity, value):
    obj = entity.user_data

    # Assign local position (swizzle components)
    obj.location[0] = -value['local_position']['x']
    obj.location[2] = value['local_position']['y']
    obj.location[1] = value['local_position']['z']

    # Assign local scale (swizzle components)
    obj.scale[0] = value['local_scale']['x']
    obj.scale[2] = value['local_scale']['y']
    obj.scale[1] = value['local_scale']['z']

    # Assign local rotation (swizzle components)
    obj.rotation_mode = 'QUATERNION'
    obj.rotation_quaternion[0] = value['local_rotation']['w']
    obj.rotation_quaternion[1] = -value['local_rotation']['x']
    obj.rotation_quaternion[3] = value['local_rotation']['y']
    obj.rotation_quaternion[2] = value['local_rotation']['z']

def update_static_mesh_component_callback(sge_scene, entity, value):
    # Disable scene updates
    disable_sge_update()

    # Make sure the object is a mesh
    validate_object_type(entity)
    obj = entity.user_data
    assert(obj.type == 'MESH')

    # Callback to set the mesh on the object
    def set_mesh(res, path, mesh):
        nonlocal obj
        obj.data = mesh

    res = types.SinGEDProps.sge_resource_manager
    res.get_resource_async(value['mesh'], 'sge::StaticMesh', set_mesh)
    enable_sge_update()

def destroy_static_mesh_component_callback(sge_scene, entity, value):
    obj = entity.user_data

    # Disable scene updates
    disable_sge_update()

    # Update object type
    validate_object_type(entity)

    # Re-enable updates
    enable_sge_update()

def validate_entity(sge_scene, obj):
    entity_id = obj.sge_entity_id

    # If the object is brand new
    if entity_id == 0:
        # Validate the parent
        if obj.parent is not None:
            validate_entity(sge_scene, obj.parent)

        # Create a new entity for it
        entity_id = sge_scene.request_new_entity(obj)
        obj.sge_entity_id = entity_id
        sge_scene.set_entity_name(entity_id, obj.name)
        # TODO: Set parent

        # Give it a transform component, and set the value
        sge_scene.request_new_component(entity_id, 'sge::CTransform3D')
        sge_scene.set_component_value(entity_id, 'sge::CTransform3D', {
            'local_position': {
                'x': -obj.location[0],
                'y': obj.location[2],
                'z': obj.location[1],
            },
            'local_scale': {
                'x': obj.scale[0],
                'y': obj.scale[2],
                'z': obj.scale[1],
            },
            'local_rotation': {
                'w': obj.rotation_quaternion[0],
                'x': -obj.rotation_quaternion[1],
                'y': obj.rotation_quaternion[3],
                'z': obj.rotation_quaternion[2],
            },
        })
        return

    # The object is a duplicate, so create a new entity and copy everything from the old one
    if sge_scene.get_entity_userdata(entity_id) != obj:

        # Validate the parent
        if obj.parent is not None:
            validate_entity(scene, obj.parent)
            parent_id = obj.parent.sge_entity_id
        else:
            parent_id = None

        print("Creating duplicate entity...")
        old_entity_id = entity_id

        # Create the new entity object
        entity_id = sge_scene.request_new_entity(obj)
        sge_scene.set_entity_name(entity_id, obj.name)
        #sge_scene.set_entity_parent(entity_id, parent_id)
        obj.sge_entity_id = entity_id

        # For each component on the original entity
        for component_type in sge_scene.get_components(old_entity_id):
            # Get the value on the old entity
            component_value = sge_scene.get_component_value(old_entity_id, component_type)

            # Create the same type of component on the new entity
            sge_scene.request_new_component(entity_id, component_type)

            # Set the component value to the same as the old entity
            sge_scene.set_component_value(entity_id, component_type, component_value)

def update_blender_entities(scene):
    global sge_should_update
    self = types.SinGEDProps

    # Make sure we're not in a critical section
    if not sge_should_update:
        return

    # Check previously selected objects
    for entity_id in self.sge_selection:
        # If the object was deleted
        if scene not in self.sge_scene.get_entity_userdata(entity_id).users_scene:
            self.sge_scene.request_destroy_entity(entity_id)
            continue

    # Save the current selection
    self.sge_selection = [obj.sge_entity_id for obj in bpy.context.selected_objects]

    # Validate all objects in current selection
    for obj in bpy.context.selected_objects:
       validate_entity(self.sge_scene, obj)

    # Check name on active object
    active_entity = bpy.context.active_object
    if active_entity is not None and active_entity.name != self.sge_scene.get_entity_name(active_entity.sge_entity_id):
        self.sge_scene.set_entity_name(active_entity.sge_entity_id, active_entity.name)

    # Make sure we don't perform realtime updates too frequently and overload the server
    if time.time() - self.sge_last_realtime_update < bpy.context.scene.singed.sge_realtime_update_delay:
        return

    for obj in bpy.context.selected_objects:
        entity_id = obj.sge_entity_id

        # Helper function binding to get a transform property
        transform_getter = partial(self.sge_scene.get_property_value, entity_id, 'sge::CTransform3D', [])

        # Access the transform properties
        local_position = transform_getter('local_position')
        if local_position is None:
            # Component hasn't been loaded yet
            continue

        local_scale = transform_getter('local_scale')
        local_rotation = transform_getter('local_rotation')

        # Helper function binding to set a transform property
        transform_setter = partial(self.sge_scene.set_property_value, entity_id, 'sge::CTransform3D')

        # Update translation properties as necessary (swizzle components)
        if obj.location[0] != -local_position['x']:
            transform_setter(['local_position'], 'x', -obj.location[0])
        if obj.location[2] != local_position['y']:
            transform_setter(['local_position'], 'y', obj.location[2])
        if obj.location[1] != local_position['z']:
            transform_setter(['local_position'], 'z', obj.location[1])

        # Update scale properties as necessary (swizzle components)
        if obj.scale[0] != local_scale['x']:
            transform_setter(['local_scale'], 'x', obj.scale[0])
        if obj.scale[2] != local_scale['y']:
            transform_setter(['local_scale'], 'y', obj.scale[2])
        if obj.scale[1] != local_scale['z']:
            transform_setter(['local_scale'], 'z', obj.scale[1])

        # Update rotation properties as necessary (swizzle components)
        obj.rotation_mode = 'QUATERNION'
        if obj.rotation_quaternion[0] != local_rotation['w']:
            transform_setter(['local_rotation'], 'w', obj.rotation_quaternion[0])
        if obj.rotation_quaternion[1] != -local_rotation['x']:
            transform_setter(['local_rotation'], 'x', -obj.rotation_quaternion[1])
        if obj.rotation_quaternion[3] != local_rotation['y']:
            transform_setter(['local_rotation'], 'y', obj.rotation_quaternion[3])
        if obj.rotation_quaternion[2] != local_rotation['z']:
            transform_setter(['local_rotation'], 'z', obj.rotation_quaternion[2])

    self.sge_last_realtime_update = time.time()

def open_active_session(host, port):
    self = types.SinGEDProps

    if self.sge_session is not None:
        close_active_session()

    # Delete everything in the scene
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()

    # Open the session
    self.sge_session = editor_session.EditorSession(host, port)

    # Create the type database object
    self.sge_typedb = type_db.TypeDB(types.create_blender_type, ui.create_blender_component)

    # Insert all of the default types
    self.sge_typedb.insert_type('bool', types.SGEBool)
    self.sge_typedb.insert_type('int8', types.SGEInt)
    self.sge_typedb.insert_type('uint8', types.SGEInt)
    self.sge_typedb.insert_type('int16', types.SGEInt)
    self.sge_typedb.insert_type('uint16', types.SGEInt)
    self.sge_typedb.insert_type('int32', types.SGEInt)
    self.sge_typedb.insert_type('uint32', types.SGEInt)
    self.sge_typedb.insert_type('int64', types.SGEInt)
    self.sge_typedb.insert_type('uint64', types.SGEInt)
    self.sge_typedb.insert_type('float', types.SGEFloat)
    self.sge_typedb.insert_type('double', types.SGEFloat)
    self.sge_typedb.insert_type('sge::String', types.SGEString)

    # Add the typedb to the session object
    self.sge_typedb.register_handlers(self.sge_session)

    # Create the scene manager object
    self.sge_scene = scene_manager.SceneManager()
    self.sge_scene.new_entity_callback(new_entity_callback)
    self.sge_scene.update_component_callback('sge::CTransform3D', update_transform_component_callback)
    self.sge_scene.update_component_callback('sge::CStaticMesh', update_static_mesh_component_callback)
    self.sge_scene.destroy_component_callback('sge::CStaticMesh', destroy_static_mesh_component_callback)
    self.sge_scene.register_handlers(self.sge_session)

    # Create the resource manager object
    self.sge_resource_manager = resource_manager.ResourceManager(create_blender_resource)
    self.sge_resource_manager.insert_resource('BLENDER_DEFAULT_MESH', bpy.data.meshes.new('DEFAULT_MESH'))
    self.sge_resource_manager.register_handlers(self.sge_session)

    # Create an empty selection list
    self.sge_selection = []

    # Add the app handlers
    bpy.app.handlers.scene_update_pre.append(cycle_session)
    bpy.app.handlers.scene_update_post.append(update_blender_entities)

    # Enable updates
    enable_sge_update()

def close_active_session():
    self = types.SinGEDProps

    if self.sge_session is None:
        return

    # Disable updates
    disable_sge_update()

    # Remove the app handlers
    bpy.app.handlers.scene_update_pre.remove(cycle_session)
    bpy.app.handlers.scene_update_post.remove(update_blender_entities)

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

    # Destroy the selection list
    self.sge_selection = None

def cycle_session(scene):
    global sge_should_update

    # Make sure we're not in a critical section
    if sge_should_update:
        types.SinGEDProps.sge_session.cycle()

def enable_sge_update():
    global sge_should_update
    sge_should_update = True

def disable_sge_update():
    global sge_should_update
    sge_should_update = False

class SinGEDConnect(Operator):
    bl_idname = 'singed.connect'
    bl_label = 'SinGED Connect'

    establish_connection = BoolProperty(default=True)

    def execute(self, context):
        if not self.establish_connection:
            close_active_session()
            return {'FINISHED'}

        try:
            open_active_session(context.scene.singed.sge_host, context.scene.singed.sge_port)
        except socket.timeout as e:
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
        layout.split()
        layout.prop(context.scene.singed, 'sge_realtime_update_delay', text='Realtime Update Delay')

        # Draw save/load scene button
        if types.SinGEDProps.sge_session is not None:
            layout.split()
            layout.prop(context.scene.singed, 'sge_scene_path')
            save_button = layout.operator(operators.SinGEDSaveScene.bl_idname, text='Save Scene')
            save_button.path = context.scene.singed.sge_scene_path
