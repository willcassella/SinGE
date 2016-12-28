# ui.py

import bpy
from bpy.types import Panel
from . import operators, core

# Classes
class SinGEDConnectPanel(Panel):
    bl_idname = 'singed.connect_panel'
    bl_label = 'SinGED Server Connection'
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'scene'

    def draw(self, context):
        layout = self.layout
        if core.active_session is None:
            layout.prop(context.scene.singed, 'host')
            layout.prop(context.scene.singed, 'port')
            connector = layout.operator(operators.SinGEDConnect.bl_idname, text='Connect to server')
            connector.establish_connection = True
        else:
            disconnector = layout.operator(operators.SinGEDConnect.bl_idname, text='Disconnect from server')
            disconnector.establish_connection = False
