# static_mesh.py

import bpy
import bmesh
import struct
import array
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty
from bpy.types import Operator
from . import archive

def export_sge_mesh(context, path, selected_only):
    data = context.active_object.data

    # Copy and triangulify its mesh data
    mesh = bmesh.new()
    mesh.from_mesh(data)
    bmesh.ops.triangulate(mesh, faces=mesh.faces)
    uvLayer = mesh.loops.layers.uv.active

    # Create float arrays for the vertex positions, normals, and uv layouts
    positions = list()
    normals = list()
    tangents = list()
    uv0 = list()

    for face in mesh.faces:
        for loop in face.loops:
            # Serialize position (swizzled)
            positions.append(-loop.vert.co[0])
            positions.append(loop.vert.co[2])
            positions.append(loop.vert.co[1])

            # Serialize normal (swizzled)
            norm = loop.vert.normal
            normals.append(-norm[0])
            normals.append(norm[2])
            normals.append(norm[1])

            # Serialize tangent (swizzled)
            tang = loop.calc_tangent()
            tangents.append(-tang[0])
            tangents.append(tang[2])
            tangents.append(tang[1])

            # Serialize UV
            if uvLayer is not None:
                uv0.append(loop[uvLayer].uv[0])
                uv0.append(loop[uvLayer].uv[1])

    # Create an output archive
    out = archive.Archive()

    # Write vertex positions
    out.push_object_member("vpos")
    out.typed_array_f32(positions)
    out.pop()

    # Write vertex normals
    out.push_object_member("vnor")
    out.typed_array_f32(normals)
    out.pop()

    # Write vertex tangents
    out.push_object_member("vtan")
    out.typed_array_f32(tangents)
    out.pop()

    # Write vertex uv 0
    out.push_object_member("uv0")
    out.typed_array_f32(uv0)
    out.pop()

    # Open a file to write to
    with open(path, 'wb') as file:
        buff = out.to_binary()
        file.write(buff)

class SGEStaticMeshExporter(Operator, ExportHelper):
    """Exports a mesh to the sge::StaticMesh file format"""
    bl_idname = "sge.export_static_mesh"
    bl_label = "Export SGE Static Mesh"

    # ExportHelper mixin class uses this
    filename_ext = ".sbin"

    filter_glob = StringProperty(
            default="*.sbin",
            options={'HIDDEN'},
            maxlen=255,  # Max internal buffer length, longer would be clamped.
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    selected_only = BoolProperty(
            name="Selected Only",
            description="Only export selected objects",
            default=False,
            )

    def execute(self, context):
        export_sge_mesh(context, self.filepath, self.selected_only)
        return {'FINISHED'}

def export_menu_func(self, context):
    self.layout.operator(SGEStaticMeshExporter.bl_idname, text="SGE Static Mesh (.sbin)")

def from_json(path, value):
    mesh = bmesh.new()
    verts = value['vpos']
    uv0 = value['uv0']
    face_verts = [None, None, None]

    # Create the uv layer
    uv_layer0 = mesh.loops.layers.uv.new('uv0')

    # For each face in the mesh
    for i in range(0, len(verts) // 9):
        # For each vertex in the face
        for v in range(0, 3):
            face_verts[v] = mesh.verts.new((-verts[i*9 + v*3 + 0], verts[i*9 + v*3 + 2], verts[i*9 + v*3 + 1]))

        # Create the face
        face = mesh.faces.new(face_verts)

        # Set the uv coordinates
        for v in range(0, 3):
            face.loops[v][uv_layer0].uv[0] = uv0[i*6 + v*2 + 0]
            face.loops[v][uv_layer0].uv[1] = uv0[i*6 + v*2 + 1]

    # Convert the bmesh to a blender mesh data object
    result = bpy.data.meshes.new(path)
    mesh.to_mesh(result)
    mesh.free()
    return result
