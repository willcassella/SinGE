# operators.py

import bpy
from bpy.types import Operator
from bpy.props import IntProperty, StringProperty
from . import types


class SinGEDNewComponent(Operator):
    bl_idname = 'singed.new_component'
    bl_label = 'SinGED New Component'

    node_id = IntProperty(name='Node Id')
    component_type_name = StringProperty(name='Type')

    def execute(self, context):
        # Unused arguments
        del context

        sge_scene = types.SinGEDProps.sge_scene
        node = sge_scene.get_node(self.node_id)
        component_type = sge_scene.get_component_type(self.component_type_name)
        component_type.request_new_instance(node)
        return {'FINISHED'}


class SinGEDDestroyComponent(Operator):
    bl_idname = 'singed.destroy_component'
    bl_label = 'SinGED Destroy Component'

    node_id = IntProperty(name='Node Id')
    component_type_name = StringProperty(name='Type')

    def execute(self, context):
        # Unused arguments
        del context

        sge_scene = types.SinGEDProps.sge_scene
        node = sge_scene.get_node(self.node_id)
        component_type = sge_scene.get_component_type(self.component_type_name)
        component_type.request_destroy_instance(node)
        return {'FINISHED'}


class SinGEDSaveScene(Operator):
    bl_idname = 'singed.save_scene'
    bl_label = 'SinGED Save Scene'

    path = StringProperty(name='Path')

    def execute(self, context):
        # Unused arguments
        del context

        types.SinGEDProps.sge_scene.save_scene(self.path)
        return {'FINISHED'}


class SinGEDGenerateLightmaps(Operator):
    bl_idname = 'singed.generate_lightmaps'
    bl_label = 'SinGED Generate Lightmaps'

    def execute(self, context):
        # Unused arguments
        del context

        types.SinGEDProps.sge_scene.generate_lightmaps()
        return {'FINISHED'}
