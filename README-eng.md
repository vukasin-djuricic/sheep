# sheeps  
**2D Computer Graphics Project**

## Documentation

A meadow with sheep.  
When the user hovers over a sheep with the **right mouse button (RMB)**, particles scatter and the sheep is shaven.  
After a while (`shaven_timeout`), the wool grows back – this only applies to the **main sheep** controlled with **WASD**.

There is a **well** placed randomly on the field.  
Clicking on the well **spawns a new sheep**.  
To spawn sheep faster, the **Q key** can be used (no cooldown).

There is a **20% chance** that a **negative sheep** spawns, triggering a **storm**, which lasts until the sheep is shaven.  
When a negative sheep is created, the **negative filter** is applied via `draw_spritesheet_negative()` from `rafgl.h`.

The storm consists of **4 phases**:
1. A **darkened circle** (with a gradient around the edge to resemble a cloud) expands to radius 100,  
2. A **2-second pause**,  
3. The circle **expands to fill the entire screen**,  
4. **Thunder** occurs.

Thunder includes **flickering effects** to simulate lightning flashes (brightness changes twice).  
The **lightning structure** is generated using **L-systems**, where points between the top and bottom of the lightning bolt are created recursively using **random weights**.  
To simulate lightning glow, a **Gaussian blur** (using 4 neighboring pixels) is applied.

Once the **negative sheep is shaven** (RMB), the **storm stops**.

While holding the **T key**, **10 rotating orbs** form around the main sheep.  
Once fully formed, **lightning is drawn between the main sheep and the negative one**.

The orbs rotate using trigonometric functions:  
- `y = sin(counter)`,  
- `x = cos(counter)`,  
and the **counter increases every frame**.

---

## Controls

- `T` – Hitbox test: draws lightning after 10 orbs appear  
- `RMB` – Shave a sheep  
- `LMB` – Click on well to spawn a sheep  
- `Q` – Spawn sheep without cooldown  
- `LMB (hold)` – Drag the main sheep around

---

## Modified

**rafgl.h**  
Lines `653`, `686`: Spritesheets had a thin line at the top (likely due to format) – drawing now starts after it.

---

## Sprites

Downloaded from:  
[https://www.spriters-resource.com/pc_computer/stardewvalley/sheet/169024/](https://www.spriters-resource.com/pc_computer/stardewvalley/sheet/169024/)
