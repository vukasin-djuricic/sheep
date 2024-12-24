#include <main_state.h>
#include <glad/glad.h>
#include <math.h>

#include <rafgl.h>

static int w, h;
static rafgl_raster_t raster;
static rafgl_texture_t tex;

void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    w = width;
    h = height;
    rafgl_raster_init(&raster, w, h);
    rafgl_texture_init(&tex);

}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    int x,y;
    {///VEZBE 1 (EXPAND)
    ///REGION

    ///1 FULL BACKGROUND
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            pixel_at_m(raster, x, y).rgba = rafgl_RGB(255, 0,0);
        }
    }*/
    ///2 PRAVOUGAONIK
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            if(x>w/2-100 && y > h/2-150 & x<w/2+100 & y<h/2+150)
                pixel_at_m(raster, x, y).rgba = rafgl_RGB(255,0,0);
            else
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(0,0,255);
        }
    }*/
    ///3 KRUG ISCRTAJ
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            if(rafgl_distance2D(x,y,w/2,h/2)<100)
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(255,0,0);
            else
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(0,0,255);
        }
    }*/
    ///4 interpolacija levo-desno (256x256)
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            int xn = x/w;
            int yn = y/h;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(x,0,0);
        }
    }*/
    ///5 interpolacija za x,y (256x256)
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            int xn = x/w;
            int yn = y/h;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(x,y,0);
        }
    }*/
    ///6 linearna interpolacija (!= 256)
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            float xn = x*1.0/w;
            float yn = y*1.0/h;
            int r = 255*xn;
            int g = 255*yn;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,0);
        }
    }*/
    ///7 inteprolacija izmedju LC i RC
    /*rafgl_pixel_rgb_t LC, RC;
    LC.rgba = rafgl_RGB(255,130,30);
    RC.rgba = rafgl_RGB(0,200,255);
    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            float xn = x*1.0/w;
            float yn = y*1.0/h;
            int r = (1-xn)*LC.r+ xn*RC.r;
            int g = (1-xn)*LC.g+ xn*RC.g;
            int b = (1-xn)*LC.b+ xn*RC.b;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);
        }
    }*/
    ///8 dole DESNO BELA BOJA
    /*rafgl_pixel_rgb_t LC, RC;
    LC.rgba = rafgl_RGB(255,130,30);
    RC.rgba = rafgl_RGB(0,200,255);
    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            float xn = x*1.0/w;
            float yn = y*1.0/h;
            int r = 255*(1-xn)*(1-yn) + 255*xn*yn;
            int g = 255*xn*(1-yn) + 255*xn*yn;
            int b = 255*(1-xn)*yn + 255*xn*yn;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,g,b);
        }
    }*/
    ///9 lerppix u krugu
    /*rafgl_pixel_rgb_t Krug, Background;
    Krug.rgba = rafgl_RGB(255,0,0);
    Background.rgba = rafgl_RGB(0,0,255);
    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            if(rafgl_distance2D(x,y,w/2,h/2)<100)
                pixel_at_m(raster,x,y) = rafgl_lerppix(Background,Krug,rafgl_distance2D(x,y,w/2,h/2)/100);
            else
                pixel_at_m(raster,x,y) = Krug;
        }
    }*/
    ///10 krug
    /*rafgl_pixel_rgb_t Bela, Crna;
    Bela.rgba = rafgl_RGB(255,255,255);
    Crna.rgba = rafgl_RGB(0,0,0);

    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            if(rafgl_distance2D(x,y,w/2,h/2)<100)
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(x*3,x*3,x*3);
                //pixel_at_m(raster,x,y) = rafgl_lerppix(Crna,Bela,1.0f*(x-(w/2-100))/200);
        }
    }*/
    ///11 krug nakoso
    /*rafgl_pixel_rgb_t Bela, Crna;
    Bela.rgba = rafgl_RGB(255,255,255);
    Crna.rgba = rafgl_RGB(0,0,0);

    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            if(rafgl_distance2D(x,y,w/2,h/2)<100)
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(x*3+y*3,x*3+y*3,x*3+y*3);
                //pixel_at_m(raster,x,y) = rafgl_lerppix(Crna,Bela,1.0f*(x-(w/2-100))/200);
        }
    }*/
    ///12 no signal tv
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
                pixel_at_m(raster,x,y).rgba = rafgl_RGB(rand(),rand(),rand());
        }
    }*/
    ///13 no signal TV GRAYSCALE
    /*for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            int r = rand()%256;
            pixel_at_m(raster,x,y).rgba = rafgl_RGB(r,r,r);
        }
    }*/
    ///14 sahovska tabla
    /*rafgl_pixel_rgb_t Bela, Crna;
    Bela.rgba = rafgl_RGB(255,255,255);
    Crna.rgba = rafgl_RGB(0,0,0);
    int velicinaKocke = w/8;
    for(y=0; y<h; y++){
        for(x=0;x<w;x++){
            int tmp = x/velicinaKocke+y/velicinaKocke;
            if(tmp%2==0)
                pixel_at_m(raster,x,y) = Bela;
            else
                pixel_at_m(raster,x,y) = Crna;
        }
    }*/
    ///15 sahovska tabla (lerpPix)
    /*int x, y;
    int stranicax=50, stranicay=50;

    int dark, bright;

    for(y = 0; y < h; y++){
        for(x = 0; x < w; x++){
//            slicna logika kao u proslom primeru kombinovana sa sencenjem kao iz primera11
//            samo sto cemo ovde za sencenje koristiti koordinate unutar samog kvadratica

//            modovanjem x i y vrednosti dobijamo lokane koordinate u okviru kvadrata
//            tako da kako rastu x i y u okviru kvadrata tako se slika posvetljava.
            dark=25+(x%stranicax) + (y%stranicay);
            bright = 100 +(x%stranicax) + (y%stranicay);
            if((x / stranicax + y / stranicay)  % 2 == 0){
//            takodje ovde necemo ici od skroz bele i crne boje
//            vec od svetlije sive ka jos svetlijoj i od tamne ka nesto svetlije tamnoj
//            odnosno upotrebicemo dark i bright koje smo izracunali na osnovu lokalne pozicije u kvadratu
                pixel_at_m(raster, x, y).rgba = rafgl_RGB(bright, bright, bright);
            }
            else{
                pixel_at_m(raster, x, y).rgba = rafgl_RGB(dark, dark, dark);
            }
        }
    }*/
    ///16 COSINUS
    int x, y,sx,sy,dist;
    int cent = h/2;
    for(y = 0; y < h; y++){
        for(x = 0; x < w; x++){
            sx = x;
            sy = cent + sin(x*0.02)*h/4;
            dist = rafgl_distance2D(sx,sy, x,y);

            if(dist<h/4){
                    //sinus color ce nam biti potreban za lerp, kao i bc boju za background
                    rafgl_pixel_rgb_t scol, bg;
                    scol.rgba = rafgl_RGB(255,255,255);
                    bg.rgba = rafgl_RGB(0,0,0);
//                    lerpujemo od bele boje scol ka crnoj boji bg po skali  1.0f * dist/(h/4)
                    pixel_at_m(raster,x,y)= rafgl_lerppix(scol, bg, 1.0f * dist/(h/4));
//                    pixel_at_m(raster,x,y)= rafgl_lerppix(scol, bg, 1.0f * dist);
                }
                else{
                    pixel_at_m(raster,x,y).rgba = rafgl_RGB(0,0,0);
                }
        }
    }



    ///ENDREGION
    }

}

void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&tex, &raster);
    rafgl_texture_show(&tex, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args)
{

}
