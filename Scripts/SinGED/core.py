# core.py

import bpy
from bpy.types import PropertyGroup, Menu, Panel
from bpy.props import BoolProperty, IntProperty, FloatProperty, StringProperty, PointerProperty
from . import EditorSession, TypeDB

class SGETypes(PropertyGroup):
    """Nothing here"""

class SinGEDProps(PropertyGroup):
    host = StringProperty(name='Host', default='localhost')
    port = IntProperty(name='Port', default=1995)
    types = PointerProperty(type=SGETypes)

class SGETypeBase(PropertyGroup):
    @classmethod
    def unregister(cls):
        bpy.utils.unregister_class(cls)

    @classmethod
    def create_property(cls, name):
        return PointerProperty(name=name, type=cls)

    @classmethod
    def draw(cls, layout, parent_obj, parent_attr_name):
        # Draw each property recursively
        self = getattr(parent_obj, parent_attr_name)
        for prop_name, (attr_name, prop_type) in cls.property_dict.items():
            # If the property is a primitive type, don't give it a label
            if not issubclass(prop_type, SGEPrimitiveBase):
                layout.label(prop_name)

            prop_type.draw(layout.column(), self, attr_name)

class SGEPrimitiveBase(object):
    @staticmethod
    def unregister():
        return

    @staticmethod
    def draw(layout, parent_obj, parent_attr_name):
        # Draw the property
        layout.prop(parent_obj, parent_attr_name)

class SGEBool(SGEPrimitiveBase):
    @staticmethod
    def create_property(name):
        return BoolProperty(name=name)

class SGEInt(SGEPrimitiveBase):
    @staticmethod
    def create_property(name):
        return IntProperty(name=name)

class SGEFloat(SGEPrimitiveBase):
    @staticmethod
    def create_property(name):
        return FloatProperty(name=name)

class SGEString(SGEPrimitiveBase):
    @staticmethod
    def create_property(name):
        return StringProperty(name=name)

class SinGEDComponentPanelBase(Panel):
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'object'

    @classmethod
    def unregister(cls):
        bpy.utils.unregister_class(cls)

    def draw(self, context):
        self.type_object.draw(self.layout, bpy.context.scene.singed.types, self.type_object.__name__)

def create_blender_type(typedb, type_name, type_info):
    # Create dictionaries for the class and the properties
    property_dict = {}
    class_dict = {
        "property_dict": property_dict
    }

    # Define each property
    if 'properties' in type_info:
        for prop_name, prop_info in type_info['properties'].items():
            # Get the property's type
            prop_type = typedb.get_type(prop_info['type'])

            # Create a name for the property
            dict_prop_name = "prop_{}".format(prop_name)

            # Create the class dictionary entry
            class_dict[dict_prop_name] = prop_type.create_property(prop_name)

            # Create the property dictionary entry
            property_dict[prop_name] = (dict_prop_name, prop_type)

    # Generate a sanitary name for the type
    class_name = type_name.replace("::", "_")

    # Create the type
    sge_type = type(class_name, (SGETypeBase,), class_dict)

    # Register it with Blender
    bpy.utils.register_class(sge_type)

    return sge_type

def create_blender_component(typedb, type_name, type_object):
     # Add the type to the types class
    setattr(SGETypes, type_object.__name__, PointerProperty(type=type_object))

    # Create a dictionary for the panel type
    panel_class_dict = {
        'type_object': type_object,
        'bl_label': type_name
    }

    # Create the panel type
    panel_type_name = "{}_panel".format(type_object.__name__)
    panel_type_object = type(panel_type_name, (SinGEDComponentPanelBase,), panel_class_dict)

    # Register it with blender
    bpy.utils.register_class(panel_type_object)

    # Add it to the typedb
    typedb.insert_type(panel_type_name, panel_type_object)

# Global variables
active_session = None
typedb = None

def close_active_session():
    global active_session, typedb
    if active_session is None:
        return

    # Close the session
    active_session.close()
    active_session = None

    # Unregister types in the TypeDB
    for type_name,sge_type in typedb.types.items():
        sge_type.unregister()

    typedb = None

def open_active_session(host, port):
    global active_session, typedb
    if active_session is not None:
        close_active_session()

    # Open the session
    active_session = EditorSession.EditorSession(host, port)

    # Create the type database object
    typedb = TypeDB.TypeDB(create_blender_type, create_blender_component)

    # Insert all of the default types
    typedb.insert_type('bool', SGEBool)
    typedb.insert_type('int8', SGEInt)
    typedb.insert_type('uint8', SGEInt)
    typedb.insert_type('int16', SGEInt)
    typedb.insert_type('uint16', SGEInt)
    typedb.insert_type('int32', SGEInt)
    typedb.insert_type('uint32', SGEInt)
    typedb.insert_type('int64', SGEInt)
    typedb.insert_type('uint64', SGEInt)
    typedb.insert_type('float', SGEFloat)
    typedb.insert_type('double', SGEFloat)
    typedb.insert_type('std::string', SGEString)

    # Add the typedb to the session object
    active_session.add_handler('get_type_info', typedb)
    active_session.add_handler('get_component_types', typedb)

    # Cycle the session a few times
    for i in range(3):
        active_session.cycle()
