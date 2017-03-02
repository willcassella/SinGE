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

class MeshData(object):
    def __init__(self):
        self.next_elem_index = 0
        self.mat_elems = dict()
        self.vpos = list()
        self.vnor = list()
        self.vtan = list()
        self.vbts = list()
        self.mat_uv = list()
        self.lm_uv = list()

class MeshMaterial(object):
    def __init__(self):
        self.path = ""
        self.start_elem_index = 0
        self.num_elems = 0

def export_submesh(obj, mesh_data, mat_uv_name, lm_uv_name):
    # Make sure this mesh has the required UV layers
    if mat_uv_name not in obj.data.uv_layers:
        return (False, "Object '{}' does not have UV layer '{}'.".format(obj.name, mat_uv_name))
    if lm_uv_name is not None and lm_uv_name not in obj.data.uv_layers:
        return (False, "Object '{}' does not have UV layer '{}'.".format(obj.name, lm_uv_name))

    # Get a copy of the object's mesh (apply modifiers)
    mesh = obj.to_mesh(scene=bpy.context.scene, apply_modifiers=True, settings='PREVIEW')

    # Triangulate mesh data with bmesh
    temp_mesh = bmesh.new()
    temp_mesh.from_mesh(mesh)
    bmesh.ops.triangulate(temp_mesh, faces=temp_mesh.faces)
    temp_mesh.to_mesh(mesh)
    temp_mesh.free()

    # Get object-to-world matrix
    model_mat = mathutils.Matrix(obj.matrix_world)

    # Get the transpose-inverse model-to-world matrix
    model_mat_inv_transp = model_mat.inverted()
    model_mat_inv_transp.transpose()

    # Create arrays for vertex indices
    vert_values = dict()

    # Calculate tangent vectors
    mesh.calc_tangents(uvmap=mat_uv_name)

    # Get the material and lightmap uv
    mat_uv = mesh.uv_layers[mat_uv_name].data
    if lm_uv_name is not None:
        lm_uv = mesh.uv_layers[lm_uv_name].data
    else:
        lm_uv = mat_uv

    # Get the materials for this mesh
    if len(mesh.materials) != 0:
        materials = mesh.materials
    else:
        materials = [None]

    # For each polygon in the mesh
    for poly in mesh.polygons:
        poly_elems = list()

        # For each loop in this polygon
        for loop_i in range(poly.loop_start, poly.loop_start + 3):
            # Get the mesh loop, mat_uv_loop, and lm_uv_loop
            mesh_loop = mesh.loops[loop_i]
            mat_uv_loop = mat_uv[loop_i]
            lm_uv_loop = lm_uv[loop_i]

            # Create the loop key for this loop (uniquely identifies this loop's values)
            loop_key = (mesh_loop.vertex_index, tuple(mesh_loop.normal), tuple(mat_uv_loop.uv), tuple(lm_uv_loop.uv))

            # If there already exists an identical loop for this vertex
            if loop_key in vert_values:
                # Get the element index for this loop, and go to the next loop
                poly_elems.append(vert_values[loop_key])
                continue
            else:
                vert_values[loop_key] = mesh_data.next_elem_index
                poly_elems.append(mesh_data.next_elem_index)
                mesh_data.next_elem_index += 1

            # Get the vertex
            vert = mesh.vertices[mesh_loop.vertex_index]

            # Get vertex position (swizzled)
            pos = model_mat * mathutils.Vector(vert.co)
            mesh_data.vpos.append( -pos[0] )
            mesh_data.vpos.append( pos[2] )
            mesh_data.vpos.append( pos[1] )

            # Get vertex normal (swizzled)
            norm = model_mat_inv_transp * mathutils.Vector(mesh_loop.normal)
            mesh_data.vnor.append( int(-norm[0] * SHORT_MAX) )
            mesh_data.vnor.append( int(norm[2] * SHORT_MAX) )
            mesh_data.vnor.append( int(norm[1] * SHORT_MAX) )

            # Get vertex tangents (swizzled)
            tang = model_mat_inv_transp * mathutils.Vector(mesh_loop.tangent)
            mesh_data.vtan.append( int(-tang[0] * SHORT_MAX) )
            mesh_data.vtan.append( int(tang[2] * SHORT_MAX) )
            mesh_data.vtan.append( int(tang[1] * SHORT_MAX) )

            # Get bitangent signs
            mesh_data.vbts.append( int(mesh_loop.bitangent_sign) )

            # Check material UV coordinates
            if mat_uv_loop.uv[0] < -0.001 or mat_uv_loop.uv[0] > 1.001 or mat_uv_loop.uv[1] < -0.001 or mat_uv_loop.uv[1] > 1.001:
                mesh.free_tangents()
                bpy.data.meshes.remove(mesh)
                return (False, "UV coordinates must be in the range [0, 1]")

            # Get material UV
            mesh_data.mat_uv.append( int(mat_uv_loop.uv[0] * USHORT_MAX) )
            mesh_data.mat_uv.append( int(mat_uv_loop.uv[1] * USHORT_MAX) )

            if lm_uv_name is None:
                continue

            # Check lightmap UV coordinates
            if lm_uv_loop.uv[0] < -0.0001 or lm_uv_loop.uv[0] > 1.0001 or lm_uv_loop.uv[1] < -0.001 or lm_uv_loop.uv[1] > 1.0001:
                mesh.free_tangents()
                bpy.data.meshes.remove(mesh)
                return (False, "UV coordinates must be in the range [0, 1]")

            # Get lightmap UV
            mesh_data.lm_uv.append( int(lm_uv_loop.uv[0] * USHORT_MAX) )
            mesh_data.lm_uv.append( int(lm_uv_loop.uv[1] * USHORT_MAX) )

        # Get the material for this polygon
        mat = materials[poly.material_index]
        mesh_data.mat_elems.setdefault(mat, list()).extend(poly_elems)

    # Free data
    mesh.free_tangents()
    bpy.data.meshes.remove(mesh)
    return (True, None)

