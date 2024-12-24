//4hero
#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <rafgl.h>
#include <game_constants.h>

// __________________________________________________________________________________________________________
//   STORM / LIGHTNING PROMENLJIVE
//_____________________________________________________
static int storm_active = 0;
static int storm_phase = 0;
static float storm_radius = 0.0f;
static float storm_speed = 50.0f;
static float storm_speed_fast = 800.0f;
static float storm_wait_time = 0.0f;
int storm_sheep_pos_x = -7;
int storm_sheep_pos_y = -7;
int storm_sheep_index = -7;

static float lightning_cooldown = 0.0f;
static int lightning_active = 0;
static float lightning_timer = 0.0f;
static int lightning_flicker_count = 0;
static int lightning_flicker_state = 0;
static float lightning_flicker_timer = 0.0f;
static float lightning_flicker_duration = 0.4f;

static int lightning_x = 0;
static int lightning_y_end = 0;
static int lightning_width = 6;
static float glow_radius = 15.0f;

//za flicker
static rafgl_pixel_rgb_t lightning_color1;
static rafgl_pixel_rgb_t lightning_color2;



static rafgl_raster_t doge;
static rafgl_raster_t upscaled_doge;
static rafgl_raster_t raster, raster2;
static rafgl_raster_t checker;
static rafgl_texture_t texture;
static rafgl_spritesheet_t hero, hero_i, sheepShaven, sheepShaven_i, well;
static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;
static char save_file[256];
int save_file_no = 0;

typedef struct _particle_t
{
    float x, y, dx, dy;
    int life;
} particle_t;

#define MAX_PARTICLES 500
particle_t particles[MAX_PARTICLES];
int particlesDirty[MAX_PARTICLES] = {0};
static float elasticity = 0.6;

typedef enum {
    SHEEP_STATE_IDLE,
    SHEEP_STATE_MOVE
} sheep_state_t;

typedef struct _sheep_t {
    float x, y;
    float dx, dy;
    int cooldown;
    int shaven;
    int frame;
    int direction;
    int hover_frames;
    int idle_dir;
    sheep_state_t state;
    float home_x, home_y;
    float move_radius;
    int dirty;  // da li je negative/dirty ovca
} sheep_t;

#define MAX_SHEEP 50
sheep_t sheep_array[MAX_SHEEP];
int sheep_count = 0;

#define SHEEP_MOVE_COOLDOWN 200
#define SHEEP_IDLE_COOLDOWN 100
#define SHEEP_SPEED 20

#define NUMBER_OF_TILES 17
rafgl_raster_t tiles[NUMBER_OF_TILES];
#define WORLD_SIZE 128
int tile_world[WORLD_SIZE][WORLD_SIZE];
#define TILE_SIZE 64

int camx = 600, camy = 600;
int well_pos_x, well_pos_y;


void init_tilemap(void)
{
    int x, y;
    for(y = 0; y < WORLD_SIZE; y++)
    {
        for(x = 0; x < WORLD_SIZE; x++)
        {
            if(randf() <0.2f)
            {
                tile_world[y][x] = 3 + rand() % 3;
            }
            else
            {
                tile_world[y][x] = rand() % 3;
            }
        }
    }
}

void render_tilemap(rafgl_raster_t *raster)
{
    int x, y;
    int x0 = camx / TILE_SIZE;
    int x1 = x0 + (raster_width / TILE_SIZE) + 1;
    int y0 = camy / TILE_SIZE;
    int y1 = y0 + (raster_height / TILE_SIZE) + 2;

    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    if(x0 >= WORLD_SIZE) x0 = WORLD_SIZE - 1;
    if(y0 >= WORLD_SIZE) y0 = WORLD_SIZE - 1;
    if(x1 >= WORLD_SIZE) x1 = WORLD_SIZE - 1;
    if(y1 >= WORLD_SIZE) y1 = WORLD_SIZE - 1;

    rafgl_raster_t *draw_tile;
    int drawWell = 0;
    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
        {
            draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
            rafgl_raster_draw_raster(raster, draw_tile,
                x * TILE_SIZE - camx,
                y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);

            if(drawWell == 0 && x-1 > x1/2 && y-1 > y1/2
                && tile_world[y][x] < 3 && tile_world[y-1][x] < 3)
            {
                drawWell = 1;
                well_pos_x = x * TILE_SIZE - camx;
                well_pos_y = y * TILE_SIZE - camy;
                rafgl_raster_draw_spritesheet(raster,&well,0,0,well_pos_x,well_pos_y);
            }
        }
    }
}


