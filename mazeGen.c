#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

void usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <filename> <width> <height>\n", prog_name);
}



typedef struct {
    int x, y;
} Point;

void generate_maze(char **maze, int width, int height) {
    // Initialize maze with walls
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            maze[i][j] = '#';
        }
    }

    // Starting point
    Point stack[width * height];
    int stackSize = 0;
    Point current = {1, 1};
    maze[current.y][current.x] = ' ';

    // DFS algorithm
    stack[stackSize++] = current;

    int directions[4][2] = {
        {0, 2}, {2, 0}, {0, -2}, {-2, 0}
    };

    srand(time(NULL));

    while (stackSize > 0) {
        Point next;
        int found = 0;
        for (int i = 0; i < 4; i++) {
            int r = rand() % 4;
            int dx = directions[r][0];
            int dy = directions[r][1];

            next.x = current.x + dx;
            next.y = current.y + dy;

            if (next.x > 0 && next.x < width - 1 && next.y > 0 && next.y < height - 1 &&
                maze[next.y][next.x] == '#') {
                maze[next.y][next.x] = ' ';
                maze[current.y + dy / 2][current.x + dx / 2] = ' ';
                stack[stackSize++] = next;
                current = next;
                found = 1;
                break;
            }
        }
        if (!found) {
            current = stack[--stackSize];
        }
    }
}

char **create_empty_maze(int width, int height) {
    char **maze = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        maze[i] = malloc(width * sizeof(char));
    }
    return maze;
}

void free_maze(char **maze, int height) {
    for (int i = 0; i < height; i++) {
        free(maze[i]);
    }
    free(maze);
}

void place_start_and_end(char **maze, int width, int height) {
    // Randomly select start and end points
    int start_x, start_y, end_x, end_y;
    start_x = rand() % (width - 2) + 1;
    start_y = rand() % (height - 2) + 1;
    end_x = rand() % (width - 2) + 1;
    end_y = rand() % (height - 2) + 1;

    // Ensure start and end points are not at the same position
    while (start_x == end_x && start_y == end_y) {
        end_x = rand() % (width - 2) + 1;
        end_y = rand() % (height - 2) + 1;
    }

    // Place start and end points
    maze[start_y][start_x] = 'S';
    maze[end_y][end_x] = 'E';
}


void save_maze_to_file(char **maze, int width, int height, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FILE_ERROR);
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fputc(maze[i][j], file);
        }
        fputc('\n', file);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return EXIT_ARG_ERROR;
    }

    char *filename = argv[1];
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);

    if (width < MIN_DIM || width > MAX_DIM || height < MIN_DIM || height > MAX_DIM) {
        fprintf(stderr, "Dimensions must be between %d and %d\n", MIN_DIM, MAX_DIM);
        return EXIT_ARG_ERROR;
    }

    char **maze = create_empty_maze(width, height);
    generate_maze(maze, width, height);
    place_start_and_end(maze, width, height); // Place start and end points
    save_maze_to_file(maze, width, height, filename);
    free_maze(maze, height);


    return EXIT_SUCCESS;
}
