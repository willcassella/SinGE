# types.py

import bpy
from bpy.types import PropertyGroup
from bpy.props import BoolProperty, IntProperty, FloatProperty, StringProperty, PointerProperty, EnumProperty

def get_unused_component_types(scene=None, context=None):
    result = []
    filter = SinGEDProps.sge_scene.get_components(bpy.context.active_object.sge_entity_id)

    for component_type in SinGEDProps.sge_typedb.component_types:
        if component_type not in filter:
            result.append((component_type, component_type, ''))

    return result

class SGETypes(PropertyGroup):
    sge_component_types = EnumProperty(items=get_unused_component_types)

class SinGEDProps(PropertyGroup):
    sge_host = StringProperty(name='Host', default='localhost')
    sge_port = IntProperty(name='Port', default=1995)
    sge_types = PointerProperty(type=SGETypes)
    sge_realtime_update_delay = FloatProperty(default=0.033)
    sge_scene_path = StringProperty(name='Path', default='')
    sge_last_realtime_update = 0
    sge_session = None
    sge_typedb = None
    sge_scene = None
    sge_resource_manager = None
    sge_selection = None

class SGETypeBase(PropertyGroup):
    sge_property_path = StringProperty()

    @classmethod
    def sge_unregister(cls):
        bpy.utils.unregister_class(cls)

    @classmethod
    def sge_create_property(cls, name):
        return PointerProperty(name=name, type=cls)

    @classmethod
    def sge_draw(cls, layout, parent_obj, parent_attr_name):
        # Draw each property recursively
        self = getattr(parent_obj, parent_attr_name)
        for prop_name, (attr_name, prop_type) in cls.sge_property_dict.items():
            # If the property is a primitive type, don't give it a label
            if not issubclass(prop_type, SGEPrimitiveBase):
                layout.label(prop_name)

            prop_type.sge_draw(layout.column(), self, attr_name)

class SGEPrimitiveBase(object):
    @staticmethod
    def sge_unregister():
        return

    @staticmethod
    def sge_draw(layout, parent_obj, parent_attr_name):
        # Draw the property
        layout.prop(parent_obj, parent_attr_name)

    @classmethod
    def sge_get(cls, name, self):
        obj = bpy.context.active_object

class SGEBool(SGEPrimitiveBase):
    @staticmethod
    def sge_get_default():
        return False

    @staticmethod
    def sge_create_property(name):
        return BoolProperty(name=name)

class SGEInt(SGEPrimitiveBase):
    @staticmethod
    def sge_get_default():
        return 0

    @staticmethod
    def sge_create_property(name):
        return IntProperty(name=name)

class SGEFloat(SGEPrimitiveBase):
    @staticmethod
    def sge_get_default():
        return 0.0

    @staticmethod
    def sge_create_property(name):
        return FloatProperty(name=name)

class SGEString(SGEPrimitiveBase):
    @staticmethod
    def sge_get_default():
        return ""

    @staticmethod
    def sge_create_property(name):
        return StringProperty(name=name)

def create_blender_type(typedb, type_name, type_info):
    # Create dictionaries for the class and the properties
    property_dict = {}
    class_dict = {
        'sge_type_name': type_name,
        'sge_property_dict': property_dict,
    }

    # Define each property
    if 'properties' in type_info:
        for prop_name, prop_info in type_info['properties'].items():
            # Get the property's type
            prop_type = typedb.get_type(prop_info['type'])

            # Create an attribute name for the property
            attr_name = "sge_prop_{}".format(prop_name)

            # Create the class dictionary entry
            class_dict[attr_name] = prop_type.sge_create_property(prop_name)

            # Create the property dictionary entry
            property_dict[prop_name] = (attr_name, prop_type)

    # Generate a sanitary name for the type
    class_name = type_name.replace("::", "_")

    # Create the type
    blender_type = type(class_name, (SGETypeBase,), class_dict)

    # Register it with Blender
    bpy.utils.register_class(blender_type)

    return blender_type