void draw_particles(rafgl_raster_t *raster)
{
    int i;
    for(i = 0; i < MAX_PARTICLES; i++)
    {
        if(particles[i].life <= 0) continue;
        int size = 3 + (particles[i].life / 50);
        int shade = 100 + (155 * particles[i].life / 300);
        if(particlesDirty[i]==1)
            rafgl_raster_draw_circle(raster, particles[i].x, particles[i].y, size, rafgl_RGB(255,215,0));
        else
            rafgl_raster_draw_circle(raster, particles[i].x, particles[i].y, size, rafgl_RGB(shade, shade, shade));
    }
}

void update_particles()
{
    int i;
    for(i = 0; i < MAX_PARTICLES; i++)
    {
        if(particles[i].life <= 0) continue;
        particles[i].life--;
        particles[i].x += particles[i].dx;
        particles[i].y += particles[i].dy;
        particles[i].dx *= 0.995f;
        particles[i].dy *= 0.995f;
        particles[i].dy -= 0.01;

        particles[i].x = rafgl_clampf(particles[i].x, 0, raster_width - 1);
        particles[i].y = rafgl_clampf(particles[i].y, 0, raster_height - 1);

        if(particles[i].x <= 20 || particles[i].x >= raster_width - 20)
        {
            particles[i].dx = -particles[i].dx * elasticity;
        }

        if(particles[i].y <= 20 || particles[i].y >= raster_height - 20)
        {
            particles[i].dy = -particles[i].dy * elasticity;
        }
    }
}

void particleFunc(float cx, float cy)
{
    int gen = 50, radius = 2;
    float angle, speed;

    for(int i = 0; (i < MAX_PARTICLES) && gen; i++)
    {
        if(particles[i].life <= 0)
        {
            particles[i].life = 250 + rand() % 150;
            particles[i].x = cx;
            particles[i].y = cy;

            angle = randf() * M_PI * 2.0f;
            speed = (0.1f + 0.3f * randf()) * radius;
            particles[i].dx = cosf(angle) * speed;
            particles[i].dy = sinf(angle) * speed;
            gen--;
        }
    }
    update_particles();
}

void dirtyParticleFunc(float cx, float cy)
{
    int gen = 300, radius = 2;
    float angle, speed;

    for(int i = 0; (i < MAX_PARTICLES) && gen; i++)
    {
        if(particles[i].life <= 0)
        {
            particles[i].life = 300 + rand() % 200;
            particles[i].x = cx;
            particles[i].y = cy;

            angle = randf() * M_PI * 2.0f;
            speed = (0.1f + 0.5f * randf()) * radius;
            particles[i].dx = cosf(angle) * speed;
            particles[i].dy = sinf(angle) * speed;
            gen--;
        }
    }
    update_particles();
}

static int any_dirty_sheep(void)
{
    for(int i=0; i<MAX_SHEEP; i++)
    {
        if(sheep_array[i].x < 0) continue;
        if(sheep_array[i].dirty == 1 && sheep_array[i].shaven == 0)
        {
            return 1;
        }
    }
    return 0;
}

void spawn_sheep()
{
    // Ne moze ako traje oluja
    if(storm_active) return;

    if(sheep_count >= MAX_SHEEP) return;
    for(int i=0; i<MAX_SHEEP; i++)
    {
        if(sheep_array[i].x <= 0)
        {
            sheep_array[i].x = rand() % (raster_width - 64);
            sheep_array[i].y = rand() % (raster_height - 64);
            sheep_array[i].dx = 0;
            sheep_array[i].dy = 0;
            sheep_array[i].state = SHEEP_STATE_IDLE;
            sheep_array[i].cooldown = SHEEP_IDLE_COOLDOWN + rand()%100;
            sheep_array[i].shaven = 0;
            sheep_array[i].frame = 0;
            sheep_array[i].direction = 4;
            sheep_array[i].hover_frames = 0;
            sheep_array[i].idle_dir = 1;
            sheep_array[i].home_x = sheep_array[i].x;
            sheep_array[i].home_y = sheep_array[i].y;
            sheep_array[i].move_radius = 100 + rand() % 100;

            sheep_array[i].dirty = (randf() < 0.2f) ? 1 : 0;
            if(sheep_array[i].dirty == 1)
            {
                particlesDirty[i] = 1;
                storm_active = 1;
                storm_phase = 1;
                storm_radius = 0.0f;
                storm_wait_time = 0.0f;
                storm_sheep_pos_x = sheep_array[i].x;
                storm_sheep_pos_y = sheep_array[i].y;
                storm_sheep_index = i;
                ///DA TRAJE dok se ne zavrsi animacija (conv HULL)
                sheep_array[i].cooldown = 300;
                //grmljavina promenjive
                lightning_active = 0;
                lightning_cooldown = 2.0f;
                lightning_timer = 0.0f;
                lightning_flicker_count = 0;
                lightning_flicker_state = 0;
                lightning_flicker_timer = 0.0f;
            }
            else
            {
                particlesDirty[i] = 0;
            }

            sheep_count++;
            break;
        }
    }
}

