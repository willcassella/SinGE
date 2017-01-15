# material.py

import bpy

def texture_from_json(path, value):
    # Create a new blender image
    img = bpy.data.images.new(path, width=value['image']['width'], height=value['image']['height'])

    # Fill the image
    bitmap = value['image']['bitmap']
    img.pixels = [float(pix) / 255 for pix in bitmap]
    
    # Create a new blender texture
    tex = bpy.data.textures.new(path, type='IMAGE')
    tex.image = img
    return tex

def material_from_json(sge_res, path, value):
    # Create a new blender material
    mat = bpy.data.materials.new(path)
    mat.use_shadeless = True

    def set_texture(res, path, tex):
        nonlocal mat
        slot = mat.texture_slots.add()
        slot.texture = tex

    # See if the material has a 'diffuse' value
    if 'params' in value:
        params = value['params']
        if 'diffuse' in params:
            diffuse = params['diffuse']
            if isinstance(diffuse, str):
                # Ask the resource manager for the texture
                sge_res.get_resource_async(diffuse, 'sge::Texture', set_texture)

    return mat
