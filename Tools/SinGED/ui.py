# ui.py

import bpy
from bpy.types import Panel
from bpy.props import PointerProperty
from . import types, operators


class SinGEDNodePanel(Panel):
    bl_label = 'SinGED node'
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'object'

    @classmethod
    def poll(cls, context):
        # If there is no active connection
        if types.SinGEDProps.sge_session is None:
            return False

        # If the current object does not have an node id, don't draw the panel
        if context.active_object.sge_node_id == 0:
            return False

        # Draw the panel
        return True

    def draw(self, context):
        layout = self.layout
        node_id = context.active_object.sge_node_id

        # Draw the node id
        layout.label(text="Node Id: {}".format(node_id))

        # Draw the add component box
        box = layout.box()
        if len(types.get_unused_component_types()) != 0:
            box.prop(context.scene.singed.sge_types, 'sge_component_types', text='Type')
            op = box.operator(operators.SinGEDNewComponent.bl_idname, text='Add new component')
            op.node_id = node_id
            op.component_type_name = context.scene.singed.sge_types.sge_component_types
        else:
            box.label("All component types in use by this object.")

        return


class SinGEDMaterialPanel(Panel):
    bl_label = 'SinGED Material Properties'
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'material'

    @classmethod
    def poll(cls, context):
        if context.active_object is None:
            return False

        # If the active object is not an mesh
        if context.active_object.type != 'MESH':
            return False

        # If the active object has not material
        if context.active_object.active_material is None:
            return False

        return True

    def draw(self, context):
        layout = self.layout

        # Draw the 'path' property
        layout.prop(context.active_object.active_material, 'sge_path', text="Material Path")


class SinGEDComponentPanelBase(Panel):
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'object'

    @classmethod
    def sge_unregister(cls):
        bpy.utils.unregister_class(cls)

    @classmethod
    def poll(cls, context):
        component_type_name = cls.sge_blender_type.sge_type_name
        sge_scene = types.SinGEDProps.sge_scene
        component_type = sge_scene.get_component_type(component_type_name)

        # The selection must at least contain the active object
        if len(context.selected_objects) == 0:
            selected_objects = [context.active_object]
        else:
            selected_objects = context.selected_objects

        # If any of the selected objects don't have a node id, don't draw the panel
        for obj in selected_objects:
            if obj.sge_node_id == 0:
                return False

            node = sge_scene.get_node(obj.sge_node_id)

            # If this object doesn't have this type of component attached, don't draw the panel
            if component_type.get_instance(node) is None:
                return False

        # Draw the panel
        return True

    def draw(self, context):
        layout = self.layout

        # Draw the 'remove component' button
        remove = layout.operator(operators.SinGEDDestroyComponent.bl_idname, text='Remove Component')
        remove.node_id = context.active_object.sge_node_id
        remove.component_type_name = self.sge_blender_type.sge_type_name

        # Draw the component properties
        self.sge_blender_type.sge_draw(layout, bpy.context.scene.singed.sge_types, self.sge_blender_type.__name__)


def initialize_blender_component_properties_path(obj, component_type_name, path_str):
    # Initialize the component type name and path to this object
    obj.sge_component_type_name = component_type_name
    obj.sge_property_path = path_str

    # For each property on the object
    for attr_name, prop_name, prop_type in obj.sge_property_list:
        # If the property is an object type
        if issubclass(prop_type, types.SGETypeBase):
            # Initialize it
            prop = getattr(obj, attr_name)
            if len(path_str) == 0:
                prop_path = prop_name
            else:
                prop_path = "{}.{}".format(path_str, prop_name)

            initialize_blender_component_properties_path(prop, component_type_name, prop_path)


def create_blender_component(typedb, type_name, blender_type):
    # Add the type to the types class
    setattr(types.SGETypes, blender_type.__name__, PointerProperty(type=blender_type))

    # Get the component object
    component = getattr(bpy.context.scene.singed.sge_types, blender_type.__name__)

    # Initialize the property path
    initialize_blender_component_properties_path(component, type_name, "")

    # Create a dictionary for the panel type
    panel_class_dict = {
        'bl_label': type_name,
        'sge_blender_type': blender_type,
    }

    # Create the panel type
    panel_type_name = "{}_panel".format(blender_type.__name__)
    blender_panel_type = type(panel_type_name, (SinGEDComponentPanelBase,), panel_class_dict)

    # Register it with blender
    bpy.utils.register_class(blender_panel_type)

    # Add it to the typedb
    typedb.insert_type(panel_type_name, blender_panel_type)
