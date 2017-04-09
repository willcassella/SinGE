# operators.py

import bpy
from bpy.types import Operator
from bpy.props import IntProperty, FloatProperty, StringProperty, FloatVectorProperty
from . import types


class SinGEDNotification(Operator):
    bl_idname = 'singed.notification'
    bl_label = 'SinGED Notification'

    message = StringProperty()

    def execute(self, context):
        self.report(type={'INFO'}, message=self.message)
        return {'FINISHED'}


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

    light_dir = FloatVectorProperty(
        name="Light direction",
        subtype='XYZ',
        size=3)

    light_color = FloatVectorProperty(
        name="Light Color",
        subtype='COLOR',
        size=3)

    light_intensity = FloatProperty(
        name="Light Intensity",
        default=1.0)

    num_indirect_sample_sets = IntProperty(
        name="Indirect sample sets",
        subtype='UNSIGNED',
        default=16)

    num_accumulation_steps = IntProperty(
        name="Accumulation steps",
        subtype='UNSIGNED',
        default=1)

    num_post_steps = IntProperty(
        name="Post-processing steps",
        subtype='UNSIGNED',
        default=2)

    lightmap_path = StringProperty(
        name="Lightmap path")

    def execute(self, context):
        # Unused arguments
        del context

        if len(self.lightmap_path) == 0:
            self.report(type={'ERROR'}, message="No lightmap path was specified")
            return {'CANCELLED'}

        types.SinGEDProps.sge_scene.generate_lightmaps(
            [self.light_dir[0], self.light_dir[1], self.light_dir[2]],
            [self.light_color[0] * self.light_intensity, self.light_color[1] * self.light_intensity, self.light_color[2] * self.light_intensity],
            self.num_indirect_sample_sets,
            self.num_accumulation_steps,
            self.num_post_steps,
            self.lightmap_path)
        return {'FINISHED'}
