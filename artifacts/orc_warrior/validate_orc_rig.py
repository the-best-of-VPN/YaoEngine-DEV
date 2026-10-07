"""Read-only rig, 72-frame motion, and GLB validation. Does not save .blend."""
import bpy
import json
import math
import struct
import traceback
from pathlib import Path
from mathutils import Vector

ROOT = Path(__file__).resolve().parent
REPORT_PATH = ROOT / 'rig_validation.json'
RIG_NAME = 'ORC_RIG • IK character controls'
report = {'source': bpy.data.filepath, 'blender_version': bpy.app.version_string,
          'errors': [], 'warnings': [], 'thresholds': {
              'weight_sum_tolerance': 0.001, 'foot_translation_tolerance': 0.002,
              'foot_rotation_tolerance_radians': 0.002, 'ik_endpoint_tolerance': 0.02,
              'equipment_floor_tolerance': 0.05}}

def finite(values):
    return all(math.isfinite(float(value)) for value in values)

def rotation_delta(a, b):
    return 2 * math.acos(min(1.0, max(0.0, abs(a.dot(b)))))

def world_bounds(obj, depsgraph):
    evaluated = obj.evaluated_get(depsgraph)
    corners = [evaluated.matrix_world @ Vector(co) for co in evaluated.bound_box]
    valid = all(finite(point) for point in corners)
    return {
        'finite': valid,
        'min': [min(p[axis] for p in corners) for axis in range(3)] if valid else None,
        'max': [max(p[axis] for p in corners) for axis in range(3)] if valid else None,
    }

def read_glb(path):
    result = {'path': str(path), 'exists': path.is_file()}
    if not path.is_file():
        report['errors'].append('Animated GLB is missing: ' + str(path))
        return result
    blob = path.read_bytes()
    magic, version, total_length = struct.unpack_from('<4sII', blob)
    if magic != b'glTF' or version != 2 or total_length != len(blob):
        raise ValueError('Invalid GLB header or length')
    document = None
    binary = b''
    cursor = 12
    while cursor < len(blob):
        length, kind = struct.unpack_from('<II', blob, cursor)
        payload = blob[cursor + 8:cursor + 8 + length]
        if kind == 0x4E4F534A:
            document = json.loads(payload.decode('utf-8').rstrip(' \t\r\n\x00'))
        elif kind == 0x004E4942:
            binary = payload
        cursor += 8 + length
    if document is None:
        raise ValueError('GLB JSON chunk is missing')
    accessors = document.get('accessors', [])
    views = document.get('bufferViews', [])
    component_formats = {5120: ('b', 1), 5121: ('B', 1), 5122: ('h', 2),
                         5123: ('H', 2), 5125: ('I', 4), 5126: ('f', 4)}

    def read_scalar_accessor(index):
        accessor = accessors[index]
        if accessor['type'] != 'SCALAR' or 'sparse' in accessor:
            raise ValueError('Expected non-sparse SCALAR time accessor')
        view = views[accessor['bufferView']]
        if view.get('buffer', 0) != 0:
            raise ValueError('Animation input references external buffer')
        fmt, size = component_formats[accessor['componentType']]
        offset = view.get('byteOffset', 0) + accessor.get('byteOffset', 0)
        stride = view.get('byteStride', size)
        return [struct.unpack_from('<' + fmt, binary, offset + step * stride)[0]
                for step in range(accessor['count'])]

    skins = document.get('skins', [])
    animations = document.get('animations', [])
    primitives = [primitive for mesh in document.get('meshes', [])
                  for primitive in mesh.get('primitives', [])]
    missing_skin_attributes = [index for index, primitive in enumerate(primitives)
                               if not {'JOINTS_0', 'WEIGHTS_0'} <= set(primitive.get('attributes', {}))]
    animated = []
    for animation in animations:
        inputs = sorted(set(sampler['input'] for sampler in animation.get('samplers', [])))
        time_sets = [read_scalar_accessor(index) for index in inputs]
        times = [time for values in time_sets for time in values]
        entry = {'name': animation.get('name'), 'channels': len(animation.get('channels', [])),
                 'samplers': len(animation.get('samplers', [])),
                 'time_accessor_count': len(inputs),
                 'all_times_finite': finite(times),
                 'all_time_accessors_strictly_increasing': all(
                     all(b > a for a, b in zip(values, values[1:])) for values in time_sets),
                 'time_min': min(times) if times else None,
                 'time_max': max(times) if times else None,
                 'time_sample_counts': sorted(set(len(values) for values in time_sets))}
        entry['duration_seconds'] = entry['time_max'] - entry['time_min'] if times else None
        animated.append(entry)
    result.update(bytes=len(blob), meshes=len(document.get('meshes', [])),
                  skins=len(skins), skin_joint_counts=[len(skin.get('joints', [])) for skin in skins],
                  animations=animated, primitives=len(primitives),
                  primitives_missing_skin_attributes=missing_skin_attributes,
                  nodes_with_skin=sum('skin' in node for node in document.get('nodes', [])))
    if not skins:
        report['errors'].append('GLB has no skins')
    if not animations:
        report['errors'].append('GLB has no animations')
    if missing_skin_attributes:
        report['errors'].append('GLB has primitives missing JOINTS_0 or WEIGHTS_0')
    expected_duration = 71 / 24
    for entry in animated:
        duration = entry['duration_seconds']
        if duration is None or abs(duration - expected_duration) > 0.05:
            report['errors'].append('GLB animation duration is not approximately 2.9583 seconds: ' + str(entry))
        if not entry['all_times_finite'] or not entry['all_time_accessors_strictly_increasing']:
            report['errors'].append('GLB animation input times are invalid')
    return result

