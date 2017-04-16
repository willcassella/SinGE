# types.py

import bpy
from bpy.types import PropertyGroup
from bpy.props import BoolProperty, IntProperty, FloatProperty, StringProperty, PointerProperty, EnumProperty, FloatVectorProperty
from functools import partial


def get_unused_component_types(scene=None, context=None):
    # Unused arguments
    del scene, context
    node_id = bpy.context.active_object.sge_node_id
    sge_scene = SinGEDProps.sge_scene
    node = sge_scene.get_node(node_id)

    used_component = sge_scene.get_node_components(node)
    result = []
    for component_type in SinGEDProps.sge_typedb.component_types:
        if component_type not in (c.type.type_name for c in used_component):
            result.append((component_type, component_type, ''))

    return result


def construct_property_display_name(prop_name):
    return prop_name.replace("_", " ")


def construct_property_path(property_path_str, prop_name):
    if len(property_path_str) == 0:
        return [prop_name]
    return property_path_str.split('.') + [prop_name]


def property_getter(component_type_name, property_path, default):
    try:
        # Get the active node and component instance
        sge_scene = SinGEDProps.sge_scene
        node_id = bpy.context.active_object.sge_node_id
        node = sge_scene.get_node(node_id)
        component_type = sge_scene.get_component_type(component_type_name)
        component_instance = component_type.get_instance(node)

        # Get the property value
        return component_instance.get_sub_property_immediate(property_path, default)
    except Exception:
        path = [component_type_name]
        path.extend(property_path)
        print("ERROR RETREIVING PROPERTY: {}".format(path))
        return default

def property_setter(component_type_name, property_path, value):
    # Get the active node and component instance
    sge_scene = SinGEDProps.sge_scene
    node_id = bpy.context.active_object.sge_node_id
    node = sge_scene.get_node(node_id)
    component_type = sge_scene.get_component_type(component_type_name)
    component_instance = component_type.get_instance(node)

    # Set the property value
    component_instance.set_sub_property_immediate(property_path, value)


class SGETypes(PropertyGroup):
    sge_component_types = EnumProperty(items=get_unused_component_types)


class SinGEDProps(PropertyGroup):
    sge_host = StringProperty(name='Host', default='localhost')
    sge_port = IntProperty(name='Port', default=1995)
    sge_types = PointerProperty(type=SGETypes)
    sge_realtime_update_delay = FloatProperty(default=0.033, precision=3, unit='TIME')
    sge_scene_path = StringProperty(name='Path', default='')
    sge_lightmap_light_dir = FloatVectorProperty(name="Light direction", subtype='XYZ', size=3, default=[0.0, -0.5, -0.5])
    sge_lightmap_light_color = FloatVectorProperty(name="Light Color", subtype='COLOR', size=3, default=[0.5, 0.5, 0.5])
    sge_lightmap_light_intensity = FloatProperty(name="Light intensity", default=1.0)
    sge_lightmap_num_indirect_sample_sets = IntProperty(name="Indirect sample sets", subtype='UNSIGNED', default=16)
    sge_lightmap_num_accumulation_steps = IntProperty(name="Accumulation steps", subtype='UNSIGNED', default=1)
    sge_lightmap_num_post_steps = IntProperty(name="Post processing steps", subtype='UNSIGNED', default=2)
    sge_lightmap_path = StringProperty(name="Lightmap path")
    sge_session = None
    sge_typedb = None
    sge_scene = None
    sge_resource_manager = None


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
        for attr_name, prop_name, prop_type in cls.sge_property_list:
            # If the property is a primitive type, don't give it a label
            if not issubclass(prop_type, SGEPrimitiveBase):
                layout.label(construct_property_display_name(prop_name))

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
        return BoolProperty(
            name=construct_property_display_name(name),
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), False),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEInt(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return IntProperty(
            name=construct_property_display_name(name),
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), 0),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEUInt(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return IntProperty(
            name=construct_property_display_name(name),
            subtype='UNSIGNED',
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), 0),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEFloat(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return FloatProperty(
            name=construct_property_display_name(name),
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), 0.0),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEString(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return StringProperty(
            name=construct_property_display_name(name),
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), ""),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEAngle(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return FloatProperty(
            name=construct_property_display_name(name),
            subtype='ANGLE',
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), 0),
            set=lambda outer, value: property_setter(outer.sge_component_type-Name, construct_property_path(outer.sge_property_path, name), value))


