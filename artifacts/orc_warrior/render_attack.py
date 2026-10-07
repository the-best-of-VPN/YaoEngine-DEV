"""Render a shareable motion preview from the final rigged Blender document."""
import bpy
import os
import subprocess
import shutil

ROOT=os.path.dirname(os.path.abspath(__file__))
scene=bpy.context.scene
scene.frame_start=1;scene.frame_end=72
scene.render.fps=24;scene.render.fps_base=1
scene.render.resolution_x=720;scene.render.resolution_y=810
scene.render.resolution_percentage=100
scene.cycles.samples=16;scene.cycles.use_denoising=True
scene.render.image_settings.file_format='PNG'
scene.render.image_settings.color_mode='RGB'
os.makedirs(os.path.join(ROOT,'attack_frames'),exist_ok=True)
scene.render.filepath=os.path.join(ROOT,'attack_frames','frame_')
bpy.ops.render.render(animation=True)
ffmpeg=shutil.which('ffmpeg')
assert ffmpeg, 'FFmpeg is required to encode the rendered PNG sequence'
movie=os.path.join(ROOT,'orc_warrior_attack.mp4')
subprocess.run([ffmpeg,'-y','-loglevel','error','-framerate','24','-start_number','1',
    '-i',os.path.join(ROOT,'attack_frames','frame_%04d.png'),'-frames:v','72',
    '-c:v','libx264','-crf','18','-pix_fmt','yuv420p','-movflags','+faststart',movie],check=True)
subprocess.run([ffmpeg,'-y','-loglevel','error','-i',movie,'-filter_complex',
    '[0:v]fps=12,scale=480:-1:flags=lanczos,split[a][b];[a]palettegen=max_colors=192[p];[b][p]paletteuse=dither=sierra2_4a',
    '-loop','0',os.path.join(ROOT,'orc_warrior_attack.gif')],check=True)
scene.render.image_settings.file_format='PNG'
scene.render.resolution_x=960;scene.render.resolution_y=1080
scene.cycles.samples=48
scene.frame_set(22)
scene.render.filepath=os.path.join(ROOT,'orc_warrior_attack_pose.png')
bpy.ops.render.render(write_still=True)
print('ATTACK_VIDEO_COMPLETE',flush=True)
