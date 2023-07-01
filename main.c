/*** INCLUDES ***/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"

/*** DATA ***/

enum algorithm {
    SELECTION_SORT,
    INSERTION_SORT,
    BUBBLE_SORT,
    MERGE_SORT,
    QUICK_SORT,
    SHELL_SORT
};

struct config {
    int width;
    int height;
    float scale;
    int delay;
    enum algorithm algorithm;
};

struct config C;

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
    for (int i = 0; i < C.width; i++) {
        if (i == C.width - 1) printf("%d]\n", arr[i]);
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
    for (int i = 0; i < C.width; i++) {
        if (i == green) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else if (i == red) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderDrawLine(renderer, i, C.height - 1, i, C.height - arr[i]);
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
    SDL_Delay(C.delay);
}

/*** SORTING ALGORITHMS ***/

/**
 * Selection sort algorithm.
*/
void selection_sort(int *arr, SDL_Renderer *renderer) {
    for (int i = 0; i < C.width - 1; i++) {
        int min = i;
        for (int j = i + 1; j < C.width; j++) {
            if (arr[j] < arr[min]) min = j;
            update_screen(arr, renderer, i, j);
        }

        if (min != i) swap(arr, i, min);
    }
}

/**
 * Insertion sort algorithm.
*/
void insertion_sort(int *arr, SDL_Renderer *renderer) {
    for (int i = 1; i < C.width; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;

            update_screen(arr, renderer, i, j);
        }

        arr[j + 1] = key;
    }
}

/**
 * Bubble sort algorithm.
*/
void bubble_sort(int *arr, SDL_Renderer *renderer) {
    for (int i = 0; i < C.width; i++) {
        for (int j = 0; j < C.width - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) swap(arr, j, j + 1);    
            update_screen(arr, renderer, j, j + 1);
        }
    }
}

/**
 * Merge sort helper function.
 * Combines sorted sub-arrays.
*/
void merge_sorted(int *arr, int l, int m, int r, SDL_Renderer *renderer) {
    int left_len = m - l + 1;
    int right_len = r - m;

    int tmp_left[left_len];
    int tmp_right[right_len];

    for (int i = 0; i < left_len; i++) {
        tmp_left[i] = arr[l + i];
    }

    for (int i = 0; i < right_len; i++) {
        tmp_right[i] = arr[m + 1 + i];
    }

    int i, j, k;
    for (i = 0, j = 0, k = l; k <= r; k++) {
        if (i < left_len && (j >= right_len || tmp_left[i] <= tmp_right[j])) {
            arr[k] = tmp_left[i];
            i++;
        } else {
            arr[k] = tmp_right[j];
            j++;
        }

        update_screen(arr, renderer, r, k);
    }
}

/**
 * Merge sort helper function.
 * Partitions the array into smaller sub-arrays recursively.
*/
void merge_sort_recursive(int *arr, int l, int r, SDL_Renderer *renderer) {
    if (l >= r) return;

    int m = l + (r - l) / 2;

    merge_sort_recursive(arr, l, m, renderer);
    merge_sort_recursive(arr, m + 1, r, renderer);

    merge_sorted(arr, l, m, r, renderer);
}

