# static_mesh.py

import bpy
import bmesh
import mathutils
import math
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty
from bpy.types import Operator
from . import archive

SHORT_MAX = 32767
USHORT_MAX = 0xFFFF

def export_submesh(obj, writer):
    # Make sure this submesh has a UV layout
    if len(obj.data.uv_layers) == 0:
        return (False, "Object '{}' does not have a UV layout".format(obj.name))

    # Get a copy of the object's mesh
    mesh = obj.data.copy()

    # Triangulate mesh data with bmesh
    temp_mesh = bmesh.new()
    temp_mesh.from_mesh(mesh)
    bmesh.ops.triangulate(temp_mesh, faces=temp_mesh.faces)
    temp_mesh.to_mesh(mesh)
    temp_mesh.free()

    # Create arrays for the vertex positions, normals, and uv layouts
    vert_values = dict()
    out_vpos = list()
    out_vnor = list()
    out_vtan = list()
    out_vbts = list()
    out_uv = list()
    out_elems = list()
    elem_index = 0

    # Get object-to-world matrix
    model_mat = mathutils.Matrix(obj.matrix_world)

    # Get the transpose-inverse model-to-world matrix
    model_mat_inv_transp = model_mat.inverted()
    model_mat_inv_transp.transpose()

    # Calculate tangent vectors
    mesh.calc_tangents()

    for mesh_loop, uv_loop in zip(mesh.loops, mesh.uv_layers.active.data):
        # Create the vertex value for this loop
        loop_key = (tuple(mesh_loop.normal), tuple(uv_loop.uv))
        vert_dict = vert_values.setdefault(mesh_loop.vertex_index, dict())

        # If there already exists an identical loop
        if loop_key in vert_dict:
            out_elems.append(vert_dict[loop_key])
            continue
        else:
            vert_dict[loop_key] = elem_index
            out_elems.append(elem_index)
            elem_index += 1

        # Get the vertex
        vert = mesh.vertices[mesh_loop.vertex_index]

        # Get vertex position (swizzled)
        pos = model_mat * mathutils.Vector(vert.co)
        out_vpos.append( -pos[0] )
        out_vpos.append( pos[2] )
        out_vpos.append( pos[1] )

        # Get vertex normal (swizzled)
        norm = model_mat_inv_transp * mathutils.Vector(mesh_loop.normal)
        out_vnor.append( int(-norm[0] * SHORT_MAX) )
        out_vnor.append( int(norm[2] * SHORT_MAX) )
        out_vnor.append( int(norm[1] * SHORT_MAX) )

        # Get vertex tangents (swizzled)
        tang = model_mat_inv_transp * mathutils.Vector(mesh_loop.tangent)
        out_vtan.append( int(-tang[0] * SHORT_MAX) )
        out_vtan.append( int(tang[2] * SHORT_MAX) )
        out_vtan.append( int(tang[1] * SHORT_MAX) )

        # Get bitangent signs
        out_vbts.append( int(mesh_loop.bitangent_sign) )

        # Get UV coordinates
        if uv_loop.uv[0] < -0.0001 or uv_loop.uv[0] > 1.0001 or uv_loop.uv[1] < -0.001 or uv_loop.uv[1] > 1.0001:
            return (False, "UV coordinates must be in the range [0, 1]")

        out_uv.append( int(uv_loop.uv[0] * USHORT_MAX) )
        out_uv.append( int(uv_loop.uv[1] * USHORT_MAX) )

    # Free tangent vectors
    mesh.free_tangents()

    # Serialize as an object
    writer.as_object()

    # Save default material
    writer.push_object_member("mat")
    writer.string(obj.data.sge_default_material)
    writer.pop()

    # Save positions
    writer.push_object_member("vpos")
    writer.typed_array_f32(out_vpos)
    writer.pop()

    # Save normals
    writer.push_object_member("vnor")
    writer.typed_array_i16(out_vnor)
    writer.pop()

    # Save tangents
    writer.push_object_member("vtan")
    writer.typed_array_i16(out_vtan)
    writer.pop()

    # Save bitangent signs
    writer.push_object_member("vbts")
    writer.typed_array_i8(out_vbts)
    writer.pop()

    # Save uv coordinates
    writer.push_object_member("uv")
    writer.typed_array_u16(out_uv)
    writer.pop()

    # Save element indices
    writer.push_object_member("ind")
    writer.typed_array_u32(out_elems)
    writer.pop()

    # Get rid of the datablock
    bpy.data.meshes.remove(mesh)
    return (True, None)

def export_sge_mesh(context, path, selected_only):

    if selected_only:
        obj_list = context.selected_objects
    else:
        obj_list = context.scene.objects

    # Create an output archive
    writer = archive.Archive()
    writer.as_object()

    # Serialize subobjects
    writer.push_object_member("objs")
    writer.as_object()
    for obj in obj_list:
        if obj.type != 'MESH':
            continue

        # Serialize this object as a subobject
        writer.push_object_member(obj.name)
        success, message = export_submesh(obj, writer)

        # Make sure serialization completed succesfully
        if not success:
            return (False, message)

        writer.pop() # obj.name

    writer.pop() # "objs"

    # Open a file to write to
    with open(path, 'wb') as file:
        buff = writer.to_binary()
        file.write(buff)

    return (True, None)

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
            default=True,
            )

    def execute(self, context):
        success, message = export_sge_mesh(context, self.filepath, self.selected_only)

        # Make sure export succeeded
        if not success:
            self.report(type={'ERROR'}, message=message)
            return {'CANCELLED'}

        return {'FINISHED'}

def export_menu_func(self, context):
    self.layout.operator(SGEStaticMeshExporter.bl_idname, text="SGE Static Mesh (.sbin)")

def from_json(path, value):
    mesh = bmesh.new()
    for name, obj in value['objs'].items():
        verts = obj['vpos']
        uv = obj['uv']
        elems = obj['ind']
        face_verts = [None, None, None]

        # Create the uv layer
        uv_layer0 = mesh.loops.layers.uv.new('material_uv')

        # For each face in the mesh
        for i in range(0, len(elems) // 3):
            # For each vertex in the face
            for v in range(0, 3):
                vert_index = elems[i*3 + v] * 3

                # Create the vertex
                vert = mesh.verts.new((
                    -verts[vert_index + 0],
                    verts[vert_index + 2],
                    verts[vert_index + 1]))

                # Assign the vertex to the face
                face_verts[v] = vert

            # Create the face
            face = mesh.faces.new(face_verts)

    # Convert the bmesh to a blender mesh data object
    result = bpy.data.meshes.new(path)
    mesh.to_mesh(result)
    mesh.free()
    return result
