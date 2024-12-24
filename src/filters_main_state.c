#include <main_state.h>
#include <glad/glad.h>
#include <math.h>

#include <rafgl.h>

static int w, h;
static rafgl_raster_t raster, doge;
static rafgl_texture_t tex;
int showDoge = 1;
void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    w = width;
    h = height;
    rafgl_raster_init(&raster, w, h);
    rafgl_raster_load_from_image(&doge, "res/doge.png");
    rafgl_texture_init(&tex);

}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    int x,y;
    ///     VEZBE 2:
    /// 1 & 2
    /*for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            pixel_at_m(raster,x,y) = pixel_at_m(doge,w-x,y);// PRIMER 1
            //pixel_at_m(raster,x,y) = pixel_at_m(doge,y,w-x);// PRIMER 2

        }

    }*/
    /// 3 NEGATIVE
    /*for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            int r = 255-sample.r;
            int g = 255-sample.g;
            int b = 255-sample.b;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);// PRIMER 1
            //pixel_at_m(raster,x,y) = pixel_at_m(doge,y,w-x);// PRIMER 2

        }

    }*/
    /// 4 GRAYSCALE
    /*for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            int brightness = rafgl_calculate_pixel_brightness(sample);
            int b = sample.r*0.299+ sample.g*0.584+ sample.b*0.144;
            //pixel_at_m(raster,x,y).rgba = rafgl_RGB(b,b,b);
            //pixel_at_m(raster,x,y).rgba = rafgl_RGB(brightness,brightness,brightness);

        }

    }*/
    /// 5 BINARY
    /*float cutoff_ = 1.0f*game_data->mouse_pos_y/h;
    int cutoff = cutoff_*256;
    for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            //int brightness = rafgl_calculate_pixel_brightness(sample);
            int b = sample.r*0.299+ sample.g*0.584+ sample.b*0.144;
            b = b<cutoff? 0: 255;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(b,b,b);

        }

    }*/
    /// 6 manje boja
    /*int cutoff = 256*(game_data->mouse_pos_y/h);
    //int cutoff = cutoff_*256;
    for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            int r = sample.r/cutoff;
            r *= cutoff;
            int g = sample.g/cutoff;
            g *= cutoff;
            int b = sample.b/cutoff;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);

        }

    }*/
    /// 7 Brightness
    int brightness = 256*(game_data->mouse_pos_y/h);
    //int cutoff = cutoff_*256;
    for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            int r = rafgl_saturatei(brightness+sample.r);
            int g = rafgl_saturatei(brightness+sample.g);
            int b = rafgl_saturatei(brightness+sample.b);
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);

        }

    }
    /// 8 CONTRAST
    /*int contrast = 127;// 256*(game_data->mouse_pos_y/h);
    //int cutoff = cutoff_*256;
    for(y = 0; y<h; y++)
    {
        for(x = 0; x<w; x++)
        {
            //if(rafgl_distance2D(x,y,w/2,h/2)<100)
            rafgl_pixel_rgb_t sample = pixel_at_m(doge,x,y);
            int r = rafgl_saturatei((sample.r-contrast)*1.5f + contrast);
            int g = rafgl_saturatei((sample.g-contrast)*1.5f + contrast);
            int b = rafgl_saturatei((sample.b-contrast)*1.5f + contrast);
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);


        }

    }*/
    /// 9 EDGING
    /*rafgl_pixel_rgb_t origin,sampled,result;
    int i,j, b1, b2, sum;
    for(y = 0; y < h; y++){
        for(x = 0; x < w; x++){
            if(y==0 || y==h-1 || x==0 || x==w-1){
                continue;
            }
            sum=0;
            origin = pixel_at_m(doge,x,y);
            b1 = rafgl_calculate_pixel_brightness(origin);
            for(i=-1; i<2; i++){
                for(j=-1; j<2; j++){
                        if(i == 0 && j == 0)
                            continue;
                    sampled = pixel_at_m(doge,x+i,y+j);
                    b2 = rafgl_calculate_pixel_brightness(sampled);
                    sum += abs(b1-b2);
                }
            }
            int edge = sum/8*2;
            result.r = rafgl_saturatei(edge);
            result.g = rafgl_saturatei(edge);
            result.b = rafgl_saturatei(edge);

            pixel_at_m(raster,x,y) = result;
        }
    }*/
    ///10 SHARPEN
    //Kernel matrica se napravi i racunamo result.r = origin.r*kernel[i][j]*kernCoef

    /// VIGNETE
    /*int r = 512;
    int cx = game_data->mouse_pos_x;
    int cy = game_data->mouse_pos_y;
    rafgl_pixel_rgb_t sample;
    rafgl_pixel_rgb_t result;
    float vf = 1.5f;
    for(y = 0; y < h; y++){
        for(x = 0; x < w; x++){
            sample = pixel_at_m(doge,x,y);
            float dist = rafgl_distance2D(x,y,cx,cy)/r;
            //dist = powf(dist, 1.8f);
            dist = sqrt(dist);
            result.r = rafgl_saturatei(sample.r*(1.0f -dist*vf));
            result.g = rafgl_saturatei(sample.g*(1.0f - dist*vf));
            result.b = rafgl_saturatei(sample.b*(1.0f - dist*vf));
            pixel_at_m(raster,x,y) = result;
        }
    }*/



    /// Space isto za sve vezbe:
    if(game_data->keys_down[RAFGL_KEY_SPACE])
        showDoge=1;
    else
        showDoge=0;

}

void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&tex, showDoge? &doge: &raster);
    rafgl_texture_show(&tex, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args)
{

}
