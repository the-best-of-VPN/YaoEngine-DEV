"""Read-only source-scene inspection; no blend or export saves."""
import bpy
import json
from pathlib import Path

report = {'blender_version': bpy.app.version_string, 'source': bpy.data.filepath}
report['collections'] = {
    col.name: {'count': len(col.objects), 'objects': [obj.name for obj in col.objects]}
    for col in bpy.data.collections
}
report['mesh_transforms'] = []
for obj in bpy.data.objects:
    if obj.type != 'MESH':
        continue
    mat = obj.matrix_world
    ident = all(abs(mat[i][j] - (1.0 if i == j else 0.0)) < 1e-7 for i in range(4) for j in range(4))
    entry = {'name': obj.name, 'vertices': len(obj.data.vertices), 'identity_matrix_world': ident,
             'matrix_world': [list(row) for row in mat], 'parent': obj.parent.name if obj.parent else None}
    if obj.data.vertices:
        co = obj.data.vertices[0].co
        entry.update(first_vertex_local=list(co), first_vertex_world=list(mat @ co))
    report['mesh_transforms'].append(entry)

# Temporary in-memory object tests normal keyframe action access under Blender 5.1.
probe = bpy.data.objects.new('__rig_api_probe__', None)
bpy.context.scene.collection.objects.link(probe)
probe.location = (0, 0, 0)
probe.keyframe_insert(data_path='location', frame=1)
probe.location = (0, 0, 1)
probe.keyframe_insert(data_path='location', frame=20)
action = probe.animation_data.action
anim = probe.animation_data
action_info = {'has_fcurves_attribute': hasattr(action, 'fcurves'),
               'action_slot': anim.action_slot.identifier if anim.action_slot else None,
               'slots': [slot.identifier for slot in action.slots], 'layers': []}
for layer in action.layers:
    layer_info = {'name': layer.name, 'strips': []}
    for strip in layer.strips:
        strip_info = {'type': strip.type, 'has_channelbag': hasattr(strip, 'channelbag'),
                      'has_channelbags': hasattr(strip, 'channelbags')}
        if hasattr(strip, 'channelbag'):
            bag = strip.channelbag(anim.action_slot)
            strip_info['slot_channelbag_fcurves'] = [
                {'data_path': fc.data_path, 'array_index': fc.array_index, 'keyframes': len(fc.keyframe_points)}
                for fc in bag.fcurves
            ] if bag else None
        layer_info['strips'].append(strip_info)
    action_info['layers'].append(layer_info)
try:
    from bpy_extras.anim_utils import action_get_channelbag_for_slot
    bag = action_get_channelbag_for_slot(action, anim.action_slot)
    action_info['action_get_channelbag_for_slot_works'] = len(bag.fcurves)
except Exception as exc:
    action_info['action_get_channelbag_for_slot_error'] = repr(exc)
report['action_api_probe'] = action_info
bpy.data.objects.remove(probe, do_unlink=True)
bpy.data.actions.remove(action)

report['gltf_parameters'] = {}
try:
    for prop in bpy.ops.export_scene.gltf.get_rna_type().properties:
        if any(term in prop.identifier for term in ('anim', 'skin', 'bone', 'frame', 'sampl', 'nla', 'action', 'apply')):
            value = {'type': prop.type, 'description': prop.description}
            if hasattr(prop, 'default'):
                value['default'] = prop.default
            if prop.type == 'ENUM':
                value['enum_items'] = [item.identifier for item in prop.enum_items]
            report['gltf_parameters'][prop.identifier] = value
except Exception as exc:
    report['gltf_parameters_error'] = repr(exc)

report['ffmpeg'] = {'build_option_ffmpeg': getattr(bpy.app.build_options, 'ffmpeg', None),
                    'has_render_ffmpeg': hasattr(bpy.context.scene.render, 'ffmpeg')}
formats = bpy.context.scene.render.image_settings.bl_rna.properties['file_format']
report['ffmpeg']['file_formats'] = [item.identifier for item in formats.enum_items]
if hasattr(bpy.context.scene.render, 'ffmpeg'):
    settings = bpy.context.scene.render.ffmpeg
    for attr in ('format', 'codec', 'constant_rate_factor', 'ffmpeg_preset', 'audio_codec'):
        if hasattr(settings, attr):
            prop = settings.bl_rna.properties[attr]
            report['ffmpeg'][attr] = {'current': getattr(settings, attr),
                                      'values': [item.identifier for item in prop.enum_items] if prop.type == 'ENUM' else None}

out = Path(__file__).with_name('rig_api_report.json')
out.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding='utf-8')
print('RIG_API_REPORT=' + str(out))
print('ACTION_API=' + json.dumps(action_info))
print('FFMPEG=' + json.dumps(report['ffmpeg']))
print('COLLECTION_COUNTS=' + json.dumps({name: value['count'] for name, value in report['collections'].items()}, ensure_ascii=False))
print('NON_IDENTITY_MESHES=' + json.dumps([entry['name'] for entry in report['mesh_transforms'] if not entry['identity_matrix_world']], ensure_ascii=False))
