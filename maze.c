/**
 * @file maze.c
 * @author (YOUR NAME)
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// defines for the required autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this pointer to the maze to be initialised
 * @param height height to allocate
 * @param width width to allocate
 * @return int 0 on success, 1 on fail
 */
int create_maze(maze *this, int height, int width)
{
    this->height = height;
    this->width = width;

    // Allocate memory for the maze map
    this->map = malloc(height * sizeof(char *));
    if (this->map == NULL)
    {
        return 1;
    }

    for (int i = 0; i < height; i++)
    {
        this->map[i] = malloc(width * sizeof(char));
        if (this->map[i] == NULL)
        {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this the pointer to the struct to free
 */
void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        free(this->map[i]);
    }
    free(this->map);
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file)
{
    char line[MAX_DIM + 2]; // Including space for newline and null terminator
    if (fgets(line, sizeof(line), file) != NULL)
    {
        int width = strlen(line);
        if (line[width - 1] == '\n')
        {
            width--; // Exclude newline character
        }
        if (width >= MIN_DIM && width <= MAX_DIM)
        {
            return width;
        }
    }
    return 0;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file)
{
    char line[MAX_DIM + 2];
    int height = 0;
    int width = 0;

    rewind(file); // Ensure file pointer is at the start
    while (fgets(line, sizeof(line), file) != NULL)
    {
        int new_width = strlen(line);
        if (line[new_width - 1] == '\n')
        {
            new_width--;
        }

        if (new_width != width && height > 0)
        {
            if (line[0] != '\n') // Ignore the trailing newline if it exists
            {
                return 0;
            }
        }
        if (line[0] != '\n') // Only count non-empty lines
        {
            height++;
            if (width == 0)
            {
                width = strlen(line);
                if (line[width - 1] == '\n')
                {
                    width--; // Exclude newline character
                }
            }
        }
    }
    if (height >= MIN_DIM && height <= MAX_DIM)
    {
        return height;
    }
    return 0;
}

/**
 * @brief Checks if the maze contains exactly one 'S' and one 'E'
 *
 * @param this Maze struct to be used
 * @return int 0 on success, 1 on fail
 */
int validate_start_and_end(maze *this)
{
    int start_count = 0;
    int end_count = 0;

    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (this->map[i][j] == 'S')
            {
                start_count++;
            }
            if (this->map[i][j] == 'E')
            {
                end_count++;
            }
        }
    }

    if (start_count == 1 && end_count == 1)
    {
        return 0;
    }

    return 1;
}

/**
 * @brief Checks if the maze contains only valid characters
 *
 * @param this Maze struct to be used
 * @return int 0 on success, 1 on fail
 */
int validate_characters(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (this->map[i][j] != 'S' && this->map[i][j] != 'E' && this->map[i][j] != ' ' && this->map[i][j] != '#')
            {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on fail
 */
int read_maze(maze *this, FILE *file)
{
    int i = 0, j = 0;
    char ch;
    rewind(file);

    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            if (j != 0)
            {
                i++;
                j = 0;
            }
        }
        else
        {
            this->map[i][j] = ch;
            if (ch == 'S')
            {
                this->start.x = j;
                this->start.y = i;
            }
            if (ch == 'E')
            {
                this->end.x = j;
                this->end.y = i;
            }
            j++;
        }
    }

    // Validate start and end points
    if (validate_start_and_end(this))
    {
        return 1;
    }

    // Validate characters
    if (validate_characters(this))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this pointer to maze to print
 * @param player the current player location
 */
void print_maze(maze *this, coord *player)
{
    // make sure we have a leading newline..
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            // decide whether player is on this spot or not
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        // end each row with a newline.
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction)
{
    int new_x = player->x;
    int new_y = player->y;

    switch (direction)
    {
    case 'W':
    case 'w':
        new_y--;
        break;
    case 'A':
    case 'a':
        new_x--;
        break;
    case 'S':
    case 's':
        new_y++;
        break;
    case 'D':
    case 'd':
        new_x++;
        break;
        break;
    default:
        printf("Invalid move\n");
        return;
    }

    if (new_x < 0 || new_y < 0 || new_x >= this->width || new_y >= this->height)
    {
        printf("Move out of bounds\n");
        return;
    }

    if (this->map[new_y][new_x] == '#')
    {
        printf("Move into a wall\n");
        return;
    }

    player->x = new_x;
    player->y = new_y;
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this current maze
 * @param player player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[])
{
    // check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }
    // set up some useful variables (you can rename or remove these if you want)
    maze this_maze;
    FILE *file = fopen(argv[1], "r");
    if (!file) 
    {
        perror("Error opening file");
        return EXIT_FILE_ERROR;
    }

    int width = get_width(file);
    if (width == 0)
    {
        fprintf(stderr, "Invalid maze width\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    int height = get_height(file);
    if (height == 0)
    {
        fprintf(stderr, "Invalid maze height\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    if (create_maze(&this_maze, height, width))
    {
        fprintf(stderr, "Error creating maze\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    if (read_maze(&this_maze, file))
    {
        fprintf(stderr, "Error reading maze\n");
        free_maze(&this_maze);
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    fclose(file);

    coord player = this_maze.start;

    char input;
    while (1)
    {
        input = getchar();
        if (input == '\n')
        {
            continue;
        }
        if (input == 'Q' || input == 'q')
        {
            break;
        }
        else if (input == 'M' || input == 'm')
        {
            print_maze(&this_maze, &player);
        }
        else
        {
            move(&this_maze, &player, input);
            if (has_won(&this_maze, &player))
            {
                printf("Congratulations! You've reached the exit!\n");
                break;
            }
        }
    }

    free_maze(&this_maze);
    return EXIT_SUCCESS;

}