class SGEColorRGBA8(SGEPrimitiveBase):
    @staticmethod
    def sge_get(outer, prop_name):
        value = property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), "ffffffff")
        red = int(value[: 2], 16)
        green = int(value[2: 4], 16)
        blue = int(value[4: 6], 16)
        alpha = int(value[6: 8], 16)
        return [float(red)/255, float(green)/255, float(blue)/255, float(alpha)/255]

    @staticmethod
    def sge_set(outer, prop_name, value):
        red = int(value[0] * 255)
        green = int(value[1] * 255)
        blue = int(value[2] * 255)
        alpha = int(value[3] * 255)
        property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), "%0.2x%0.2x%0.2x%0.2x" % (red, green, blue, alpha))

    @staticmethod
    def sge_create_property(name):
        return FloatVectorProperty(
            name=name,
            subtype='COLOR',
            size=4,
            min=0.0,
            max=1.0,
            get=lambda outer: SGEColorRGBA8.sge_get(outer, name),
            set=lambda outer, value: SGEColorRGBA8.sge_set(outer, name, value))


class SGEColorRGBF32(SGEPrimitiveBase):
    @staticmethod
    def sge_create_property(name):
        return FloatVectorProperty(
            name=construct_property_display_name(name),
            subtype='COLOR',
            size=3,
            get=lambda outer: property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), [0.0, 0.0, 0.0]),
            set=lambda outer, value: property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, name), value))


class SGEVec2(SGEPrimitiveBase):
    @staticmethod
    def sge_get(outer, prop_name):
        value = property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), None)
        if value is None:
            return [0.0, 0.0]
        else:
            return [value['x'], value['y']]

    @staticmethod
    def sge_set(outer, prop_name, value):
        property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), {'x': value[0], 'y': value[1]})

    @staticmethod
    def sge_create_property(name):
        return FloatVectorProperty(
            name=construct_property_display_name(name),
            subtype='XYZ',
            size=2,
            get=lambda outer: SGEVec2.sge_get(outer, name),
            set=lambda outer, value: SGEVec2.sge_set(outer, name, value))


class SGEVec3(SGEPrimitiveBase):
    @staticmethod
    def sge_get(outer, prop_name):
        value = property_getter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), None)
        if value is None:
            return [0.0, 0.0, 0.0]
        else:
            return [value['x'], value['y'], value['z']]

    @staticmethod
    def sge_set(outer, prop_name, value):
        property_setter(outer.sge_component_type_name, construct_property_path(outer.sge_property_path, prop_name), {'x': value[0], 'y': value[1], 'z': value[2]})

    @staticmethod
    def sge_create_property(name):
        return FloatVectorProperty(
            name=construct_property_display_name(name),
            subtype='XYZ',
            size=3,
            get=lambda outer: SGEVec3.sge_get(outer, name),
            set=lambda outer, value: SGEVec3.sge_set(outer, name, value))


def create_blender_type(typedb, type_name, type_info):
    # Create dictionaries for the class and the properties
    property_list = list()
    class_dict = {
        'sge_type_name': type_name,
        'sge_property_list': property_list,
        'sge_component_type_name': StringProperty(),
        'sge_property_path': StringProperty(),
    }

    # Define each property
    if 'properties' in type_info:
        properties = list(type_info['properties'].items())
        properties.sort(key=lambda prop: prop[1]['index'])

        for prop_name, prop_info in properties:
            # Get the property's type
            prop_type = typedb.get_type(prop_info['type'])

            # Create an attribute name for the property
            attr_name = "sge_prop_{}".format(prop_name)

            # Create the class dictionary entry
            class_dict[attr_name] = prop_type.sge_create_property(prop_name)

            # Create the property list entry
            property_list.append((attr_name, prop_name, prop_type))

    # Generate a sanitary name for the type
    class_name = type_name.replace("::", "_")

    # Create the type
    blender_type = type(class_name, (SGETypeBase,), class_dict)

    # Register it with Blender
    bpy.utils.register_class(blender_type)

    return blender_type
