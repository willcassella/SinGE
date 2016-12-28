# operators.py

import bpy
from bpy.props import BoolProperty
from bpy.types import Operator
from . import core

class SinGEDConnect(Operator):
    bl_idname = 'singed.connect'
    bl_label = 'SinGED Connect'

    establish_connection = BoolProperty(default=True)

    def execute(self, context):
        if self.establish_connection:
            core.open_active_session(context.scene.singed.host, context.scene.singed.port)
        else:
            core.close_active_session()
        return {'FINISHED'}