void update_sheep(float delta_time)
{
    int i;
    for(i=0; i<MAX_SHEEP; i++)
    {
        if(sheep_array[i].x < 0) continue;
        sheep_t *sh = &sheep_array[i];

        sh->cooldown--;

        float dist_home = rafgl_distance2D(sh->x, sh->y, sh->home_x, sh->home_y);

        if(sh->state == SHEEP_STATE_IDLE)
        {
            sh->direction = 4;
            if(sh->hover_frames == 0)
            {
                sh->frame += sh->idle_dir;
                if(sh->frame >= 3 || sh->frame <= 0)
                    sh->idle_dir *= -1;
                sh->frame %= 4;
                if(sh->frame == 3 || sh->frame == 0)
                    sh->hover_frames = 80;
                else
                    sh->hover_frames = 40;
            }
            else
            {
                sh->hover_frames--;
            }

            if(sh->cooldown <= 0)
            {
                if(randf() < 0.7f)
                {
                    sh->state = SHEEP_STATE_MOVE;
                    float angle = randf() * M_PI * 2.0f;
                    if(dist_home > sh->move_radius * 0.8f)
                    {
                        angle = atan2f(sh->home_y - sh->y, sh->home_x - sh->x);
                    }

                    sh->dx = cosf(angle) * SHEEP_SPEED;
                    sh->dy = sinf(angle) * SHEEP_SPEED;
                    sh->cooldown = SHEEP_MOVE_COOLDOWN + rand()%100;
                }
                else
                {
                    sh->cooldown = SHEEP_IDLE_COOLDOWN + rand()%100;
                }
            }

        }
        else // SHEEP_STATE_MOVE
        {
            sh->x += sh->dx * delta_time;
            sh->y += sh->dy * delta_time;

            if(dist_home > sh->move_radius)
            {
                sh->x = rafgl_clampf(sh->x, sh->home_x - sh->move_radius, sh->home_x + sh->move_radius);
                sh->y = rafgl_clampf(sh->y, sh->home_y - sh->move_radius, sh->home_y + sh->move_radius);
                sh->dx = 0;
                sh->dy = 0;
                sh->state = SHEEP_STATE_IDLE;
                sh->cooldown = SHEEP_IDLE_COOLDOWN + rand()%100;
            }

            int moving = (fabs(sh->dx) > 1 || fabs(sh->dy) > 1);
            if(moving)
            {
                if(fabs(sh->dx) > fabs(sh->dy))
                {
                    if(sh->dx > 0)
                        sh->direction = 1;
                    else
                        sh->direction = 11;
                }
                else
                {
                    if(sh->dy > 0)
                        sh->direction = 0;
                    else
                        sh->direction = 2;
                }

                if(sh->hover_frames == 0)
                {
                    sh->frame = (sh->frame + 1) % 4;
                    sh->hover_frames = 10;
                }
                else
                {
                    sh->hover_frames--;
                }
            }
            else
            {
                sh->state = SHEEP_STATE_IDLE;
                sh->cooldown = SHEEP_IDLE_COOLDOWN + rand()%100;
            }


            if(sh->x < 0) sh->x = 0;
            if(sh->x > raster_width - 64) sh->x = raster_width - 64;
            if(sh->y < 0) sh->y = 0;
            if(sh->y > raster_height - 64) sh->y = raster_height - 64;
            //sh->x = rafgl_saturatei(raster_width-64);
            //sh->y = rafgl_saturatei(raster_height-64);
        }
    }
}

void draw_sheep(rafgl_raster_t *raster)
{
    for(int i=0; i<MAX_SHEEP; i++)
    {
        draw_sheep_specific(raster,i);
    }
}

