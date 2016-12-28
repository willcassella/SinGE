import bpy
from bpy.types import PropertyGroup
from bpy.props import FloatProperty, PointerProperty

class SGEVec3(PropertyGroup):
    x = FloatProperty(name="x")
    y = FloatProperty(name="y")
    z = FloatProperty(name="z")
    
class SGEQuat(PropertyGroup):
    x = FloatProperty(name="x")
    y = FloatProperty(name="y")
    z = FloatProperty(name="z")
    w = FloatProperty(name="w")
    
class SGETransform3D(PropertyGroup):
    local_position = PointerProperty(name="local_position", type=SGEVec3)
    local_scale = PointerProperty(name="local_scale", type=SGEVec3)
    local_rotation = PointerProperty(name="local_rotation", type=SGEQuat)

# Panel for editing SGE component properties
class SGEComponentsPanel(bpy.types.Panel):
    bl_idname = "OBJECT_PT_sge_components"
    bl_label = "SGE Components"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"
    
    def draw(self, context):
        layout = self.layout
        
        layout.label("Transform")
        row = layout.row()
        column = row.column()
        column.label("Position")
        pos = context.object.test_property.local_position
        column.prop(pos, "x")
        column.prop(pos, "y")
        column.prop(pos, "z")

        column = row.column()
        column.label("Scale")
        scale = context.object.test_property.local_scale
        column.prop(scale, "x")
        column.prop(scale, "y")
        column.prop(scale, "z")

bpy.utils.register_class(SGEVec3)
bpy.utils.register_class(SGETransform3D)
bpy.types.Object.test_property = PointerProperty(type=SGETransform3D)
bpy.utils.register_class(SGEComponentsPanel)