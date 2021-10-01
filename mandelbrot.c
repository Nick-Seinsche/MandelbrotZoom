#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define CPX_USE_LONG_DOUBLE
#include "complex.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

//#include "SDL2/SDL_thread.h"

// ---------------------------------------------------------------------------

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FULLSCREEN 0

// SETS the depth of the fractal; Change as you like.
#define MAX_ITER 90

// Sets the threshhold after which the mandelbrot iteration is considered
// bounded.
// The black region will always be a superset of the mandelbrot set.
// The are equal if MAX_ITER tends to infinity and BOUNDED_THRESHOLD = 2.
#define BOUNDED_THRESHOLD 2

// 1 - Use Conrol points
// 2 - Use continous coloring
// 3 - Black and White
#define COLORING 1

// ---------------------------------------------------------------------------

// posX, posY center of the viewport, zoom is the zoom level
// the camera is used to update the viewport
struct Camera{
    long double posX, posY;
    long double zoom;
};

typedef struct Point{
    long double x, y;
} POINT;

typedef struct PixelColor{
    uint8_t r, g, b;
} PIXEL_COLOR;


#if COLORING == 1

// control point colors
struct PixelColor color1 = {0, 7, 100};

float position1 = 0.16;
struct PixelColor color2 = {32, 107, 203};

float position2 = 0.42;
float delta2 = position2 - position1;
struct PixelColor color3 = {237, 255, 255};

float position3 = 0.6425;
float delta3 = position3 - position2;
struct PixelColor color4 = {255, 170, 0};

float position4 = 0.8575;
float delta4 = position4 - position3;
struct PixelColor color5 = {0, 2, 0};

struct PixelColor color6 = {0, 0, 0};


void calulateColorWithControlPoints(uint8_t* r, uint8_t* g,
                                    uint8_t* b, int k) {
    // low t - not in the mandelbrot set (if BOUNDED_THRESHOLD = 2)
    // high t - probably inside the mandelbrot set

    float t = (float) k / MAX_ITER;
    if (t <= position1){
        float l = t / position1;
        *r = (1 - l) * color1.r + l * color2.r;
        *g = (1 - l) * color1.g + l * color2.g;
        *b = (1 - l) * color1.b + l * color2.b;
    }else if (t <= position2) {
        float l = (t - position1) / delta2;
        *r = (1 - l) * color2.r + l * color3.r;
        *g = (1 - l) * color2.g + l * color3.g;
        *b = (1 - l) * color2.b + l * color3.b;
    }else if (t <= position3) {
        float l = (t - position2) / delta3;
        *r = (1 - l) * color3.r + l * color4.r;
        *g = (1 - l) * color3.g + l * color4.g;
        *b = (1 - l) * color3.b + l * color4.b;
    }else if (t <= position4) {
        float l = (t - position3) / delta4;
        *r = (1 - l) * color4.r + l * color5.r;
        *g = (1 - l) * color4.g + l * color5.g;
        *b = (1 - l) * color4.b + l * color5.b;
    }else {
        *r = color6.r;
        *g = color6.g;
        *b = color6.b;
    }
}

#endif


/*
    Calculates the viewport of a given Camera. Calcs two sets of points:
    [x_min, x_max] and [y_min, y_max]. Any given point inside of the intervals
    will be part of the viewport.
*/
void calculatePerspective(struct Camera* c, POINT viewport[2]){
     viewport[0].x = c -> posX - 1.0 / (exp(c -> zoom) - 1.0);
     viewport[0].y = c -> posX + 1.0 / (exp(c -> zoom) - 1.0);
     viewport[1].x = (c -> posY - 1.0 / (exp(c -> zoom) - 1.0) *
                        (long double) WINDOW_HEIGHT / WINDOW_WIDTH);
     viewport[1].y = (c -> posY + 1.0 / (exp(c -> zoom) - 1) *
                        (long double) WINDOW_HEIGHT / WINDOW_WIDTH);
}

