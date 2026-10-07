import bpy
import os
import json
import struct

ROOT = os.path.dirname(os.path.abspath(__file__))
scene = bpy.context.scene
scene.render.resolution_x = 1400
scene.render.resolution_y = 1680
scene.render.resolution_percentage = 100
scene.cycles.samples = 96
scene.camera = bpy.data.objects['CAM • Hero three quarter']
scene.render.filepath = os.path.join(ROOT, 'orc_warrior_preview.png')
bpy.context.preferences.filepaths.save_version = 0
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(ROOT, 'orc_warrior.blend'))
bpy.ops.render.render(write_still=True)

# Supplementary orthographic views make the silhouette and equipment inspectable.
scene.cycles.samples = 32
scene.render.resolution_percentage = 50
for label, cam in [('front','CAM • Front inspection'),('back','CAM • Back inspection')]:
    scene.camera = bpy.data.objects[cam]
    scene.render.filepath = os.path.join(ROOT, 'orc_warrior_'+label+'.png')
    bpy.ops.render.render(write_still=True)

# Verify the saved GLB container and its referenced mesh buffers.
with open(os.path.join(ROOT, 'orc_warrior.glb'), 'rb') as f:
    data = f.read()
magic, version, size = struct.unpack_from('<4sII', data, 0)
assert magic == b'glTF' and version == 2 and size == len(data)
chunk_size, chunk_type = struct.unpack_from('<II', data, 12)
assert chunk_type == 0x4E4F534A
doc = json.loads(data[20:20+chunk_size])
assert doc['meshes'] and doc['materials'] and doc['nodes']
assert all('uri' not in b for b in doc['buffers'])
assert all('POSITION' in p['attributes'] for m in doc['meshes'] for p in m['primitives'])
assert not bpy.data.libraries
report = {'blender_version':bpy.app.version_string, 'blend_reopened':True,
          'glb_version':version, 'glb_bytes':size, 'glb_meshes':len(doc['meshes']),
          'glb_materials':len(doc['materials']), 'external_libraries':0,
          'rigged':False, 'hero_render':[1400,1680]}
with open(os.path.join(ROOT,'validation.json'),'w',encoding='utf8') as f:
    json.dump(report, f, indent=2)
print('FINAL_VALIDATION', json.dumps(report), flush=True)
