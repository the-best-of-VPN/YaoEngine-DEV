import yao

def on_update(actor: yao.Actor, dt: float):
    actor.move(4.0 * dt)