/*
    Keyhandler:

    W - UP
    S - DOWN
    A - LEFT
    D - RIGHT
    Q - Zoom in
    R - Zoom out
*/
void handleKeys(SDL_KeyboardEvent* event, struct Camera* c){
    if (event -> type == SDL_KEYUP){
        //printf("Release\n");
    }else if (event -> type == SDL_KEYDOWN){
        //printf("Press %d\n", event -> keysym);
        switch(event -> keysym.sym){
        case SDLK_w:
            c -> posY -= 1 / exp(c -> zoom) / 6;
            break;
        case SDLK_a:
            c -> posX -= 1 / exp(c -> zoom) / 6;
            break;
        case SDLK_s:
            c -> posY += 1 / exp(c -> zoom) / 6;
            break;
        case SDLK_d:
            c -> posX += 1 / exp(c -> zoom) / 6;
            break;
        case SDLK_q:
            c -> zoom += 0.2;
            printf("zoom: %f\n", (float) c -> zoom);
            break;
        case SDLK_e:
            c -> zoom -= 0.2;
            break;
        }
    }
}


/*
    displays integer with
    k - 1000
    kk - 100k
    m - million
    b - billion
    > 2b - large number
*/
char* display(long int num){
    char* str = (char*) malloc(35 * sizeof(char));
    if (num < 0){
        strcpy(str, "large number");
        return str;
    }
    if(num / 1000000000.0 > 1){
        num = num / 1000000000;
        sprintf(str, "%f", (double) num);
        strcat(str, "b");
    }else if(num / 1000000.0 > 1){
        num = num / 1000000;
        sprintf(str, "%ld", num);
        strcat(str, "m");
    }else if (num / 1000.0 > 1){
        num = num / 1000;
        sprintf(str, "%ld", num);
        strcat(str, "k");
    }else{
        sprintf(str, "%ld", num);
    }
    return str;
}


/*
    Updates the window title
*/
void updateTitle(SDL_Window* window, struct Camera camera, float zm,
                                                        unsigned short fps){
    char buffer[90];
    sprintf(buffer,
        "Camera: [%f, %f], zoom: %s, fps: %hu, Controls: W,A,S,D,Q,E",
        (float) camera.posX, (float) camera.posY, display((long int) zm), fps);

    SDL_SetWindowTitle(window, buffer);
}


/*
    Maps the value v element of (a, b) interval
    to (c, d) interval
*/
double map(long double v, long double a, long double b, long double c,
    long double d){

    return c + (d - c) * ( v / (b - a) - a / (b - a) );
}


// typedef struct ThreadData{
//     uint8_t* ptr;
//     uint8_t r, g, b;
// } THREAD_DATA;
//
// static int write(THREAD_DATA* td){
//     *(td -> ptr) = td -> r;
//     td -> ptr ++;
//     *(td -> ptr) = td -> g;
//     td -> ptr ++;
//     *(td -> ptr) = td -> b;
//     td -> ptr ++;
//     return 0;
//
// }

void update(uint8_t* pixels, POINT viewport[2], float prime[3]){
    uint8_t r = 0, g = 0, b = 0;
    complex c, z;
    int k;
    //SDL_Thread* thread;
    //int currentIndex = 0;

    for (int j = 0 ; j < WINDOW_HEIGHT ; ++j){
        for (int i = 0 ; i < WINDOW_WIDTH; ++i){
            c = (complex) {map(i, 0, WINDOW_WIDTH, viewport[0].x, viewport[0].y),
                          map(j, 0, WINDOW_HEIGHT, viewport[1].x, viewport[1].y)};
            z = (complex) {0, 0};
            k = 0;
            while (k <= MAX_ITER){
                z = cpx_add(cpx_mul(z, z), c);
                if (z.real * z.real + z.imag * z.imag
                    > BOUNDED_THRESHOLD * BOUNDED_THRESHOLD) break;
                // if (z.real + z.imag > BOUNDED_THRESHOLD ||
                //     z.real + z.imag < -BOUNDED_THRESHOLD) break;
                k += 1;
            }

            #if COLORING == 1
            calulateColorWithControlPoints(&r, &g, &b, k);
            #elif COLORING == 2
            // calculate with prime colors
            k = (int)((float) k / MAX_ITER * 255) % 256;
            r = (int)(cos(k) * k + (long double) prime[0] * k) % 255;
            g = (int)(sin(k * k) * k + (long double) prime[1] * k) % 255;
            b = (int)(k * (long double) prime[2] + sin(k) * k / MAX_ITER) % 255;
            #elif COLORING == 3
            //black and white
            k = (int)((float) k / MAX_ITER * 255) % 256;
            r = (uint8_t) k;
            g = (uint8_t) k;
            b = (uint8_t) k;
            #endif

            // fancy coloring
            // k = (int)((float) k / MAX_ITER * 255) % 256;
            // r = (uint8_t)(k - k * sin(log(z.real / z.imag))) % 256;
            // g = (uint8_t)(k - k * cos(sqrt(z.real / z.imag))) % 256;
            // b = (uint8_t)(k - k * sin(log(z.real / z.imag))) % 256;

            *pixels++ = r;
            *pixels++ = g;
            *pixels++ = b;
        }
    }
}


