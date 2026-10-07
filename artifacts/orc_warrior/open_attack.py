"""Start playback in the newly opened Blender animation document."""
import bpy

def preview():
    windows=list(bpy.context.window_manager.windows)
    if not windows:
        return 1.0
    window=windows[0]
    area=next((a for a in window.screen.areas if a.type=='VIEW_3D'),None)
    if area:
        with bpy.context.temp_override(window=window,area=area):
            bpy.context.scene.frame_set(1)
            if not window.screen.is_animation_playing:
                bpy.ops.screen.animation_play()
    return None

bpy.app.timers.register(preview,first_interval=4.0)
