"""A planted, single-handed heavy axe slash, baked to the Orc IK controls.

Character front is -Y and right is -X.  Coordinates below are armature space.
The long haft is accounted for: the attack rolls the cutting edge forward,
then moves the whole axe through an open plane ahead of the character.
"""
import bpy
import math
from mathutils import Vector, Quaternion, Matrix


def build_attack(rig, scene):
    scene.frame_start = 1
    scene.frame_end = 72
    scene.render.fps = 24
    scene.render.fps_base = 1.0
    scene.frame_set(1)
    bones = rig.pose.bones
    rig.animation_data_create()
    action = bpy.data.actions.new('Orc | Heavy Axe Slash')
    rig.animation_data.action = action
    previous_quaternions = {}

    # frame, wrist R, axe yaw, axe pitch, wrist L, torso yaw, torso lean,
    # pelvis drop. Pitch 0 is an upright haft; positive pitch swings forward.
    # Rz(yaw) Ry(-pitch): at yaw 108 degrees, the blade faces (+.31,-.95).
    poses = [
        (1,  (-1.970, -.170, 3.115),   0,   0, (1.970, -.170, 3.115),   0, 0, 0),
        (8,  (-2.055, -.125, 3.690),  38, -10, (1.825, -.300, 3.270),  -4, 0, -.018),
        (14, (-1.930,  .095, 4.345),  82, -22, (1.680, -.425, 3.430),  -9, -1, -.040),
        (20, (-1.750,  .150, 4.650), 108, -32, (1.600, -.470, 3.540), -12, -2, -.055),
        (23, (-1.685, -.060, 4.725), 108,  -7, (1.565, -.525, 3.580),  -6, 1, -.055),
        (26, (-1.510, -.585, 4.545), 108,  40, (1.580, -.560, 3.590),   2, 5, -.075),
        (29, (-1.345, -1.020, 4.180),108,  83, (1.630, -.535, 3.540),   8, 8, -.085),
        (34, (-1.290, -1.040, 3.845),108, 109, (1.685, -.495, 3.460),  11, 9, -.080),
        (40, (-1.315,  -.940, 3.700),108, 118, (1.725, -.450, 3.390),   9, 7, -.067),
        (44, (-1.430,  -.740, 3.705),105, 111, (1.760, -.410, 3.340),   7, 5, -.050),
        (54, (-1.875,  -.490, 3.645), 88,  69, (1.840, -.310, 3.245),   3, 3, -.028),
        (63, (-2.035,  -.255, 3.330), 43,  25, (1.935, -.210, 3.145),   1, 1, -.010),
        (72, (-1.970,  -.170, 3.115),  0,   0, (1.970, -.170, 3.115),   0, 0, 0),
    ]

    def quat(axis, degrees):
        return Quaternion(axis, math.radians(degrees))

    def body_rotation(twist, lean, lateral=0):
        return quat((0, 0, 1), twist) @ quat((1, 0, 0), lean) @ quat((0, 1, 0), lateral)

    def interpolate(frame):
        for i in range(len(poses) - 1):
            a, b = poses[i], poses[i + 1]
            if a[0] <= frame <= b[0]:
                t = (frame - a[0]) / float(b[0] - a[0])
                # The fast strike uses a continuous near-linear middle sweep;
                # the larger preparation and recovery intervals ease in/out.
                if a[0] >= 20 and b[0] <= 29:
                    if a[0] == 20:
                        t = t * t * (1.55 - .55 * t)
                    else:
                        t = .18 * (t * t * (3 - 2 * t)) + .82 * t
                else:
                    t = t * t * (3 - 2 * t)
                vals = [frame]
                for j in range(1, len(a)):
                    if isinstance(a[j], tuple):
                        vals.append(Vector(a[j]).lerp(Vector(b[j]), t))
                    else:
                        vals.append(a[j] + (b[j] - a[j]) * t)
                return vals
        raise ValueError('Attack frame outside baked range')

    def key_pose(pb, frame, location=True):
        pb.rotation_mode = 'QUATERNION'
        q = pb.rotation_quaternion.copy()
        old = previous_quaternions.get(pb.name)
        if old is not None and q.dot(old) < 0:
            q.negate()
            pb.rotation_quaternion = q
        previous_quaternions[pb.name] = q.copy()
        if location:
            pb.keyframe_insert(data_path='location', frame=frame, group=pb.name)
        pb.keyframe_insert(data_path='rotation_quaternion', frame=frame, group=pb.name)

    def set_world_control(name, location, rotation, frame):
        pb = bones[name]
        pb.rotation_mode = 'QUATERNION'
        desired = rotation.to_matrix().to_4x4() @ pb.bone.matrix_local
        desired.translation = Vector(location)
        pb.matrix = desired
        bpy.context.view_layer.update()
        key_pose(pb, frame)

    def set_body(name, delta, frame, offset=None):
        pb = bones[name]
        pb.rotation_mode = 'QUATERNION'
        desired = delta.to_matrix().to_4x4() @ pb.bone.matrix_local
        if pb.parent:
            parent_delta = pb.parent.matrix @ pb.parent.bone.matrix_local.inverted()
            desired.translation = parent_delta @ pb.bone.head_local
        else:
            desired.translation = pb.bone.head_local
        if offset is not None:
            desired.translation += Vector(offset)
        pb.matrix = desired
        bpy.context.view_layer.update()
        key_pose(pb, frame)

    def set_local_global_axis(name, axis, degrees, frame):
        # Convert a world-axis additive rotation into this bone's rest axes.
        pb = bones.get(name)
        if pb is None:
            return
        rest_q = pb.bone.matrix_local.to_quaternion()
        pb.rotation_mode = 'QUATERNION'
        pb.rotation_quaternion = rest_q.inverted() @ quat(axis, degrees) @ rest_q
        key_pose(pb, frame, location=False)

    def reachable(side, target, frame):
        shoulder = bones['upper_arm.' + side].head.copy()
        delta = Vector(target) - shoulder
        # Frame 1 and 72 preserve the exact authored rest pose (1.628 m).
        if frame not in (1, 72) and delta.length > 1.620:
            target = shoulder + delta.normalized() * 1.620
        return Vector(target)

    identity = Quaternion((1, 0, 0, 0))
    pole_rest = {side: bones['CTRL_elbow.' + side].bone.head_local.copy() for side in ('R', 'L')}
    foot_rest = {side: bones['CTRL_foot.' + side].bone.head_local.copy() for side in ('R', 'L')}
    knee_rest = {side: bones['CTRL_knee.' + side].bone.head_local.copy() for side in ('R', 'L')}
    # The actual blade profile, not just the centre, is used for clearance QA.
    blade_outline = [(-.08, .22), (-.35, .31), (-.67, .54), (-.96, .58),
                     (-1.12, .39), (-1.19, .05), (-1.10, -.35), (-.86, -.63),
                     (-.68, -.30), (-.33, -.13), (-.08, -.14)]
    blade_points = [Vector((-1.98 + x, y, 4.72 + z))
                    for x, z in blade_outline for y in (-.50, -.24)]
    blade_points += [Vector((-1.59, -.37, 4.89)), Vector((-1.98, -.37, 5.13))]
    haft_points = [Vector((-1.98 + x, -.37 + y, z))
                   for z in (.80, 5.13) for x, y in ((-.12, 0), (.12, 0), (0, -.12), (0, .12))]
    stats = {
        'action': action.name,
        'frames': 72,
        'fps': 24,
        'duration_seconds': 3.0,
        'minimum_blade_height': 100.0,
        'minimum_weapon_height': 100.0,
        'maximum_weapon_height': -100.0,
        'maximum_wrist_reach_R': 0.0,
        'maximum_wrist_reach_L': 0.0,
        'maximum_wrist_target_error': 0.0,
        'maximum_foot_drift': 0.0,
        'clearance_limit_blade': 1.0,
        'sampled_keyframes': [],
        'head_clearance_note': 'Blade stays on the right during raising, then travels forward of the torso. Visual contact-sheet review remains required.',
    }

    for frame in range(1, 73):
        scene.frame_set(frame)
        _, wrist_r, yaw, pitch, wrist_l, twist, lean, drop = interpolate(frame)
        load = max(0.0, min(1.0, -drop / .085))
        # Torso offsets stay small; fixed IK ankles keep both soles planted.
        set_body('pelvis', body_rotation(twist * .30, lean * .22), frame,
                 (0, -.024 * load, drop))
        set_body('spine', body_rotation(twist * .64, lean * .58), frame)
        set_body('chest', body_rotation(twist, lean), frame)
        set_body('neck', body_rotation(twist * .72, lean * .67), frame)
        set_body('head', body_rotation(twist * .40, lean * .40), frame)

        wrist_r = reachable('R', wrist_r, frame)
        wrist_l = reachable('L', wrist_l, frame)
        axe_q = quat((0, 0, 1), yaw) @ quat((0, 1, 0), -pitch)
        shield_q = body_rotation(-7.0 * load, 7.5 * load, -3.5 * load)
        set_world_control('CTRL_hand.R', wrist_r, axe_q, frame)
        set_world_control('CTRL_hand.L', wrist_l, shield_q, frame)

        # Elbows remain on their respective outer sides. Preserve rest poles
        # exactly at loop ends because the rig's pole angles were calibrated.
        p_r = pole_rest['R'].lerp(Vector((-2.95, .64, 4.10)), load)
        p_l = pole_rest['L'].lerp(Vector((2.90, .78, 3.90)), load)
        set_world_control('CTRL_elbow.R', p_r, identity, frame)
        set_world_control('CTRL_elbow.L', p_l, identity, frame)
        for side in ('R', 'L'):
            set_world_control('CTRL_foot.' + side, foot_rest[side], identity, frame)
            set_world_control('CTRL_knee.' + side, knee_rest[side], identity, frame)

        # A small delayed cloth response adds weight without crossing boots.
        recoil = math.sin(math.pi * min(1.0, max(0.0, (frame - 26) / 28.0)))
        set_local_global_axis('skirt.front', (1, 0, 0), -3.5 * load - 4.0 * recoil, frame)
        set_local_global_axis('skirt.back', (1, 0, 0), 2.0 * load + 2.0 * recoil, frame)
        set_local_global_axis('skirt.R', (0, 1, 0), 2.5 * load, frame)
        set_local_global_axis('skirt.L', (0, 1, 0), -2.5 * load, frame)
        bpy.context.view_layer.update()

        weapon = bones['weapon.axe']
        weapon_delta = weapon.matrix @ weapon.bone.matrix_local.inverted()
        transformed_blade = [weapon_delta @ p for p in blade_points]
        transformed_haft = [weapon_delta @ p for p in haft_points]
        blade_min = min(p.z for p in transformed_blade)
        all_points = transformed_blade + transformed_haft
        stats['minimum_blade_height'] = min(stats['minimum_blade_height'], blade_min)
        stats['minimum_weapon_height'] = min(stats['minimum_weapon_height'], min(p.z for p in all_points))
        stats['maximum_weapon_height'] = max(stats['maximum_weapon_height'], max(p.z for p in all_points))
        for side, wrist in (('R', wrist_r), ('L', wrist_l)):
            reach = (wrist - bones['upper_arm.' + side].head).length
            stats['maximum_wrist_reach_' + side] = max(stats['maximum_wrist_reach_' + side], reach)
            error = (bones['hand.' + side].head - wrist).length
            stats['maximum_wrist_target_error'] = max(stats['maximum_wrist_target_error'], error)
            drift = (bones['foot.' + side].head - foot_rest[side]).length
            stats['maximum_foot_drift'] = max(stats['maximum_foot_drift'], drift)
        if frame in (1, 8, 14, 20, 23, 26, 29, 34, 40, 44, 54, 63, 72):
            stats['sampled_keyframes'].append({
                'frame': frame,
                'right_wrist': [round(v, 4) for v in wrist_r],
                'shaft_direction': [round(v, 4) for v in (axe_q @ Vector((0, 0, 1)))],
                'minimum_blade_height': round(blade_min, 4),
                'highest_weapon_point': round(max(p.z for p in all_points), 4),
            })

    markers = [('01 • Ready', 1), ('02 • Wind-up', 8), ('03 • Full charge', 20),
               ('04 • Fast diagonal slash', 24), ('05 • Impact', 29),
               ('06 • Follow-through', 40), ('07 • Recover', 45), ('08 • Ready / loop', 72)]
    for name, frame in markers:
        marker = scene.timeline_markers.get(name)
        if marker is None:
            marker = scene.timeline_markers.new(name, frame=frame)
        else:
            marker.frame = frame
    for key, value in list(stats.items()):
        if isinstance(value, float):
            stats[key] = round(value, 5)
    stats['blade_height_check_passed'] = stats['minimum_blade_height'] > 1.0
    stats['floor_check_passed'] = stats['minimum_weapon_height'] > .25
    scene['attack_motion'] = '72 frames / 24 fps; wind-up 1-20, slash 21-29, follow-through 30-44, recovery 45-72.'
    scene['attack_clearance_min_blade_z'] = stats['minimum_blade_height']
    scene['attack_clearance_min_weapon_z'] = stats['minimum_weapon_height']
    scene.frame_set(1)
    bpy.context.view_layer.update()
    return stats
