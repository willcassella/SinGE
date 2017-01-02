# StaticMesh.py

import bpy
import bmesh
import struct
import array

def export_mesh(data, path):

    # Copy and triangulify it's mesh data
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
            positions.append(loop.vert.co[1])
            positions.append(loop.vert.co[2])
            positions.append(loop.vert.co[0])

            # Serialize normal (swizzled)
            normals.append(loop.vert.normal[1])
            normals.append(loop.vert.normal[2])
            normals.append(loop.vert.normal[0])

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

mesh = bpy.context.object.data
export_mesh(mesh, "C:/Users/Will/Downloads/monkey.wmesh")