#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define CPX_USE_LONG_DOUBLE
#include "complex.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 400

// SETS the depth of the fractal; Change as you like.
#define MAX_ITER 45


struct Camera{
    long double posX, posY;
    long double zoom;
};


typedef struct Point{
    long double x, y;
} POINT;

/*
    Calculates the viewport of a given Camera. Returns two sets of points:
    [x_min, x_max] and [y_min, y_max]. Any given point inside of the intervals
    will be part of the viewport.
*/
void calculatePerspective(struct Camera* c, POINT points[2]){
     points[0].x = c -> posX - 1.0 / (exp(c -> zoom) - 1.0);
     points[0].y = c -> posX + 1.0 / (exp(c -> zoom) - 1.0);
     points[1].x = (c -> posY - 1.0 / (exp(c -> zoom) - 1.0) *
                    (long double) WINDOW_HEIGHT / WINDOW_WIDTH);
     points[1].y = (c -> posY + 1.0 / (exp(c -> zoom) - 1) *
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
            break;
        case SDLK_e:
            c -> zoom -= 0.2;
            break;
        }
    }
}

/*
    Updates the window title
*/
void updateTitle(SDL_Window* window, struct Camera camera, float zm,
                                                        unsigned short fps){
    char buffer[90];
    sprintf(buffer,
        "Camera: [%f, %f], zoom: %f, fps: %hu, Controls: W,A,S,D,Q,E",
        (float) camera.posX, (float) camera.posY, zm, fps);

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


/*
    Calculate the pixels for the mandelbrot set
*/
void update2(SDL_Renderer* renderer, POINT real, POINT imag,
                                        float var1, float var2, float var3){
    double r, g, b;
    complex c, z;
    int k;
    //#pragma omp parallel for
    for (int i = 0 ; i < WINDOW_WIDTH ; ++i){
        for (int j = 0 ; j < WINDOW_HEIGHT ; ++j){
            //complex c = {(double) i / exp(zoom) - x_offset, (double) j / exp(zoom) - y_offset};
            c = (complex) {map(i, 0, WINDOW_WIDTH, real.x, real.y),
                          map(j, 0, WINDOW_HEIGHT, imag.x, imag.y)};
            z = (complex) {0, 0};
            k = 0;
            while (k <= MAX_ITER){
                z = cpx_add(cpx_mul(z, z), c);
                if (z.real + z.imag > 17 || z.real + z.imag < -17) break;
                k += 1;
            }
            k = (int)((double) k / MAX_ITER * 255) % 255;
            r = (int)(cos(k) * k + (long double) var1 * k) % 255;
            g = (int)(sin(k * k) * k + (long double) var2 * k) % 255;
            b = (int)(k * (long double) var3 + sin(k) * k / MAX_ITER) % 255;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, i, j);
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

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                &window, &renderer);

    struct Camera camera = {0.0, 0.0, 0.33};
    POINT points[2];
    calculatePerspective(&camera, points);
    srand(time(0));
    float var1 = (float)(rand() % 250) / 100.0 + 0.5;
    float var2 = (float)(rand() % 300) / 100.0 + 0.1 + var1 / 5.0;
    float var3 = (float)(rand() % 200) / 100.0 + 0.2;

    printf("%f, %f, %f\n", var1, var2, var3);

    SDL_RenderClear(renderer);
    update2(renderer, points[0], points[1], var1, var2, var3);
    SDL_RenderPresent(renderer);

    // Window loop
    bool running = true;

    float defaultZoom = points[0].y - points[0].x;
    float zoomLevel = defaultZoom;
    float zm = 1.0;

    clock_t start, stop, delta = 0;
    unsigned short frames = 0;
    unsigned short fps = 0;

    updateTitle(window, camera, zm, frames);

    while(running){

        start = clock();

        while(SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN){
                handleKeys(&event.key, &camera);
                calculatePerspective(&camera, points);
                zoomLevel = points[0].y - points[0].x;
                zm = defaultZoom / zoomLevel;
            }else if(event.type == SDL_QUIT){
                running = false;
            }
        }
        //#pragma omp parallel
        //{
            SDL_RenderClear(renderer);
            update2(renderer, points[0], points[1], var1, var2, var3);
            SDL_RenderPresent(renderer);
        //}
        stop = clock();
        delta += stop - start;
        frames ++;
        if ((delta / (double) CLOCKS_PER_SEC) > 1){
            printf("[INFO] FPS: %d\n", frames);
            fps = frames;
            frames = delta = 0;
        }
        updateTitle(window, camera, zm, fps);
    }
    return 0;
}