void draw_sheep_specific(rafgl_raster_t *raster, int i)
{
    if(sheep_array[i].x < 0) return;
        sheep_t *sh = &sheep_array[i];

        if(sh->x < 0 || sh->y < 0)
        {
            sh->x = raster_width / 2;
            sh->y = raster_height / 2;
        }

        if(!sh->shaven)
        {
            if(sh->direction == 11)
            {
                if(sh->dirty == 1)
                    rafgl_raster_draw_spritesheet_negative(raster, &hero_i, sh->frame, 1, sh->x, sh->y);
                else
                    rafgl_raster_draw_spritesheet(raster, &hero_i, sh->frame, 1, sh->x, sh->y);
            }
            else
            {
                if(sh->dirty == 1)
                    rafgl_raster_draw_spritesheet_negative(raster, &hero, sh->frame, sh->direction, sh->x, sh->y);
                else
                    rafgl_raster_draw_spritesheet(raster, &hero, sh->frame, sh->direction, sh->x, sh->y);
            }
        }
        else
        {
            if(sh->direction == 11)
            {
                rafgl_raster_draw_spritesheet(raster, &sheepShaven_i, sh->frame, 1, sh->x, sh->y);
            }
            else
            {
                rafgl_raster_draw_spritesheet(raster, &sheepShaven, sh->frame, sh->direction, sh->x, sh->y);
            }
        }
}

/*int is_point_inside_convex_hull(int polygon[10][2], int px, int py) {
    for (int i = 0; i < 11; i++) {
        // Trenutna ivica (A -> B)
        int ax = polygon[i][0], ay = polygon[i][1];
        int bx = polygon[(i + 1) % 10][0], by = polygon[(i + 1) % 10][1];

        // Vektorski proizvod: (B - A) x (P - A)
        int cross_product = (bx - ax) * (py - ay) - (by - ay) * (px - ax);

        // Ako je vektorski proizvod manji od 0, tacka je desno -> nije unutar omotaca
        if (cross_product < 0) {
            return 0;
        }
    }
    // Ako je tacka levo od svih pravih, unutar je omotaca
    return 1;
}*/

#define MAX_SEGMENTS 500

typedef struct {
    int x1, y1, x2, y2;
} LightningSegment;

typedef struct {
    LightningSegment segments[MAX_SEGMENTS];
    int count;
} LightningPath;

void generate_lightning(LightningPath *path, int x1, int y1, int x2, int y2, int depth, float displacement) {
    if (depth == 0 || path->count >= MAX_SEGMENTS)
    {
        path->segments[path->count++] = (LightningSegment){x1, y1, x2, y2};
        return;
    }

    //random opsge
    int mx = (x1 + x2) / 2 + (rand() % (int)(2 * displacement)) - displacement;
    int my = (y1 + y2) / 2 + (rand() % (int)(2 * displacement)) - displacement;

    generate_lightning(path, x1, y1, mx, my, depth - 1, displacement / 2.0f);
    generate_lightning(path, mx, my, x2, y2, depth - 1, displacement / 2.0f);
}

void render_lightning(rafgl_raster_t *raster, LightningPath *path, rafgl_pixel_rgb_t color) {
    for (int i = 0; i < path->count; i++) {
        LightningSegment seg = path->segments[i];
        rafgl_raster_draw_line(raster, seg.x1, seg.y1, seg.x2, seg.y2,rafgl_RGB(color.r,color.g,color.b));
        //rafgl_raster_draw_line()

        //gaus
        for (int offset = -2; offset <= 2; offset++) {
            if (offset == 0) continue;
            rafgl_raster_draw_line(raster, seg.x1 + offset, seg.y1 + offset, seg.x2 + offset, seg.y2 + offset,
                                   rafgl_RGB(89, 186, 231)); // Glow color
        }
    }
}

void draw_lightning_effect(rafgl_raster_t *raster, int x1, int y1, int x2, int y2) {
    LightningPath lightning_path = {0};

    generate_lightning(&lightning_path, x1, y1, x2, y2, 5, 100.0f);

    rafgl_pixel_rgb_t main_color;
    main_color.rgba = rafgl_RGB(255, 255, 255);
    render_lightning(raster, &lightning_path, main_color);
}



int is_point_inside_convex_hull(int polygon[10][2], int px, int py) {//vektorskim proizvodom
    int n = 10;

    int ax = polygon[0][0], ay = polygon[0][1];
    int bx = polygon[1][0], by = polygon[1][1];

    for (int i = 0; i < 10; i++)
    {
        int ax = polygon[i][0], ay = polygon[i][1];
        int bx = polygon[(i + 1) % n][0], by = polygon[(i + 1) % n][1];
        int cross_product = (bx - ax) * (py - ay) - (by - ay) * (px - ax);

        if (cross_product < 0) {
            return 0;
        }
    }
    //unutra 1, van 0
    return 1;
}


