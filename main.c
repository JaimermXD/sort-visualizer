/*** INCLUDES ***/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"

/*** DEFINES ***/

#define ARR_LEN 200
#define WIDTH 200 // Must be equal to ARR_LEN
#define HEIGHT 150
#define SCALE 5
#define DELAY 0

/*** UTILS ***/

/**
 * Generate a random number in the range [0, n), with n <= RAND_MAX.
*/
int randint(int n) {
    if ((n - 1) == RAND_MAX) return rand();

    int end = RAND_MAX / n;
    end *= n;

    int r;
    while ((r = rand()) >= end);
    return r % n;
}

/**
 * Print array in the form [1, 2, 3, 4, ...].
*/
void print_arr(int *arr) {
    printf("[");
    for (int i = 0; i < ARR_LEN; i++) {
        if (i == ARR_LEN - 1) printf("%d]\n", arr[i]);
        else printf("%d, ", arr[i]);
    }
}

/**
 * Swap arr[i] and arr[j].
*/
void swap(int *arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

/*** SDL DISPLAY ***/

/**
 * Draw current state of the sorting process.
*/
void draw_state(int *arr, SDL_Renderer *renderer, int green, int red) {
    for (int i = 0; i < ARR_LEN; i++) {
        if (i == green) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else if (i == red) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderDrawLine(renderer, i, HEIGHT - 1, i, HEIGHT - arr[i]);
    }
}

/**
 * Clear screen and draw current state.
*/
void update_screen(int *arr, SDL_Renderer *renderer, int i, int j) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_state(arr, renderer, i, j);

    SDL_RenderPresent(renderer);
    SDL_Delay(DELAY);
}

/*** SORTING ALGORITHMS ***/

/**
 * Bubble sort algorithm.
*/
void bubble_sort(int *arr, SDL_Renderer *renderer) {
    for (int i = 0; i < ARR_LEN; i++) {
        for (int j = i; j < ARR_LEN; j++) {
            if (arr[i] > arr[j]) swap(arr, i, j);
            update_screen(arr, renderer, i, j);
        }
    }
}

/**
 * Quick sort helper function.
 * Partitions array in two sub-arrays determined by the pivot.
*/
int partition(int *arr, int start, int end, SDL_Renderer *renderer) {
    int pivot = arr[end];
    int i = start - 1;

    for (int j = start; j <= end - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr, i, j);
        }

        update_screen(arr, renderer, i, j);
    }

    i++;
    swap(arr, i, end);

    return i;
}

/**
 * Quick sort algorithm.
*/
void quick_sort(int *arr, int start, int end, SDL_Renderer *renderer) {
    if (end <= start) return;

    int pivot = partition(arr, start, end, renderer);
    quick_sort(arr, start, pivot - 1, renderer);
    quick_sort(arr, pivot + 1, end, renderer);
}

/*** MAIN ***/

/**
 * Da main loop.
 * Initializes the array along with SDL and its components. It also executes the sorting
 * algorithm, listens for SDL events and performs the final cleanup.
*/
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    srand(time(NULL));

    // Numbers in range [1, HEIGHT - 1]
    int arr[ARR_LEN];
    for (int i = 0; i < ARR_LEN; i++) {
        arr[i] = randint(HEIGHT - 1) + 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL subsystems: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Sort Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);
    if (!window) {
        SDL_Log("Unable to create window: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: '%s'\n", SDL_GetError());
        return 1;
    }

    if (SDL_RenderSetScale(renderer, SCALE, SCALE) != 0) {
        SDL_Log("Unable to set render scale: '%s'\n", SDL_GetError());
        return 1;
    }

    clock_t begin = clock();

    // bubble_sort(arr, renderer);
    quick_sort(arr, 0, ARR_LEN - 1, renderer);

    clock_t end = clock();

    print_arr(arr);
    printf("Elapsed time: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                
                default:
                    break;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}