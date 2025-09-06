#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/mcsyscalls.c"

#define AIR 0
#define STONE 1
#define DIRT 2
#define WOOD 3

#define MAP_W 20
#define MAP_H 20
#define MAP_SIZE (MAP_W * MAP_H)

struct key_press
{
    bool w;
    bool a;
    bool s;
    bool d;
};

typedef struct key_press key_press;

// Snake node structure
typedef struct SnakeNode
{
    int x, y;
    struct SnakeNode *next;
} SnakeNode;

// Global map buffers (current displayed state and next frame)
static int current_map[MAP_SIZE];
static int next_map[MAP_SIZE];

// Snake head and food position
static SnakeNode *head = NULL;
static int food_x, food_y;
static int direction = 1; // 0=up, 1=right, 2=down, 3=left (start facing right)
static bool game_over = false;

void set_block(int x, int y, int z, int id)
{
    place_block(x, y, z, id);
}

key_press get_key_press()
{
    key_press kp;
    kp.w = run_if("{w: true}", 9);
    kp.a = run_if("{a: true}", 9);
    kp.s = run_if("{s: true}", 9);
    kp.d = run_if("{d: true}", 9);
    return kp;
}

// Map 2D coordinate to linear array index
int map_index(int x, int y)
{
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
        return -1;
    return y * MAP_W + x;
}

// Initialize map as empty (AIR)
void init_map()
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        current_map[i] = AIR;
        next_map[i] = AIR;
    }
}

// Set block in next_map (virtual frame buffer)
void set_next_block(int x, int y, int id)
{
    int idx = map_index(x, y);
    if (idx != -1)
    {
        next_map[idx] = id;
    }
}

// Flush changes: compare next_map with current_map, update only differences
void flush_map()
{
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            int idx = map_index(x, y);

            if (current_map[idx] != next_map[idx])
            {
                set_block(x, y, 0, next_map[idx]); // z=0 fixed
                current_map[idx] = next_map[idx];
            }
        }
    }
}

// Create new snake node
SnakeNode *create_node(int x, int y)
{
    SnakeNode *node = malloc(sizeof(SnakeNode));
    node->x = x;
    node->y = y;
    node->next = NULL;
    return node;
}

// Initialize snake (length 3, start near center facing right)
__attribute__((optimize("O0"))) void init_snake()
{
    int start_x = MAP_W / 2 - 2;
    int start_y = MAP_H / 2;

    printf("Initializing snake at (%d, %d)\n", start_x, start_y);

    // head → mid → tail
    head = create_node(start_x + 2, start_y);
    SnakeNode *mid = create_node(start_x + 1, start_y);
    SnakeNode *tail = create_node(start_x, start_y);

    head->next = mid;
    mid->next = tail;
    tail->next = NULL;
}

// Free all snake nodes
void free_snake()
{
    while (head)
    {
        SnakeNode *temp = head;
        head = head->next;
        free(temp);
    }
}

// Check if position is occupied by snake body
bool is_position_occupied(int x, int y)
{
    SnakeNode *cur = head;
    while (cur)
    {
        if (cur->x == x && cur->y == y)
            return true;
        cur = cur->next;
    }
    return false;
}

// Spawn food at random position (not overlapping snake)
void spawn_food()
{
    int tries = 0;
    do
    {
        food_x = rand() % MAP_W;
        food_y = rand() % MAP_H;
        tries++;
        if (tries > 100)
            break; // Prevent infinite loop if map is full
    } while (is_position_occupied(food_x, food_y));

    set_next_block(food_x, food_y, WOOD);
}

// Move snake one step
void move_snake()
{
    // Calculate new head position
    int new_x = head->x;
    int new_y = head->y;

    // printf("Moving snake to (%d, %d)\n", new_x, new_y);

    switch (direction)
    {
    case 0:
        new_y--;
        break; // up
    case 1:
        new_x++;
        break; // right
    case 2:
        new_y++;
        break; // down
    case 3:
        new_x--;
        break; // left
    }

    // Check wall collision
    if (new_x < 0 || new_x >= MAP_W || new_y < 0 || new_y >= MAP_H)
    {
        printf("Wall collision!\n");
        game_over = true;
        return;
    }

    // Check self-collision (excluding new head position, since tail will move)
    SnakeNode *cur = head->next;
    while (cur)
    {
        if (cur->x == new_x && cur->y == new_y)
        {
            printf("Self-collision at (%d, %d)\n", new_x, new_y);
            game_over = true;
            return;
        }
        cur = cur->next;
    }

    // Create new head
    SnakeNode *new_head = create_node(new_x, new_y);
    new_head->next = head;
    head = new_head;

    bool ate_food = (new_x == food_x && new_y == food_y);

    if (!ate_food)
    {
        // Remove tail if not eating
        SnakeNode *tail = head;
        while (tail->next->next)
        {
            tail = tail->next;
        }
        SnakeNode *old_tail = tail->next;
        tail->next = NULL;
        set_next_block(old_tail->x, old_tail->y, AIR); // Clear old tail
        free(old_tail);
    }
    else
    {
        spawn_food(); // Respawn food after eating
    }

    // Update body (everything after head)
    cur = head->next;
    while (cur)
    {
        set_next_block(cur->x, cur->y, DIRT);
        cur = cur->next;
    }

    // Update head
    set_next_block(head->x, head->y, STONE);
}

// Handle input and update direction (no 180° turns)
void handle_input()
{
    key_press kp = get_key_press();
    if (kp.w && direction != 2)
        direction = 0; // up
    else if (kp.s && direction != 0)
        direction = 2; // down
    else if (kp.d && direction != 3)
        direction = 1; // right
    else if (kp.a && direction != 1)
        direction = 3; // left
}

int main()
{
    srand(1145);

    init_map();
    init_snake();

    bool first_loop = true;

    // Main game loop
    while (!game_over)
    {
        // Clear next_map, preserve food position
        for (int i = 0; i < MAP_SIZE; i++)
        {
            if (current_map[i] != WOOD)
            {
                next_map[i] = AIR;
            }
            else
            {
                next_map[i] = WOOD; // Keep food
            }
        }

        if (first_loop)
        {
            first_loop = false;
            spawn_food(); // Spawn food on first loop
        }

        handle_input();
        move_snake();

        if (!game_over)
        {
            flush_map(); // Only update changed blocks
        }
    }

    printf("Game over!\n");

    free_snake();
    return 0;
}