int main(){
    // Declaring the window
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    // Initing SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    // https://wiki.libsdl.org/SDL_CreateWindow
    const char* title = "Title";

    // SDL_WINDOW_FULLSCREEN_DESKTOP
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                    #if FULLSCREEN == 1
                    SDL_WINDOW_FULLSCREEN_DESKTOP
                    #else
                    0
                    #endif
                );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0,
    //                            &window, &renderer);

    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    struct Camera camera = {0, 0, 0.33};
    POINT viewport[2];
    calculatePerspective(&camera, viewport);

    srand(time(0));
    float colors[3];
    colors[0] = (float)(rand() % 300) / 100.0 + 0.85;
    colors[1] = (float)(rand() % 300) / 100.0 + 0.85;
    colors[2] = (float)(rand() % 300) / 100.0 + 0.85;

    /*
        r=3.410000, g=1.140000, b=2.420000
        r=2.690000, g=3.480000, b=1.020000

        0.8 - 2.3
        1.3 - 1.7
        1.4 - 2.5
    */
    printf("r=%f, g=%f, b=%f\n", colors[0], colors[1], colors[2]);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                    SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    uint8_t *pixels = (uint8_t*) malloc(WINDOW_WIDTH * WINDOW_HEIGHT
                                                * sizeof(uint8_t) * 3);
    int pitch = sizeof(uint8_t) * 3 * WINDOW_WIDTH;

    //SDL_RenderClear(renderer);
    //update2(renderer, points[0], points[1], var1, var2, var3);
    //SDL_RenderPresent(renderer);

    float defaultZoom = viewport[0].y - viewport[0].x;
    float zoomLevel = defaultZoom;
    float zoomedIn = 1.0;

    // clock_t start, stop, delta = 0;
    // unsigned short frames = 0;
    // unsigned short fps = 0;

    // start = clock();
    // update3(pixels, points, vars);
    // stop = clock();
    // delta += stop - start;
    // printf("delta: %f\n", (double) delta);
    //
    // start = clock();
    // update2(renderer, points[0], points[1], vars[0], vars[1], vars[2]);
    // stop = clock();
    // delta += stop - start;
    // printf("delta: %f\n", (double) delta);
    //
    // return 0;

    updateTitle(window, camera, zoomLevel, 0);

    // Window loop
    bool running = true;

    while(running){

        //start = clock();

        while(SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN){
                handleKeys(&event.key, &camera);
                calculatePerspective(&camera, viewport);
                zoomLevel = viewport[0].y - viewport[0].x;
                zoomedIn = defaultZoom / zoomLevel;
            }else if(event.type == SDL_QUIT){
                running = false;
            }
        }
        update(pixels, viewport, colors);
        SDL_UpdateTexture(texture, NULL, pixels, pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // stop = clock();
        // delta += stop - start;
        // frames ++;
        // if ((delta / (double) CLOCKS_PER_SEC) > 1){
        //     printf("[INFO] FPS: %d\n", frames);
        //     fps = frames;
        //     frames = delta = 0;
        // }
        updateTitle(window, camera, zoomedIn, 0);
    }

    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
