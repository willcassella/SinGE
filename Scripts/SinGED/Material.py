import bpy
import json

# Get the active material on the active object
mat = bpy.context.object.active_material

# Create a dictionary to represent it
result = {
    "vertex_shader": "engine_content/shaders/basic.vert",
    "fragment_shader": "engine_content/shaders/basic.frag"
}

# Create material parameters
params = {} 
result["params"] = params

# Save texture parameters
for tex in mat.texture_slots:
    # If the texture slot doesn't have anything
    if tex is None:
        continue
    
    # If the texture is not an image
    if tex.texture.type != 'IMAGE':
        continue
    
    # Set the path
    params[tex.name] = tex.texture.image.filepath
    
# Save the file
with open("C:/Users/Will/Downloads/test.json", 'w') as file:
    file.write(json.dumps(result))