/**
 * Merge sort algorithm.
*/
void merge_sort(int *arr, SDL_Renderer *renderer) {
    merge_sort_recursive(arr, 0, C.width - 1, renderer);
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

/**
 * Shell sort algorithm.
*/
void shell_sort(int *arr, SDL_Renderer *renderer) {
    for (int i = C.width / 2; i > 0; i /= 2) {
        for (int j = i; j < C.width; j++) {
            int tmp = arr[j];

            int k;
            for (k = j; k >= i && arr[k - i] > tmp; k -= i) {
                arr[k] = arr[k - i];
            }

            arr[k] = tmp;

            update_screen(arr, renderer, j, k);
        }
    }
}

/*** ARGUMENTS ***/

/**
 * Parse command-line arguments.
 * Sets global config defaults and changes optional values accordingly.
*/
void parse_args(int argc, char **argv) {
    C.width = 200;
    C.height = 150;
    C.scale = 5;
    C.delay = 0;
    C.algorithm = SELECTION_SORT;

    int opt;
    while ((opt = getopt(argc, argv, ":w:h:s:d:")) != -1) {
        switch (opt) {
            case 'w':
                C.width = atoi(optarg);
                if (C.width <= 0) {
                    fprintf(stderr, "Invalid width value\n");
                    exit(1);
                }
                break;
            case 'h':
                C.height = atoi(optarg);
                if (C.height <= 0) {
                    fprintf(stderr, "Invalid height value\n");
                    exit(1);
                }
                break;
            case 's':
                C.scale = atof(optarg);
                if (C.scale <= 0) {
                    fprintf(stderr, "Invalid scale value\n");
                    exit(1);
                }
                break;
            case 'd':
                C.delay = atoi(optarg);
                if (C.delay < 0) {
                    fprintf(stderr, "Invalid delay value\n");
                    exit(1);
                }
                break;
            case ':':
                fprintf(stderr, "Option requires a value\n");
                exit(1);
            case '?':
                fprintf(stderr, "Unknown option\n");
                exit(1);
        }
    }
}

/**
 * Handle command-line arguments.
 * Checks number of non-optional arguments and sets algorithm.
*/
void handle_args(int argc, char **argv) {
    int args_len = 0;
    int arg_pos;
    for (; optind < argc; optind++) {
        if (args_len == 0) arg_pos = optind;
        args_len++;
    }
    
    if (args_len > 1) {
        fprintf(stderr, "Too many arguments provided (use `help` to see usage)");
        exit(1);
    } else if (args_len == 1) {
        if (strcmp(argv[arg_pos], "help") == 0) {
            printf("Usage: sort-visualizer [-w WIDTH] [-h HEIGHT] [-s SCALE] [-d DELAY] [ALGORITHM]\n");
            printf("\n");
            printf("Default values:\n");
            printf("  WIDTH -- 200\n");
            printf("  HEIGHT -- 150\n");
            printf("  SCALE -- 5\n");
            printf("  DELAY -- 0\n");
            printf("  ALGORITHM -- ss\n");
            printf("\n");
            printf("Available algorithms:\n");
            printf("  Selection sort -- ss\n");
            printf("  Insertion sort -- is\n");
            printf("  Bubble sort -- bs\n");
            printf("  Merge sort -- ms\n");
            printf("  Quick sort -- qs\n");
            printf("  Shell sort -- shs\n");
            exit(0);
        } else if (strcmp(argv[arg_pos], "ss") == 0) {
            C.algorithm = SELECTION_SORT;
        } else if (strcmp(argv[arg_pos], "is") == 0) {
            C.algorithm = INSERTION_SORT;
        } else if (strcmp(argv[arg_pos], "bs") == 0) {
            C.algorithm = BUBBLE_SORT;
        } else if (strcmp(argv[arg_pos], "ms") == 0) {
            C.algorithm = MERGE_SORT;
        } else if (strcmp(argv[arg_pos], "qs") == 0) {
            C.algorithm = QUICK_SORT;
        } else if (strcmp(argv[arg_pos], "shs") == 0) {
            C.algorithm = SHELL_SORT;
        } else {
            fprintf(stderr, "Unknown algorithm (use `help` to see available ones)\n");
            exit(1);
        }
    }
}

/*** MAIN ***/

/**
 * Da main loop.
 * Initializes the array along with SDL and its components, it also executes the
 * sorting algorithm, listens for SDL events and performs the final cleanup.
*/
int main(int argc, char **argv) {
    parse_args(argc, argv);
    handle_args(argc, argv);

    srand(time(NULL));

    // Numbers in range [1, C.height - 1]
    int arr[C.width];
    for (int i = 0; i < C.width; i++) {
        arr[i] = randint(C.height - 1) + 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL subsystems: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Sort Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, C.width * C.scale, C.height * C.scale, 0);
    if (!window) {
        SDL_Log("Unable to create window: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: '%s'\n", SDL_GetError());
        return 1;
    }

    if (SDL_RenderSetScale(renderer, C.scale, C.scale) != 0) {
        SDL_Log("Unable to set render scale: '%s'\n", SDL_GetError());
        return 1;
    }

    clock_t begin = clock();

    switch (C.algorithm) {
        case SELECTION_SORT:
            selection_sort(arr, renderer);
            break;
        case INSERTION_SORT:
            insertion_sort(arr, renderer);
            break;
        case BUBBLE_SORT:
            bubble_sort(arr, renderer);
            break;
        case MERGE_SORT:
            merge_sort(arr, renderer);
            break;
        case QUICK_SORT:
            quick_sort(arr, 0, C.width - 1, renderer);
            break;
        case SHELL_SORT:
            shell_sort(arr, renderer);
            break;
    }

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
                
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_q) running = 0;
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