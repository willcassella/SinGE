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


def sge_editor_send(eventDict):
    # Convert the json to a byte string
    out_str = json.dumps(eventDict).encode()

    # Construct a packet
    out_packet = struct.pack("I", len(out_str)) + out_str

    # Send it
    sock.send(out_packet)

def transform_to_dict(dict, entity):
    dict["local_position"] = [
        entity.location[1],
        entity.location[2],
        entity.location[0]
    ]

def send_transform_event(scene):
    # Create a dictionary for all selected objects
    transformComponents = {}
    sceneDict = {
        "component_mod": {
            "sge::CTransform3D": transformComponents
        }
    }

    for obj in bpy.context.selected_objects:
        entityId = obj["sge_entity_id"]

        if entityId is None:
            continue

        transformDict = transformComponents[entityId] = {}
        transform_to_dict(transformDict, obj)

    sge_editor_send(sceneDict)

# Get scene data
engine_scene = sge_editor_receive()

# Instantiate all entities
entities = {}
for id,v in engine_scene["entities"].items():
    # Create an entity object (empty)
    entity = bpy.data.objects.new(v["name"], None)
    entity["sge_entity_id"] = id
    bpy.context.scene.objects.link(entity)
    entities[id] = entity

# Instantiate all transforms
for id,transform in engine_scene["components"]["sge::CTransform3D"].items():
    entity = entities[id]

    # Assign the local position (swizzle comopnents)
    local_position = transform["local_position"]
    entity.location[1] = local_position[0]
    entity.location[2] = local_position[1]
    entity.location[0] = local_position[2]

    # Assign local scale (swizzle components)
    local_scale = transform["local_scale"]
    entity.scale[1] = local_scale[0]
    entity.scale[2] = local_scale[1]
    entity.scale[0] = local_scale[2]

    # Assign local rotation (swizzle components)
    local_rotation = transform["local_rotation"]
    entity.rotation_quaternion[2] = local_rotation[0]
    entity.rotation_quaternion[3] = local_rotation[1]
    entity.rotation_quaternion[1] = local_rotation[2]
    entity.rotation_quaternion[0] = -local_rotation[3]

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

# Register the update handler
bpy.app.handlers.scene_update_post.append(send_transform_event)
