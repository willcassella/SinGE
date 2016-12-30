# integration.py

import bpy
from bpy.props import BoolProperty
from bpy.types import Operator, Panel
from . import editor_session, type_db, scene_manager, types, ui
from functools import partial
import time

def create_blender_entity(scene, entity_id, name, components):
    # Create the object and add it to the scene
    obj = bpy.data.objects.new(name, None)
    bpy.context.scene.objects.link(obj)
    obj.sge_entity_id = entity_id

    # Add a query to get the object's position, since
    scene.add_get_component_query(entity_id, 'sge::CTransform3D')
    return obj

def transform_blender_entity(scene, entity_id, value):
    obj = scene.user_entity_data[entity_id]
    obj.location[0] = -value['local_position']['x']
    obj.location[2] = value['local_position']['y']
    obj.location[1] = value['local_position']['z']

def update_blender_entity_transform(scene):
    self = types.SinGEDProps

    # Make sure we don't perform realtime updates too frequently and overload the server
    if time.time() - self.sge_last_realtime_update < self.sge_realtime_update_delay:
        return
    else:
        self.sge_last_realtime_update = time.time()

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

        # Helper function binding to set a transform property
        transform_setter = partial(self.sge_scene.set_property_value, obj.sge_entity_id, 'sge::CTransform3D')

        # If the object has been moved in blender, update the property
        if obj.location[0] != -local_position['x']:
            transform_setter(['local_position'], 'x', -obj.location[0])
        if obj.location[2] != local_position['y']:
            transform_setter(['local_position'], 'y', obj.location[2])
        if obj.location[1] != local_position['z']:
            transform_setter(['local_position'], 'z', obj.location[1])

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
    self.sge_typedb.insert_type('std::string', types.SGEString)

    # Add the typedb to the session object
    self.sge_typedb.register_handlers(self.sge_session)

    # Create the scene manager object
    self.sge_scene = scene_manager.SceneManager(create_blender_entity)
    self.sge_scene.add_get_component_callback('sge::CTransform3D', transform_blender_entity)
    self.sge_scene.register_handlers(self.sge_session)

    # Tell blender to update the session
    bpy.app.handlers.scene_update_pre.append(cycle_session)
    bpy.app.handlers.scene_update_post.append(update_blender_entity_transform)

def close_active_session():
    self = types.SinGEDProps

    if self.sge_session is None:
        return

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
        if self.establish_connection:
            open_active_session(context.scene.singed.sge_host, context.scene.singed.sge_port)
        else:
            close_active_session()
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
            layout.prop(context.scene.singed, 'sge_host')
            layout.prop(context.scene.singed, 'sge_port')
            connector = layout.operator(SinGEDConnect.bl_idname, text='Connect to server')
            connector.establish_connection = True
        else:
            disconnector = layout.operator(SinGEDConnect.bl_idname, text='Disconnect from server')
            disconnector.establish_connection = False