def export_sge_mesh(context, path, selected_only, mat_uv_name, lm_uv_name=None):
    if selected_only:
        obj_list = context.selected_objects
    else:
        obj_list = context.scene.objects

    # Create a mesh data object
    mesh_data = MeshData()

    for obj in obj_list:
        if obj.type != 'MESH':
            continue

        # Attempt to export the mesh
        success, message = export_submesh(obj, mesh_data, mat_uv_name, lm_uv_name)

        # Make sure serialization completed succesfully
        if not success:
            return (False, message)

    # Create an index and material list
    total_elems = list()
    materials = list()

    for mat, elems in mesh_data.mat_elems.items():
        mesh_mat = MeshMaterial()

        # If this object actually had a material
        if mat is not None:
            mesh_mat.path = mat.sge_path

        mesh_mat.start_elem_index = len(total_elems)
        mesh_mat.num_elems = len(elems)

        # Add the elements to the total list
        total_elems.extend(elems)

        # Add the material
        materials.append(mesh_mat)

    # Create an output archive
    writer = archive.Archive()
    writer.as_object()

    # Save positions
    writer.push_object_member("vpos")
    writer.typed_array_f32(mesh_data.vpos)
    writer.pop() # "vpos"

    # Save normals
    writer.push_object_member("vnor")
    writer.typed_array_i16(mesh_data.vnor)
    writer.pop() # "vnor"

    # Save tangents
    writer.push_object_member("vtan")
    writer.typed_array_i16(mesh_data.vtan)
    writer.pop() # "vtan"

    # Save bitangent signs
    writer.push_object_member("vbts")
    writer.typed_array_i8(mesh_data.vbts)
    writer.pop() # "vbts"

    # Save material uv coordinates
    writer.push_object_member("mtuv")
    writer.typed_array_u16(mesh_data.mat_uv)
    writer.pop() # "mtuv"

    # Save lightmap uv coordinates
    if lm_uv_name is not None:
        writer.push_object_member("lmuv")
        writer.typed_array_u16(mesh_data.lm_uv)
        writer.pop() # "lmuv"

    # Save element indices
    writer.push_object_member("elem")
    writer.typed_array_u32(total_elems)
    writer.pop() # "elem"

    # Save materials
    writer.push_object_member("mats")
    writer.as_generic_array()

    for mat in materials:
        # Save materials
        writer.push_generic_array_element()
        writer.as_object()

        # Save material path
        writer.push_object_member("path")
        writer.string(mat.path)
        writer.pop() # "path"

        # Save material start element index
        writer.push_object_member("estrt")
        writer.u32(mat.start_elem_index)
        writer.pop() # "estrt"

        # Save material element index count
        writer.push_object_member("ecnt")
        writer.u32(mat.num_elems)
        writer.pop() # "ecnt"

        writer.pop() # mat array element

    writer.pop() # "mats"

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
            maxlen=255)

    # Option to export only selected objects
    selected_only = BoolProperty(
            name="Selected Only",
            description="Only export selected objects",
            default=True)

    material_uv = StringProperty(
        name="Material UV",
        description="UV layout used for materials",
        default="UVMap"
    )

    lightmap_uv = StringProperty(
        name="Lightmap UV",
        description="Which UV layout to use for lightmapping",
        default="")

    def execute(self, context):

        if len(self.lightmap_uv) == 0:
            success, message = export_sge_mesh(context, self.filepath, self.selected_only, self.material_uv)
        else:
            success, message = export_sge_mesh(context, self.filepath, self.selected_only, self.material_uv, self.lightmap_uv)

        # Make sure export succeeded
        if not success:
            self.report(type={'ERROR'}, message=message)
            return {'CANCELLED'}

        return {'FINISHED'}

def export_menu_func(self, context):
    self.layout.operator(SGEStaticMeshExporter.bl_idname, text="SGE Static Mesh (.sbin)")

def from_json(path, value):
    mesh = bmesh.new()
    verts = value['vpos']
    uv = value['mtuv']
    elems = value['elem']
    face_verts = [None, None, None]

    # Create the uv layer
    uv_layer0 = mesh.loops.layers.uv.new('UVMap')

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
