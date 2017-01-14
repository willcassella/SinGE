import bpy
import bmesh
import struct
import array
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty
from bpy.types import Operator

def export_sge_mesh(context, path, selected_only):
    data = context.active_object.data

    # Copy and triangulify its mesh data
    mesh = bmesh.new()
    mesh.from_mesh(data)
    bmesh.ops.triangulate(mesh, faces=mesh.faces)
    uvLayer = mesh.loops.layers.uv.active

    # Create float arrays for the vertex positions, normals, and uv layouts
    positions = array.array('f')
    normals = array.array('f')
    uv0 = array.array('f')

    for face in mesh.faces:
        for loop in face.loops:
            # Serialize position (swizzled)
            positions.append(-loop.vert.co[0])
            positions.append(loop.vert.co[2])
            positions.append(loop.vert.co[1])

            # Serialize normal (swizzled)
            normals.append(loop.vert.normal[0])
            normals.append(loop.vert.normal[2])
            normals.append(loop.vert.normal[1])

            # Serialize UV
            if uvLayer is not None:
                uv0.append(loop[uvLayer].uv[0])
                uv0.append(loop[uvLayer].uv[1])

    # Open a file to write to
    with open(path, 'wb') as file:

        # Write out vertex positions
        file.write(struct.pack('I', len(positions) // 3))
        positions.tofile(file)

        # Write out vertex normals
        file.write(struct.pack('I', len(normals) // 3))
        normals.tofile(file)

        # Wrute out UVs
        file.write(struct.pack('I', len(uv0) // 2))
        uv0.tofile(file)

class SGEStaticMeshExporter(Operator, ExportHelper):
    """Exports a mesh to the sge::StaticMesh file format"""
    bl_idname = "sge.export_static_mesh"
    bl_label = "Export SGE Static Mesh"

    # ExportHelper mixin class uses this
    filename_ext = ".smesh"

    filter_glob = StringProperty(
            default="*.smesh",
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
    self.layout.operator(SGEStaticMeshExporter.bl_idname, text="SGE Static Mesh (.smesh)")

def from_json(path, value):
    mesh = bmesh.new()
    verts = value['vertex_positions']
    face = [None, None, None]

    for i in range(0, len(verts), 9):
        for v in range(0, 3):
            face[v] = mesh.verts.new((-verts[v * 3 + i], verts[v * 3 + i + 2], verts[v * 3 + i + 1]))
        mesh.faces.new(face)

    # Convert the bmesh to a blender mesh data object
    result = bpy.data.meshes.new(path)
    mesh.to_mesh(result)
    return result
