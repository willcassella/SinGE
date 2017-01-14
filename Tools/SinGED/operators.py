# operators.py

import bpy
from bpy.types import Operator
from bpy.props import IntProperty, StringProperty
from . import types

class SinGEDNewComponent(Operator):
    bl_idname = 'singed.new_component'
    bl_label = 'SinGED New Component'

    entity_id = IntProperty(name='Entity Id')
    component_type = StringProperty(name='Type')

    def execute(self, context):
        types.SinGEDProps.sge_scene.request_new_component(self.entity_id, self.component_type)
        return {'FINISHED'}

class SinGEDDestroyComponent(Operator):
    bl_idname = 'singed.destroy_component'
    bl_label = 'SinGED Destroy Component'

    entity_id = IntProperty(name='Entity Id')
    component_type = StringProperty(name='Type')

    def execute(self, context):
        types.SinGEDProps.sge_scene.request_destroy_component(self.entity_id, self.component_type)
        return {'FINISHED'}

class SinGEDSaveScene(Operator):
    bl_idname = 'singed.save_scene'
    bl_label = 'SinGED Save Scene'

    path = StringProperty(name='Path')

    def execute(self, context):
        types.SinGEDProps.sge_scene.save_scene(self.path)
        return {'FINISHED'}
