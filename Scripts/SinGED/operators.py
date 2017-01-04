# operators.py

import bpy
from bpy.types import Operator
from bpy.props import IntProperty, StringProperty
from . import types

def sge_new_component(entity_id, component_type):
    types.SinGEDProps.sge_scene.new_component(entity_id, component_type)
    types.SinGEDProps.sge_scene.add_get_component_query(entity_id, component_type)
    return

class SinGEDNewComponent(Operator):
    bl_idname = 'singed.new_component'
    bl_label = 'SinGED New Component'

    entity_id = IntProperty(name='Entity Id')
    component_type = StringProperty(name='Type')

    def execute(self, context):
        sge_new_component(self.entity_id, self.component_type)
        return {'FINISHED'}
