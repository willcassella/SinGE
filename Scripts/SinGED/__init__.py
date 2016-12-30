# __init__.py

#    Addon info
bl_info = {
    "name": "SinGED Client",
    "author": "Will Cassella",
    "version": (0, 1),
    "blender": (2, 78, 0),
    "description": "SinGE editor client for Blender",
    "warning": "",
    "wiki_url": "",
    "category": "Development",
}

# If we're reloading the module
if "bpy" in locals():
    # Close the active session
    core.close_active_session()

    # Reimport modules
    import importlib
    importlib.reload(types)
    importlib.reload(ui)
    importlib.reload(integration)
else:
    import bpy
    from bpy.props import PointerProperty, IntProperty
    from . import types, ui, integration

# Register classes in the module
def register():
    print ("Registering ", __name__)
    bpy.utils.register_class(types.SGETypes)
    bpy.utils.register_class(types.SinGEDProps)
    bpy.utils.register_class(integration.SinGEDConnect)
    bpy.utils.register_class(integration.SinGEDConnectPanel)
    bpy.types.Scene.singed = PointerProperty(type=types.SinGEDProps)
    bpy.types.Object.sge_entity_id = IntProperty(default=0)

# Unregister classes
def unregister():
    print ("Unregistering ", __name__)

    # Close the active session
    integration.close_active_session()

    del bpy.types.Object.sge_entity_id
    del bpy.types.Scene.singed
    bpy.utils.unregister_class(integration.SinGEDConnectPanel)
    bpy.utils.unregister_class(integration.SinGEDConnect)
    bpy.utils.unregister_class(types.SinGEDProps)
    bpy.utils.unregister_class(types.SGETypes)

if __name__ == "__main__":
    register()
