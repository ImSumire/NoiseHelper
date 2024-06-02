import pygame as pg
import pygame_gui as pgg
from time import perf_counter
from noise import snoise2


W, H = 1280, 720
cx, cy = 0, 0

pg.init()
screen = pg.display.set_mode((W, H), pg.RESIZABLE)
manager = pgg.UIManager((W, H))
clock = pg.time.Clock()
pg.display.set_caption("Simplex Noise Helper")

# Parameters
parms_defaults = {
    "Canva Size": (110, (1, 320)),
    "Tile Size": (6, (1, 64)),
    "Amplitude": (0.1, (0.0001, 1.0)),
    "Octaves": (1, (1, 5)),
    "Persistence": (0.5, (0.001, 1.0)),
    "Lacunarity": (2.0, (0.001, 10.0)),
}

parms = {parm: default for parm, (default, _) in parms_defaults.items()}

# GUI
padding = 80

fps_label = pgg.elements.UILabel(pg.Rect(25, 60, 300, 20), "Fps: ...", manager)

compute_label = pgg.elements.UILabel(
    pg.Rect(25, 77, 300, 20), "Computed in ...ms", manager
)

sliders = {
    parm: pgg.elements.UIHorizontalSlider(
        pg.Rect(25, 130 + i * padding, 300, 20), start_value, value_range, manager
    )
    for i, (parm, (start_value, value_range)) in enumerate(parms_defaults.items())
}

labels = {
    parm: pgg.elements.UILabel(
        pg.Rect(25, 70 + padding + i * padding, 300, 20),
        f"{parm}: {start_value}",
        manager,
    )
    for i, (parm, (start_value, _)) in enumerate(parms_defaults.items())
}


while True:
    # Update
    dt = clock.tick(60)
    fps_label.set_text(f"Fps: {clock.get_fps():.1f}")

    for event in pg.event.get():
        manager.process_events(event)
        if event.type == pg.QUIT:
            pg.quit()
            exit()

    for parm, label in labels.items():
        val = sliders[parm].get_current_value()
        parms[parm] = val
        if isinstance(val, int):
            label.set_text(f"{parm}: {val}")
        else:
            label.set_text(f"{parm}: {val:.4f}")

    manager.update(dt)

    amplitude = parms["Amplitude"]
    octaves = parms["Octaves"]
    persistence = parms["Persistence"]
    lacunarity = parms["Lacunarity"]
    canva_size = parms["Canva Size"]
    tile_size = parms["Tile Size"]

    vx, vy = pg.mouse.get_rel()
    if pg.mouse.get_pressed()[0] and pg.Rect(
        400, 25, (s := canva_size * tile_size), s
    ).collidepoint(*pg.mouse.get_pos()):
        cx -= vx
        cy -= vy
        pg.mouse.set_cursor(pg.SYSTEM_CURSOR_SIZEALL)
    else:
        pg.mouse.set_cursor(pg.SYSTEM_CURSOR_ARROW)

    # Render
    screen.fill((24, 25, 38))
    manager.draw_ui(screen)

    for y in range(canva_size):
        for x in range(canva_size):
            n = snoise2(
                (x + cx) * amplitude,
                (y + cy) * amplitude,
                octaves=octaves,
                persistence=persistence,
                lacunarity=lacunarity,
            )
            n += 1.0
            n *= 127.0
            pg.draw.rect(
                screen,
                (n, n, n),
                (x * tile_size + 400, y * tile_size + 25, tile_size, tile_size),
            )

    start = perf_counter()
    snoise2(
        amplitude,
        amplitude,
        octaves=octaves,
        persistence=persistence,
        lacunarity=lacunarity,
    )
    end = perf_counter()
    compute_label.set_text(f"Computed in {(end - start) * 1_000_000:.2f}µs")

    pg.display.flip()