int circles_spawn_angle = 0; // pomeraj za ugao
int circles_spawn_order = 0; // da se stvaraju jedan po jedan
void draw_spawn_circles(rafgl_raster_t *raster)
{
    //SPAWN ANIMATION

        storm_sheep_pos_x = sheep_array[storm_sheep_index].x;//update pos
        storm_sheep_pos_y = sheep_array[storm_sheep_index].y;
        int br = circles_spawn_angle;
        circles_spawn_angle--;
        circles_spawn_order = circles_spawn_order <= 400 ? ++circles_spawn_order:circles_spawn_order;
        if(circles_spawn_order/40 >= 10)
        {
            int tmpArr [10][2];//x i y u vrsti
            int index = 0;
            for(int i = 0; i<circles_spawn_order/40; i++)
            {
                float radian_br = br * (M_PI / 180.0f);
                int tx = cosf(radian_br) * 50;
                int ty = sinf(radian_br) * 50;
                br += 36;
                rafgl_raster_draw_circle(raster,storm_sheep_pos_x+32+tx-3,storm_sheep_pos_y+40+ty+3,5,rafgl_RGB(1,255,4));
                tmpArr[i][0] = storm_sheep_pos_x+32+tx;
                tmpArr[i][1] = storm_sheep_pos_y+32+ty;
            }
            //stavi tacke u niz
            //SPOJI IH

            rafgl_pixel_rgb_t c;
            c.rgba = rafgl_RGB(1,255,4);//(89,186,231);
            for(int y = storm_sheep_pos_y-100; y<storm_sheep_pos_y+100;y++)
            {
                for(int x = storm_sheep_pos_x-100; x<storm_sheep_pos_x+100;x++)
                {
                    if(is_point_inside_convex_hull(tmpArr,x,y))
                        pixel_at_pm(raster,x,y) = c;
                }
            }
            draw_sheep_specific(raster, storm_sheep_index);
            return;
        }
        for(int i = 0; i<circles_spawn_order/40; i++)
        {
            float radian_br = br * (M_PI / 180.0f);
            int tx = cosf(radian_br) * 50;
            int ty = sinf(radian_br) * 50;
            br += 36;
            rafgl_raster_draw_circle(raster,storm_sheep_pos_x+32+tx,storm_sheep_pos_y+40+ty,6,rafgl_RGB(1,255,4));//(89,186,231));
        }

}

