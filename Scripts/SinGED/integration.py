# integration.py

import bpy
from bpy.props import BoolProperty
from bpy.types import Operator, Panel
from . import editor_session, type_db, scene_manager, resource_manager, types, ui, static_mesh
from functools import partial
import time
import socket

def create_blender_resource(res_manager, path, type, value):
    if type == 'sge::StaticMesh':
        return static_mesh.from_json(path, value)

def create_blender_entity(scene, entity_id, name, components):
    # Create the object
    if 'sge::CStaticMesh' in components:
        bpy.ops.mesh.primitive_cube_add()
        obj = bpy.context.active_object
        obj.name = name
    else:
        obj = bpy.data.objects.new(name, None)
        bpy.context.scene.objects.link(obj)

    # Set the entity id on it
    obj.sge_entity_id = entity_id

    # Add a query to get all component data
    for component in components:
        scene.add_get_component_query(entity_id, component)
    return obj

def blender_static_mesh(scene, entity_id, value):
    obj = scene.user_entity_data[entity_id]
    res = types.SinGEDProps.sge_resource_manager

    def set_mesh(path, mesh):
        nonlocal obj
        obj.data = mesh

    res.get_resource(value['mesh'], 'sge::StaticMesh', set_mesh)

def transform_blender_entity(scene, entity_id, value):
    obj = scene.user_entity_data[entity_id]

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

def update_blender_entity_transform(scene):
    self = types.SinGEDProps

    # Make sure we don't perform realtime updates too frequently and overload the server
    if time.time() - self.sge_last_realtime_update < bpy.context.scene.singed.sge_realtime_update_delay:
        return

    for obj in bpy.context.selected_objects:
        if obj.sge_entity_id == 0:
            # New object, need to add to the scene TODO
            continue

        # Helper function binding to get a transform property
        transform_getter = partial(self.sge_scene.get_property_value, obj.sge_entity_id, 'sge::CTransform3D', [])

        # Access the transform properties
        local_position = transform_getter('local_position')
        if local_position is None:
            # Component hasn't been loaded yet
            continue
        local_scale = transform_getter('local_scale')
        local_rotation = transform_getter('local_rotation')

        # Helper function binding to set a transform property
        transform_setter = partial(self.sge_scene.set_property_value, obj.sge_entity_id, 'sge::CTransform3D')

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
        if obj.rotation_quaternion[1] != local_rotation['x']:
            transform_setter(['local_rotation'], 'x', -obj.rotation_quaternion[1])
        if obj.rotation_quaternion[3] != -local_rotation['y']:
            transform_setter(['local_rotation'], 'y', obj.rotation_quaternion[3])
        if obj.rotation_quaternion[2] != local_rotation['z']:
            transform_setter(['local_rotation'], 'z', obj.rotation_quaternion[2])

    self.sge_last_realtime_update = time.time()

def open_active_session(host, port):
    self = types.SinGEDProps

    if self.sge_session is not None:
        close_active_session()

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
    self.sge_scene = scene_manager.SceneManager(create_blender_entity)
    self.sge_scene.add_get_component_callback('sge::CTransform3D', transform_blender_entity)
    self.sge_scene.add_get_component_callback('sge::CStaticMesh', blender_static_mesh)
    self.sge_scene.register_handlers(self.sge_session)

    # Create the resource manager object
    self.sge_resource_manager = resource_manager.ResourceManager(create_blender_resource)
    self.sge_resource_manager.register_handlers(self.sge_session)

    # Tell blender to update the session
    bpy.app.handlers.scene_update_pre.append(cycle_session)
    bpy.app.handlers.scene_update_post.append(update_blender_entity_transform)

def close_active_session():
    self = types.SinGEDProps

    if self.sge_session is None:
        return

    # Remove blender app handlers
    bpy.app.handlers.scene_update_pre.remove(cycle_session)
    bpy.app.handlers.scene_update_post.remove(update_blender_entity_transform)

    # Close the session
    self.sge_session.close()
    self.sge_session = None

    # Unregister types in the TypeDB
    for type_name, sge_type in self.sge_typedb.types.items():
        sge_type.sge_unregister()
    self.sge_typedb = None

    # Destroy the scene manager
    self.sge_scene = None

def cycle_session(scene):
    self = types.SinGEDProps

    if self.sge_session is not None:
        self.sge_session.cycle()

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
        if types.SinGEDProps.sge_session is None:
            box = layout.box()
            box.prop(context.scene.singed, 'sge_host')
            box.prop(context.scene.singed, 'sge_port')
            connector = box.operator(SinGEDConnect.bl_idname, text='Connect to server')
            connector.establish_connection = True
        else:
            disconnector = layout.operator(SinGEDConnect.bl_idname, text='Disconnect from server')
            disconnector.establish_connection = False
        layout.split()
        layout.prop(context.scene.singed, 'sge_realtime_update_delay', text='Realtime Update Delay')