def validate():
    scene = bpy.context.scene
    rig = bpy.data.objects.get(RIG_NAME)
    if rig is None or rig.type != 'ARMATURE':
        report['errors'].append('Expected armature is missing: ' + RIG_NAME)
        return
    collections = [collection for collection in bpy.data.collections
                   if collection.name.startswith(('01 ', '02 ', '03 ', '04 '))]
    character = list({obj.name: obj for collection in collections for obj in collection.objects}.values())
    report['collections'] = {collection.name: len(collection.objects) for collection in collections}
    report['character_object_count'] = len(character)
    report['mesh_count'] = sum(obj.type == 'MESH' for obj in character)
    report['unconverted_objects'] = [{'name': obj.name, 'type': obj.type}
                                   for obj in character if obj.type != 'MESH']
    if len(character) != 266:
        report['errors'].append('Expected 266 character objects, found ' + str(len(character)))
    if report['unconverted_objects']:
        report['errors'].append('Some character objects were not converted to meshes')

    deform_bones = {bone.name for bone in rig.data.bones if bone.use_deform}
    required = ['root', 'pelvis', 'spine', 'chest', 'neck', 'head', 'weapon.axe', 'weapon.shield']
    for side in ('R', 'L'):
        required += [prefix + side for prefix in ('upper_arm.', 'forearm.', 'hand.', 'thigh.', 'shin.', 'foot.',
                                                  'CTRL_hand.', 'CTRL_foot.', 'CTRL_elbow.', 'CTRL_knee.')]
    report['bone_count'] = len(rig.data.bones)
    report['deform_bone_count'] = len(deform_bones)
    report['missing_bones'] = [name for name in required if name not in rig.data.bones]
    if report['missing_bones']:
        report['errors'].append('Required bones missing: ' + ', '.join(report['missing_bones']))
    report['ik_constraints'] = []
    for side in ('R', 'L'):
        for lower, target, pole in (('forearm.', 'hand.', 'elbow.'), ('shin.', 'foot.', 'knee.')):
            bone_name = lower + side
            pb = rig.pose.bones.get(bone_name)
            matches = [constraint for constraint in pb.constraints if constraint.type == 'IK'] if pb else []
            valid = any(constraint.target == rig and constraint.subtarget == 'CTRL_' + target + side
                        and constraint.pole_target == rig and constraint.pole_subtarget == 'CTRL_' + pole + side
                        and constraint.chain_count == 2 and not constraint.mute and constraint.influence > 0.99
                        for constraint in matches)
            report['ik_constraints'].append({'bone': bone_name, 'valid': valid})
            if not valid:
                report['errors'].append('Missing or invalid two-bone IK: ' + bone_name)

    skinning = []
    expected_weapon_counts = {'weapon.axe': 0, 'weapon.shield': 0}
    for obj in character:
        if obj.type != 'MESH':
            continue
        mods = [mod for mod in obj.modifiers if mod.type == 'ARMATURE' and mod.object == rig
                and mod.show_viewport and mod.show_render and mod.use_vertex_groups]
        names = {group.index: group.name for group in obj.vertex_groups}
        missing_weights = 0
        invalid_weights = 0
        sum_errors = 0
        max_sum_error = 0.0
        max_influences = 0
        expected_weapon = ('weapon.axe' if obj.name.startswith('Axe |') else
                           'weapon.shield' if obj.name.startswith('Shield |') else None)
        wrong_weapon_weights = 0
        for vertex in obj.data.vertices:
            weights = [(names[group.group], group.weight) for group in vertex.groups
                       if names.get(group.group) in deform_bones]
            if any(not math.isfinite(weight) or weight < 0 for _, weight in weights):
                invalid_weights += 1
            weights = [(name, weight) for name, weight in weights if weight > 1e-8 and math.isfinite(weight)]
            total = sum(weight for _, weight in weights)
            max_influences = max(max_influences, len(weights))
            if total <= 1e-8:
                missing_weights += 1
            error = abs(total - 1)
            max_sum_error = max(max_sum_error, error)
            sum_errors += error > 0.001
            if expected_weapon and (len(weights) != 1 or weights[0][0] != expected_weapon or abs(weights[0][1] - 1) > 0.001):
                wrong_weapon_weights += 1
        entry = {'name': obj.name, 'vertices': len(obj.data.vertices), 'armature_modifiers': len(mods),
                 'unweighted_vertices': missing_weights, 'invalid_weight_vertices': invalid_weights,
                 'non_normalized_vertices': sum_errors, 'max_weight_sum_error': max_sum_error,
                 'max_influences': max_influences, 'expected_weapon_bone': expected_weapon,
                 'incorrect_weapon_vertices': wrong_weapon_weights}
        skinning.append(entry)
        if expected_weapon:
            expected_weapon_counts[expected_weapon] += 1
        if not mods or not len(obj.data.vertices) or missing_weights or invalid_weights or sum_errors or wrong_weapon_weights:
            report['errors'].append('Skinning failed: ' + obj.name)
    report['skinning'] = skinning
    report['weapon_object_counts'] = expected_weapon_counts
    for name, parent in (('weapon.axe', 'hand.R'), ('weapon.shield', 'hand.L')):
        bone = rig.data.bones.get(name)
        if not bone or not bone.parent or bone.parent.name != parent:
            report['errors'].append('Weapon bone has incorrect parent: ' + name)

    action = rig.animation_data.action if rig.animation_data else None
    report['action'] = None
    if action is None:
        report['errors'].append('Rig has no active Action')
    else:
        from bpy_extras.anim_utils import action_get_channelbag_for_slot
        bag = action_get_channelbag_for_slot(action, rig.animation_data.action_slot)
        frames = [key.co.x for curve in bag.fcurves for key in curve.keyframe_points] if bag else []
        report['action'] = {'name': action.name, 'frame_range': list(action.frame_range),
                            'fcurves': len(bag.fcurves) if bag else 0,
                            'min_keyframe': min(frames) if frames else None,
                            'max_keyframe': max(frames) if frames else None,
                            'scene_frame_range': [scene.frame_start, scene.frame_end],
                            'fps': scene.render.fps / scene.render.fps_base}
        if not frames or abs(min(frames) - 1) > 0.001 or abs(max(frames) - 72) > 0.001:
            report['errors'].append('Action does not cover frames 1 through 72')

    if report['missing_bones']:
        report['glb'] = read_glb(ROOT / 'orc_warrior_attack.glb')
        return
    scene.frame_set(1)
    bpy.context.view_layer.update()
    graph = bpy.context.evaluated_depsgraph_get()
    floor_object = bpy.data.objects.get('Basalt display plinth')
    floor_z = world_bounds(floor_object, graph)['max'][2] if floor_object else 0.23
    report['equipment_floor_z'] = floor_z
    body = bpy.data.objects.get('ORC • unified muscular body')
    weapons = [obj for obj in character if obj.name.startswith(('Axe |', 'Shield |'))]
    references = {}
    for side in ('R', 'L'):
        matrix = rig.matrix_world @ rig.pose.bones['foot.' + side].matrix
        references[side] = (matrix.translation.copy(), matrix.to_quaternion())
    report['frame_samples'] = []
    summary = {'max_foot_translation_drift': 0.0, 'max_foot_rotation_drift': 0.0,
               'max_hand_target_error': 0.0, 'max_foot_target_error': 0.0,
               'min_equipment_z': float('inf'), 'body_nonfinite_vertices': 0,
               'unreachable_arm_frames': [], 'unreachable_leg_frames': [],
               'sudden_limb_rotation_frames': []}
    previous_rotations = {}
    for frame in range(1, 73):
        scene.frame_set(frame)
        bpy.context.view_layer.update()
        graph = bpy.context.evaluated_depsgraph_get()
        eval_rig = rig.evaluated_get(graph)
        sample = {'frame': frame, 'feet': {}, 'hands': {}, 'reachability': {}, 'equipment': {}}
        for side in ('R', 'L'):
            foot = eval_rig.matrix_world @ eval_rig.pose.bones['foot.' + side].matrix
            hand = eval_rig.matrix_world @ eval_rig.pose.bones['hand.' + side].matrix
            foot_target = eval_rig.matrix_world @ eval_rig.pose.bones['CTRL_foot.' + side].matrix
            hand_target = eval_rig.matrix_world @ eval_rig.pose.bones['CTRL_hand.' + side].matrix
            translation_drift = (foot.translation - references[side][0]).length
            rotation_drift = rotation_delta(foot.to_quaternion(), references[side][1])
            foot_error = (foot.translation - foot_target.translation).length
            hand_error = (hand.translation - hand_target.translation).length
            sample['feet'][side] = {'head_world': list(foot.translation), 'quaternion_world': list(foot.to_quaternion()),
                                    'translation_drift': translation_drift, 'rotation_drift': rotation_drift,
                                    'ik_endpoint_error': foot_error}
            sample['hands'][side] = {'head_world': list(hand.translation), 'target_world': list(hand_target.translation),
                                     'ik_endpoint_error': hand_error,
                                     'rotation_error': rotation_delta(hand.to_quaternion(), hand_target.to_quaternion())}
            summary['max_foot_translation_drift'] = max(summary['max_foot_translation_drift'], translation_drift)
            summary['max_foot_rotation_drift'] = max(summary['max_foot_rotation_drift'], rotation_drift)
            summary['max_foot_target_error'] = max(summary['max_foot_target_error'], foot_error)
            summary['max_hand_target_error'] = max(summary['max_hand_target_error'], hand_error)
            for kind, upper, lower, control in (('arm', 'upper_arm.', 'forearm.', 'CTRL_hand.'),
                                                ('leg', 'thigh.', 'shin.', 'CTRL_foot.')):
                start = eval_rig.matrix_world @ eval_rig.pose.bones[upper + side].head
                target = eval_rig.matrix_world @ eval_rig.pose.bones[control + side].head
                rest_lengths = [rig.data.bones[name + side].length for name in (upper, lower)]
                # This asset's rig has unit world scale; include rig scale for validation clarity.
                scale = max(abs(value) for value in eval_rig.matrix_world.to_scale())
                maximum = sum(rest_lengths) * scale
                minimum = abs(rest_lengths[0] - rest_lengths[1]) * scale
                distance = (target - start).length
                reach = {'distance': distance, 'max_reach': maximum, 'min_reach': minimum,
                         'excess': max(0.0, distance - maximum, minimum - distance)}
                sample['reachability'][kind + '.' + side] = reach
                if reach['excess'] > 0.02:
                    summary['unreachable_' + kind + '_frames'].append({'frame': frame, 'side': side, **reach})
                for name in (upper + side, lower + side):
                    quaternion = eval_rig.pose.bones[name].matrix.to_quaternion()
                    if name in previous_rotations:
                        delta = rotation_delta(quaternion, previous_rotations[name])
                        if delta > math.radians(50):
                            summary['sudden_limb_rotation_frames'].append({'frame': frame, 'bone': name, 'degrees': math.degrees(delta)})
                    previous_rotations[name] = quaternion.copy()
        for label, prefix in (('axe', 'Axe |'), ('shield', 'Shield |')):
            boxes = [world_bounds(obj, graph) for obj in weapons if obj.name.startswith(prefix)]
            valid = bool(boxes) and all(box['finite'] for box in boxes)
            aggregate = {'finite': valid}
            if valid:
                aggregate['min'] = [min(box['min'][axis] for box in boxes) for axis in range(3)]
                aggregate['max'] = [max(box['max'][axis] for box in boxes) for axis in range(3)]
                summary['min_equipment_z'] = min(summary['min_equipment_z'], aggregate['min'][2])
                if aggregate['min'][2] < floor_z - 0.05:
                    report['errors'].append('Equipment crosses floor tolerance at frame %d: %s min_z=%.6f' % (frame, label, aggregate['min'][2]))
            else:
                report['errors'].append('Nonfinite or empty equipment bounds at frame %d: %s' % (frame, label))
            sample['equipment'][label] = aggregate
        if body is not None:
            evaluated_body = body.evaluated_get(graph)
            invalid = sum(not finite(evaluated_body.matrix_world @ vertex.co) for vertex in evaluated_body.data.vertices)
            summary['body_nonfinite_vertices'] += invalid
            sample['body_vertices'] = len(evaluated_body.data.vertices)
            sample['body_nonfinite_vertices'] = invalid
        else:
            report['errors'].append('Body mesh is missing')
        report['frame_samples'].append(sample)
    if not math.isfinite(summary['min_equipment_z']):
        summary['min_equipment_z'] = None
    report['motion_summary'] = summary
    if summary['max_foot_translation_drift'] > 0.002 or summary['max_foot_rotation_drift'] > 0.002:
        report['errors'].append('Planted feet drift beyond tolerance')
    if summary['max_hand_target_error'] > 0.02 or summary['max_foot_target_error'] > 0.02:
        report['errors'].append('IK endpoints miss targets by more than 0.02 units')
    if summary['unreachable_arm_frames'] or summary['unreachable_leg_frames']:
        report['errors'].append('Some limb IK targets are outside reachable range by more than 0.02 units')
    if summary['body_nonfinite_vertices']:
        report['errors'].append('Evaluated body contains nonfinite vertex coordinates')
    if summary['sudden_limb_rotation_frames']:
        report['warnings'].append('Limb rotation jumps exceed 50 degrees per frame; inspect reported frames visually')
    report['glb'] = read_glb(ROOT / 'orc_warrior_attack.glb')

try:
    validate()
except Exception:
    report['errors'].append('Validator exception: ' + traceback.format_exc())
report['passed'] = not report['errors']
REPORT_PATH.write_text(json.dumps(report, indent=2, ensure_ascii=False, allow_nan=False), encoding='utf-8')
print('RIG_VALIDATION=' + json.dumps({
    'passed': report['passed'], 'report': str(REPORT_PATH),
    'errors': report['errors'], 'warnings': report['warnings'],
    'motion_summary': report.get('motion_summary'), 'glb': report.get('glb')}, ensure_ascii=False), flush=True)