static void apply_storm_effect(rafgl_raster_t *rast, float delta_time)
{
    if(!storm_active) return;

    float diag = sqrtf(raster_width * raster_width + raster_height * raster_height);

    switch(storm_phase)
    {
    case 1:
    {
        //storm_phase = 4;/// TESTING_______________________________________________
        float R1 = 100.0f;
        float fade_width = 20.0f;
        float darkness_center = 0.5f;

        storm_radius += storm_speed * delta_time;
        if(storm_radius > R1)
        {
            storm_radius = R1;
            storm_phase = 2;  // prelazimo u fazu cekanja
            storm_wait_time = 2.0f;
        }

        //iscrtavanje fade kruga
        for(int y = 0; y < raster_height; y++)
        {
            for(int x = 0; x < raster_width; x++)
            {
                float dist = rafgl_distance2D(x, y, well_pos_x+20, well_pos_y+20);

                if(dist < storm_radius - fade_width)
                {
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness_center);
                }
                else if(dist < storm_radius)
                {
                    float border_factor = (dist - (storm_radius - fade_width)) / fade_width;
                    float darkness = darkness_center * (1.0f - border_factor);
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness);
                }
                // van kruga -> original
            }
        }
    }
    break;

    //wait na deltatime
    case 2:
    {
        float fade_width = 20.0f;
        float darkness_center = 0.5f;

        if(storm_wait_time > 0.0f)
        {
            storm_wait_time -= delta_time;
            if(storm_wait_time < 0.0f) storm_wait_time = 0.0f;
        }

        // Iscrtavamo isti krug
        for(int y = 0; y < raster_height; y++)
        {
            for(int x = 0; x < raster_width; x++)
            {
                float dist = rafgl_distance2D(x, y, well_pos_x+20, well_pos_y+20);

                if(dist < storm_radius - fade_width)
                {
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness_center);
                }
                else if(dist < storm_radius)
                {
                    float border_factor = (dist - (storm_radius - fade_width)) / fade_width;
                    float darkness = darkness_center * (1.0f - border_factor);
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness);
                }
            }
        }

        // kada istekne => faza 3
        if(storm_wait_time <= 0.0f)
        {
            storm_phase = 3;
        }
    }
    break;

    // FAZA 3 pomracenje do dijagonale
    case 3:
    {
        float base_fade_width = 20.0f;
        float fade_scale_factor = 0.2f;
        float darkness_center = 0.5f;

        // fade_width raste s radijusom
        float fade_width = base_fade_width + fade_scale_factor * storm_radius;

        storm_radius += storm_speed_fast * delta_time;
        if(storm_radius >= diag)//presao dijagonalu
        {
            storm_radius = diag;
            storm_phase = 4;

            // mozes reset parametara munje
            lightning_active = 0;
            lightning_cooldown = 1.0f + randf() * 2.0f;
            return;
        }

        for(int y = 0; y < raster_height; y++)
        {
            for(int x = 0; x < raster_width; x++)
            {
                float dist = rafgl_distance2D(x, y, well_pos_x+20, well_pos_y+20);

                if(dist < storm_radius - fade_width)
                {
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness_center);
                }
                else if(dist < storm_radius)
                {
                    float border_factor = (dist - (storm_radius - fade_width)) / fade_width;
                    float darkness = darkness_center * (1.0f - border_factor);
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black = { .rgba = rafgl_RGB(0,0,0) };
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness);
                }
            }
        }
    }
    break;

    case 4:
    {
        //default 80%
        float base_darkness = 0.8f;
        float darkness = base_darkness;

        if (!lightning_active) {
            if (lightning_cooldown > 0.0f)
            {
                lightning_cooldown -= delta_time;
            }
            else
            {
                lightning_active = 1;
                lightning_timer = 0.0f;

                lightning_cooldown = 1.0f + randf() * 3.0f;
                lightning_x = rand() % raster_width;
                lightning_y_end = (raster_height / 2) + rand() % (raster_height / 2);

                lightning_flicker_count = 2;
                lightning_flicker_state = 0;
                lightning_flicker_timer = lightning_flicker_duration;
            }
        }

        // Ako munja trenutno traje, obrađujemo flicker
        if (lightning_active) {
            lightning_timer += delta_time;

            if (lightning_flicker_count > 0)
            {
                lightning_flicker_timer -= delta_time;
                if (lightning_flicker_timer <= 0.0f)
                {
                    lightning_flicker_count--;
                    lightning_flicker_state ^= 1; // flipuj
                    lightning_flicker_timer = lightning_flicker_duration;
                }

                //state 0 - manja, 1 - veca

                float flicker_brightness_small = 0.05f;
                float flicker_brightness_big = 0.1f;
                float current_flicker_brightness = (lightning_flicker_state == 0) ? flicker_brightness_small : flicker_brightness_big;

                darkness = base_darkness - current_flicker_brightness;
                if (darkness < 0.0f) darkness = 0.0f;

                for (int y = 0; y < raster_height; y++) {
                    for (int x = 0; x < raster_width; x++) {
                        rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                        rafgl_pixel_rgb_t black;
                        black.rgba = rafgl_RGB(0, 0, 0);
                        pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, darkness);
                    }
                }
                rafgl_pixel_rgb_t lightning_color = (lightning_flicker_state == 0)
                    ? (rafgl_pixel_rgb_t){ .rgba = rafgl_RGB(180, 220, 255) }
                    : (rafgl_pixel_rgb_t){ .rgba = rafgl_RGB(255, 250, 230) };

                draw_lightning_effect(&raster, lightning_x, -500, lightning_x, lightning_y_end);
                draw_spawn_circles(&raster);
                return;
            }
             else {
                //pocni novu
                if (lightning_timer > (2 * lightning_flicker_duration)) {
                    lightning_active = 0;
                    lightning_timer = 0.0f;
                }
                for (int y = 0; y < raster_height; y++) {
                for (int x = 0; x < raster_width; x++) {
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black;
                    black.rgba = rafgl_RGB(0, 0, 0);
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, base_darkness);
                }
            }
            }
        } else {
            //nista se ne desava, samo brightness
            for (int y = 0; y < raster_height; y++) {
                for (int x = 0; x < raster_width; x++) {
                    rafgl_pixel_rgb_t orig = pixel_at_pm(rast, x, y);
                    rafgl_pixel_rgb_t black;
                    black.rgba = rafgl_RGB(0, 0, 0);
                    pixel_at_pm(rast, x, y) = rafgl_lerppix(orig, black, base_darkness);
                }
            }
        }

        // SPAWN ANIMATION
        draw_spawn_circles(&raster);
    }
    break;

    }
}

