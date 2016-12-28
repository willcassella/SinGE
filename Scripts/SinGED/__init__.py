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
    "category": "Development"
}

# If we're reloading the module
if "bpy" in locals():
    # Close the active session
    core.close_active_session()

    # Reimport modules
    import importlib
    importlib.reload(core)
    importlib.reload(operators)
    importlib.reload(ui)
else:
    import bpy
    from bpy.props import PointerProperty
    from . import core, operators, ui

# Register classes in the module
def register():
    print ("Registering ", __name__)
    bpy.utils.register_class(core.SGETypes)
    bpy.utils.register_class(core.SinGEDProps)
    bpy.utils.register_class(operators.SinGEDConnect)
    bpy.utils.register_class(ui.SinGEDConnectPanel)
    bpy.types.Scene.singed = PointerProperty(type=core.SinGEDProps)

# Unregister classes
def unregister():
    print ("Unregistering ", __name__)

    # Close the active session
    core.close_active_session()

    del bpy.types.Scene.singed
    bpy.utils.unregister_class(ui.SinGEDConnectPanel)
    bpy.utils.unregister_class(operators.SinGEDConnect)
    bpy.utils.unregister_class(core.SinGEDProps)
    bpy.utils.unregister_class(core.SGETypes)

if __name__ == "__main__":
    register()
