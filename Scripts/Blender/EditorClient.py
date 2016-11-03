# EditorClient.py
import bpy
import json
import socket
import struct

# Connect to the editor server
sock = socket.socket()
sock.connect(("localhost", 1995))

# Load scene data
def sge_editor_receive():
    # Get the length of the incoming string
    (in_len,) = struct.unpack("I", sock.recv(4))
    
    # Get the incoming string
    in_str = sock.recv(in_len)
    
    # Convert it to json
    return json.loads(in_str.decode("utf-8"))


def sge_editor_send(mod_js):
    # Convert the json to a byte string
    out_str = json.dumps(mod_js).encode()
    
    # Construct a packet
    out_packet = struct.pack("I", len(out_str)) + out_str
    
    # Send it
    sock.send(out_packet)

# Get scene data
engine_scene = sge_editor_receive()

# Instantiate all entities
entities = {}
for id,v in engine_scene["entities"].items():
    entity = bpy.data.objects.new(v["name"], None)
    bpy.context.scene.objects.link(entity)
    entities[id] = entity

# Instantiate all transforms
for id,transform in engine_scene["components"]["sge::CTransform3D"].items():
    entity = entities[id]
    local_position = transform["local_position"]
    entity.location[0] = local_position[0]
    entity.location[1] = local_position[1]
    entity.location[2] = local_position[2]
    
# Instantiate all cameras
for id,camera in engine_scene["components"]["sge::CPerspectiveCamera"].items():
    entity = entities[id]
    
    # Create the component data and an object to own it
    cam_data = bpy.data.cameras.new("")
    owner = bpy.data.objects.new("", cam_data)
    
    # Add it to the scene and parent it to the entity
    bpy.context.scene.objects.link(owner)
    owner.parent = entity
    
# Instantiate all meshes
for id,mesh in engine_scene["components"]["sge::CStaticMesh"].items():
    entity = entities[id]
    
    # Create the component data and an object to own it
    mesh_data = bpy.data.meshes.new("")
    owner = bpy.data.objects.new("", mesh_data)
    
    # Add it to the scene and parent it to the entity
    bpy.context.scene.objects.link(owner)
    owner.parent = entity
    