void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // mozes setovati boje munje (ako koristis globalne)
    lightning_color1.rgba = rafgl_RGB(180, 220, 255);
    lightning_color2.rgba = rafgl_RGB(255, 250, 230);

    rafgl_raster_load_from_image(&doge, "res/images/doge.png");
    rafgl_raster_load_from_image(&checker, "res/images/checker32.png");

    raster_width = width;
    raster_height = height;

    rafgl_raster_init(&upscaled_doge, raster_width, raster_height);
    rafgl_raster_bilinear_upsample(&upscaled_doge, &doge);

    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);

    char tile_path[256];
    for(int i = 0; i < NUMBER_OF_TILES; i++)
    {
        sprintf(tile_path, "res/tiles/svgset%d.png", i);
        rafgl_raster_load_from_image(&tiles[i], tile_path);
    }

    init_tilemap();

    rafgl_spritesheet_init(&hero, "res/customs/sheepRK.png", 4, 5);
    rafgl_spritesheet_init(&hero_i, "res/customs/sheepRKInvert.png", 4, 5);
    rafgl_spritesheet_init(&well, "res/customs/wellK.png", 1,1);

    rafgl_spritesheet_init(&sheepShaven, "res/customs/sheepshearedRK.png", 4, 5);
    rafgl_spritesheet_init(&sheepShaven_i, "res/customs/sheepshearedRKInvert.png", 4, 5);

    rafgl_texture_init(&texture);

    for(int i=0; i<MAX_SHEEP; i++)
    {
        sheep_array[i].x = -100;
    }
}


int pressed;
float location = 0;
float selector = 0;

int animation_running = 0;
int animation_frame = 0;
int direction = 0;

int hero_pos_x = RASTER_WIDTH / 2;
int hero_pos_y = RASTER_HEIGHT / 2;
int hero_speed = 150;
int hover_frames = 0;
int sheepIdle = 1;
int shaven = 0;
int sheep_shaven = 0;
int hostage = 0;

int lmbCount = 0;


