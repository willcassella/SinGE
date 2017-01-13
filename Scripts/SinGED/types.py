# types.py

import bpy
from bpy.types import PropertyGroup
from bpy.props import BoolProperty, IntProperty, FloatProperty, StringProperty, PointerProperty, EnumProperty, FloatVectorProperty
from functools import partial

def get_unused_component_types(scene=None, context=None):
    result = []
    filter = SinGEDProps.sge_scene.get_components(bpy.context.active_object.sge_entity_id)

    for component_type in SinGEDProps.sge_typedb.component_types:
        if component_type not in filter:
            result.append((component_type, component_type, ''))

    return result

def property_getter(self, name, default):
    # Get the component type and property path
    property_path = self.sge_property_path.split('.')
    component_type = property_path[0]
    property_path = property_path[1: ]

    # Get the active object entity id
    entity_id = bpy.context.active_object.sge_entity_id

    # Get the property value
    value = SinGEDProps.sge_scene.get_property_value(entity_id, component_type, property_path, name)

    # Returnt the default if there is no value
    if value is None:
        return default
    return value

def property_setter(self, name, value):
    # Get the component type and property path
    property_path = self.sge_property_path.split('.')
    component_type = property_path[0]
    property_path = property_path[1: ]

    # Get the active object entity id
    entity_id = bpy.context.active_object.sge_entity_id

    # Set the property value
    SinGEDProps.sge_scene.set_property_value(entity_id, component_type, property_path, name, value)

    # Add a query to retrieve the component value
    SinGEDProps.sge_scene.add_get_component_query(entity_id, component_type)

class SGETypes(PropertyGroup):
    sge_component_types = EnumProperty(items=get_unused_component_types)

class SinGEDProps(PropertyGroup):
    sge_host = StringProperty(name='Host', default='localhost')
    sge_port = IntProperty(name='Port', default=1995)
    sge_types = PointerProperty(type=SGETypes)
    sge_realtime_update_delay = FloatProperty(default=0.033, precision=3, unit='TIME')
    sge_scene_path = StringProperty(name='Path', default='')
    sge_last_realtime_update = 0
    sge_session = None
    sge_typedb = None
    sge_scene = None
    sge_resource_manager = None
    sge_selection = None

class SGETypeBase(PropertyGroup):
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

class SGEBool(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return FloatProperty(
            name=name,
            get=lambda self: property_getter(self, name, False),
            set=lambda self, value: property_setter(self, name, value))

class SGEInt(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return IntProperty(
            name=name,
            get=lambda self: property_getter(self, name, 0),
            set=lambda self, value: property_setter(self, name, value))

class SGEUInt(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return IntProperty(
            name=name,
            subtype='UNSIGNED',
            get=lambda self: property_getter(self, name, 0),
            set=lambda self, value: property_setter(self, name, value))

class SGEFloat(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return FloatProperty(
            name=name,
            get=lambda self: property_getter(self, name, 0.0),
            set=lambda self, value: property_setter(self, name, value))

class SGEString(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return StringProperty(
            name=name,
            get=lambda self: property_getter(self, name, ""),
            set=lambda self, value: property_setter(self, name, value))

class SGEColorRGBA8(SGEPrimitiveBase):
    @staticmethod
    def sge_get(self, name):
        value = property_getter(self, name, "ffffffff")
        red = int(value[: 2], 16)
        green = int(value[2: 4], 16)
        blue = int(value[4: 6], 16)
        alpha = int(value[6: 8], 16)
        return [float(red)/255, float(green)/255, float(blue)/255, float(alpha)/255]

    def sge_set(self, name, value):
        red = int(value[0] * 255)
        green = int(value[1] * 255)
        blue = int(value[2] * 255)
        alpha = int(value[3] * 255)
        property_setter(self, name, "%0.2x%0.2x%0.2x%0.2x" % (red, green, blue, alpha))

    @staticmethod
    def sge_create_property(name):
        return FloatVectorProperty(
            name=name,
            subtype='COLOR',
            size=4,
            min=0.0,
            max=1.0,
            get=lambda self: SGEColorRGBA8.sge_get(self, name),
            set=lambda self, value: SGEColorRGBA8.sge_set(self, name, value))

def create_blender_type(typedb, type_name, type_info):
    # Create dictionaries for the class and the properties
    property_dict = {}
    class_dict = {
        'sge_type_name': type_name,
        'sge_property_dict': property_dict,
        'sge_property_path': StringProperty(),
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