int krugZaTesting = 0;
int vremeTesting = 0;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    render_tilemap(&raster);

    // SISANJE HEROJA (opciono)
    if(game_data->is_rmb_down)
    {
        if(rafgl_distance2D(game_data->mouse_pos_x, game_data->mouse_pos_y, hero_pos_x+32, hero_pos_y+40)<=30)
        {
            if(sheep_shaven== 0)
            {
                shaven = 1;
                particleFunc(hero_pos_x+32, hero_pos_y+40);
                sheep_shaven = 600;
            }
        }

        // SISANJE OVACA
        for(int i=0; i<MAX_SHEEP; i++)
        {
            if(sheep_array[i].x < 0) continue;
            sheep_t *shp = &sheep_array[i];
            float dx = (shp->x + 32) - game_data->mouse_pos_x;
            float dy = (shp->y + 40) - game_data->mouse_pos_y;
            if(dx*dx + dy*dy <= (30*30))
            {
                if(!shp->shaven)
                {
                    shp->shaven = 1;
                    if(shp->dirty)
                    {
                        shp->dirty = 0;
                        dirtyParticleFunc(shp->x+32, shp->y+40);
                    }
                    else
                    {
                        particleFunc(shp->x+32, shp->y+40);
                    }
                }
            }
        }

        // Ako nema vise prljavih, gasimo oluju
        if(storm_active && !any_dirty_sheep())
        {
            storm_active = 0;
            storm_phase = 0;
            storm_radius = 0.0f;
            // mozes reset i munja parametre
            lightning_active = 0;
            lightning_timer = 0.0f;
            lightning_cooldown = 0.0f;
            circles_spawn_angle = 0;
            circles_spawn_order = 0;
        }
    }

    update_particles();
    draw_particles(&raster);

    if(sheep_shaven > 0)
    {
        sheep_shaven--;
    }
    else
    {
        shaven = 0;
    }

    // DUPLICATE SHEEP
    if(game_data->keys_pressed[RAFGL_KEY_Q])
    {
        spawn_sheep();
    }

    update_sheep(delta_time);

    // Hero pokreti
    animation_running = 1;
    if(game_data->keys_down[RAFGL_KEY_W])
    {
        hero_pos_y -= hero_speed * delta_time;
        direction = 2;
    }
    else if(game_data->keys_down[RAFGL_KEY_S])
    {
        hero_pos_y += hero_speed * 2 * delta_time;
        direction = 0;
    }
    else if(game_data->keys_down[RAFGL_KEY_A])
    {
        hero_pos_x -= hero_speed * delta_time;
        direction = 11;
    }
    else if(game_data->keys_down[RAFGL_KEY_D])
    {
        hero_pos_x += hero_speed * 2 * delta_time;
        direction = 1;
    }
    else
    {
        animation_running = 0;
    }

    if(animation_running ==1 && hover_frames >5)
        hover_frames = 0;

    if(animation_running)
    {
        if(hover_frames == 0)
        {
            animation_frame = (animation_frame + 1) % 4;
            hover_frames = 5;
        }
        else
        {
            hover_frames--;
        }
    }
    else
    {
        direction = 4;
        if(hover_frames == 0)
        {
            animation_frame += sheepIdle;
            if(animation_frame >= 3 || animation_frame <= 0)
                sheepIdle *= -1;
            animation_frame %= 4;
            if(animation_frame == 3 || animation_frame == 0)
                hover_frames = 150;
            else
                hover_frames = 60;
        }
        else
        {
            hover_frames --;
        }
    }



    if(game_data->is_lmb_down)
    {
        if(rafgl_distance2D(game_data->mouse_pos_x, game_data->mouse_pos_y,hero_pos_x+32,hero_pos_y+40)<=40)
        {
            hero_pos_x = game_data->mouse_pos_x-32;
            hero_pos_y = game_data->mouse_pos_y-10;
            hostage = 1;
            animation_frame = 1;
        }
    }
    else
    {
        hostage = 0;
    }

    draw_sheep(&raster);

    if(game_data->is_lmb_down && lmbCount <=0)
    {
        if(rafgl_distance2D(game_data->mouse_pos_x, game_data->mouse_pos_y,well_pos_x+20,well_pos_y+20)<=30)
        {
            spawn_sheep();
            lmbCount = 60;
        }
    }
    lmbCount--;
    if(lmbCount < 0) lmbCount = 0;



    // Snimanje
    if(game_data->keys_pressed[RAFGL_KEY_S] && game_data->keys_down[RAFGL_KEY_LEFT_SHIFT])
    {
        sprintf(save_file, "save%d.png", save_file_no++);
        rafgl_raster_save_to_png(&raster, save_file);
    }

    draw_particles(&raster);
    apply_storm_effect(&raster, delta_time);

    if(game_data->keys_down[RAFGL_KEY_T])
    {
        rafgl_raster_draw_circle(&raster,hero_pos_x+32,hero_pos_y+40,40,rafgl_RGB(254,0,251));
        rafgl_raster_draw_circle(&raster,well_pos_x+20,well_pos_y+20,30,rafgl_RGB(254,0,251));

        int br = krugZaTesting;
        krugZaTesting--;
        vremeTesting = vremeTesting <= 400 ? ++vremeTesting:vremeTesting;

        for(int i = 0; i<vremeTesting/40; i++)
        {
            float radian_br = br * (M_PI / 180.0f);
            int tx = cosf(radian_br) * 50;
            int ty = sinf(radian_br) * 50;
            br += 36;
            rafgl_raster_draw_circle(&raster,hero_pos_x+32+tx,hero_pos_y+40+ty,5,rafgl_RGB(254,0,251));
        }
        //draw_lightning_effect(&raster, storm_sheep_pos_x, storm_sheep_pos_y, hero_pos_x, hero_pos_y);


    }
    if(game_data->keys_down[RAFGL_KEY_T])
    {
        if(vremeTesting/40 >= 10)
            draw_lightning_effect(&raster, storm_sheep_pos_x+32, storm_sheep_pos_y+40, hero_pos_x+32, hero_pos_y+40);


    }
    else
        vremeTesting = 0;
    // Hero
    if(shaven == 0)
    {
        if(direction == 11)
            rafgl_raster_draw_spritesheet(&raster, &hero_i, animation_frame, 1, hero_pos_x, hero_pos_y);
        else
            rafgl_raster_draw_spritesheet(&raster, &hero, animation_frame, direction, hero_pos_x, hero_pos_y);
    }
    else
    {
        if(direction == 11)
            rafgl_raster_draw_spritesheet(&raster, &sheepShaven_i, animation_frame, 1, hero_pos_x, hero_pos_y);
        else
            rafgl_raster_draw_spritesheet(&raster, &sheepShaven, animation_frame, direction, hero_pos_x, hero_pos_y);
    }

}

void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_raster_cleanup(&raster2);
    rafgl_texture_cleanup(&texture);
}
