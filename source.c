/*
    CreateTarget: https://math.stackexchange.com/questions/143932/calculate-point-given-x-y-angle-and-distance
    PointDistance: https://en.wikipedia.org/wiki/Pythagorean_theorem
    DrawRays: https://www.topcoder.com/thrive/articles/Geometry%20Concepts%20part%202:%20%20Line%20Intersection%20and%20its%20Applications
*/

#include <SDL2/SDL.h>   // Library used for drawing
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_TITLE "Raycasting with SDL2"
#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 500

#define TRUE 1
#define FALSE 0

// Macros to set the color of a renderer
#define COLOR_WHITE(x) SDL_SetRenderDrawColor(x, 255, 255, 255, 255);
#define COLOR_BLACK(x) SDL_SetRenderDrawColor(x, 0, 0, 0, 255);
#define COLOR_RED(x) SDL_SetRenderDrawColor(x, 255, 0, 0, 255);
#define COLOR_GREEN(x) SDL_SetRenderDrawColor(x, 0, 255, 0, 255);
#define COLOR_BLUE(x) SDL_SetRenderDrawColor(x, 0, 0, 255, 255);

#define FRAME_RENDER_DELAY 25 // Time waited after a frame to start rendering the new one
#define MAX_WALL_COUNT 128
#define MAX_BUFFER_SIZE 20
#define MAP_DATA_SEPERATOR "," // Seperator used in the map.txt file

#define PLAYER_STEP_SIZE 5 // Ammount of pixel the player can move each frame
#define PLAYER_VIEW_RANGE 1000
#define PLAYER_FOV 50 // Ammount of rays cast from the player in 1 degree increments
#define PLAYER_SPAWN_X 50
#define PLAYER_SPAWN_Y 50
#define PLAYER_SPAWN_DIR 0

// Custom var to hold the player data
typedef struct
{   
    int x;
    int y;
    int dir;
}PLAYER_STRUCT;

// Function prototypes
void SendFrame(SDL_Renderer * ren);
int PointDistance(int x1, int y1, int x2, int y2);
void DrawPlayer(SDL_Renderer * ren, PLAYER_STRUCT player);
int PointOnLine(int x1, int y1, int x2, int y2, int tx, int ty);
void DrawWalls(SDL_Renderer * ren, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount);
void DrawRays(SDL_Renderer * ren, PLAYER_STRUCT player, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount);
SDL_Point CreateTarget(int x, int y, int dir, int dist);
PLAYER_STRUCT MovePlayer(PLAYER_STRUCT player);

#ifdef _WIN32
int WinMain(int argc, char ** argv)
#else
int main(int argc, char ** argv)
#endif
{
    // The program doesnt take any arguments so we cast to void to aVOID(hehe) getting unused var errors 
    (void)argc;
    (void)argv; 

    SDL_Window * win;
    SDL_Renderer * ren;
    SDL_Event event;
    PLAYER_STRUCT player = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y, PLAYER_SPAWN_DIR};
    SDL_Point walls[MAX_WALL_COUNT][2];
    int wallsCount = 0;
    int running = TRUE;

    // Configure the window and renderer to be centered and shown
    win = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, 0);

    // Load map data
    FILE* fp;
    fp = fopen("map.txt", "r");

    // To read the data we use strtok to split the buffer into each instance of the seperation character
    char buffer[MAX_BUFFER_SIZE];
    char *token;
    while (fgets(buffer, MAX_BUFFER_SIZE - 1, fp))
    {
        token = strtok(buffer, MAP_DATA_SEPERATOR);
        walls[wallsCount][0].x = atoi(token);
        token = strtok(NULL, MAP_DATA_SEPERATOR);
        walls[wallsCount][0].y = atoi(token);
        token = strtok(NULL, MAP_DATA_SEPERATOR);
        walls[wallsCount][1].x = atoi(token);
        token = strtok(NULL, MAP_DATA_SEPERATOR);
        walls[wallsCount][1].y = atoi(token);
        wallsCount++;
    }

    fclose(fp); // Close your darn files

    // Main event loop
    while (running)
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT)
            {
                running = FALSE;
            }
        }

        // Handle KeyBoard
        player = MovePlayer(player);

        // Render world
        DrawWalls(ren, walls, wallsCount);
        DrawRays(ren, player, walls, wallsCount);
        DrawPlayer(ren, player);
        SendFrame(ren);

        SDL_Delay(FRAME_RENDER_DELAY);
    }

    // Destroy the window and renderer
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    return 0;
}

PLAYER_STRUCT MovePlayer(PLAYER_STRUCT player)
{
    // Get a snapshot of the keyboard
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    // Check what keys where held down and if we are not at the edge of the screen
    if (keys[SDL_SCANCODE_W] && player.y > PLAYER_STEP_SIZE)
    {
        player.y -= PLAYER_STEP_SIZE;
    }

    if (keys[SDL_SCANCODE_A] && player.x > PLAYER_STEP_SIZE)
    {
        player.x -= PLAYER_STEP_SIZE;
    }

    if (keys[SDL_SCANCODE_S] && player.y < WINDOW_HEIGHT - PLAYER_STEP_SIZE)
    {
        player.y += PLAYER_STEP_SIZE;
    }

    if (keys[SDL_SCANCODE_D] && player.x < WINDOW_WIDTH /3 - PLAYER_STEP_SIZE)
    {
        player.x += PLAYER_STEP_SIZE;
    }
    
    if (keys[SDL_SCANCODE_Q])
    {
        player.dir -= PLAYER_STEP_SIZE;
    }

    if (keys[SDL_SCANCODE_E])
    {
        player.dir += PLAYER_STEP_SIZE;
    }

    return player;
}

SDL_Point CreateTarget(int x, int y, int dir, int dist)
{
    // Create point dist pixels away in dir direction from x and y
    SDL_Point target;

    double traslated = dir * (M_PI / 180.0); // Convert to radians
    target.x = round(x + dist * cos(traslated));
    target.y = round(y + dist * sin(traslated));

    return target;
}

int PointOnLine(int x1, int y1, int x2, int y2, int tx, int ty)
{
    // Check if a point is on a line segment
    if (fmin(x1, x2) <= tx && tx <= fmax(x1, x2) && fmin(y1, y2) <= ty  && ty <= fmax(y1, y2))
    {
        return TRUE;
    }

    return FALSE;
}

int PointDistance(int x1, int y1, int x2, int y2)
{
    // Find the distance between two points
    int out = sqrt( pow(x2 - x1, 2) + pow(y2 - y1, 2) );
    return out;
}

void DrawRays(SDL_Renderer * ren, PLAYER_STRUCT player, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount)
{
    // Find ray and wall intersections and draw them
    SDL_Point target;
    SDL_Point closest;
    SDL_Rect rect;

    // The thickness the bars can be based on the window width
    int rayPrevieuwRenderStep = WINDOW_WIDTH * 0.6667 / PLAYER_FOV;
    int rayCount = 0;
    int closestDistance;
    rect.w = rayPrevieuwRenderStep; // We can pre calculate the width as this is a constant

    // Loop for each ray in fov at 1 degree step
    for (int a = player.dir - PLAYER_FOV /2; a < player.dir + PLAYER_FOV /2; a++)
    {
        rayCount++;
        target = CreateTarget(player.x, player.y, a, PLAYER_VIEW_RANGE); // Find the point at wich the ray is out of view distance
        closest = target;
        closestDistance = PLAYER_VIEW_RANGE;

        // Loop over each wall
        for (int i = 0; i < wallCount; i++)
        {
            // I wont go into detail of the actual math, it will be linked at the top
            float a1 = target.y - player.y;
            float b1 = player.x - target.x;
            float c1 = a1 * player.x + b1 * player.y;

            float a2 = walls[i][1].y - walls[i][0].y;
            float b2 = walls[i][0].x - walls[i][1].x;
            float c2 = a2 * walls[i][0].x + b2 * walls[i][0].y;

            float det = a1 * b2 - a2 * b1;

            // If its not parallel
            if (det != 0)
            {
                int x = (b2 * c1 - b1 * c2) / det;
                int y = (a1 * c2 - a2 * c1) / det;

                // If the point falls inside the wall and the ray
                if (PointOnLine(player.x, player.y, target.x, target.y, x, y) && 
                    PointOnLine(walls[i][0].x, walls[i][0].y, walls[i][1].x, walls[i][1].y, x, y))
                {
                    // Calculate the distance to the point
                    int dist = PointDistance(player.x, player.y, x, y);

                    // If its the closest wall, store it
                    if (dist < closestDistance)
                    {
                        closest.x = x;
                        closest.y = y;
                        closestDistance = dist;
                    }
                }
            }
        }

        // If the ray hit something
        if (closestDistance != PLAYER_VIEW_RANGE)
        {
            COLOR_GREEN(ren);
            SDL_RenderDrawLine(ren, player.x, player.y, closest.x, closest.y);

            // Draw the 3D preview
            rect.x = rayPrevieuwRenderStep * rayCount + WINDOW_WIDTH /3;
            rect.y = closestDistance /2;
            rect.h = WINDOW_HEIGHT - rect.y * 2;

            int brightness = 255 - closestDistance /2;

            SDL_SetRenderDrawColor(ren, brightness, brightness, brightness, 255);
            SDL_RenderFillRect(ren, &rect);
        }
    }
}

void DrawWalls(SDL_Renderer * ren, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount)
{
    // Loop over each wall and draw it
    COLOR_WHITE(ren);

    for (int i = 0; i < wallCount; i++)
    {
        SDL_RenderDrawLine(ren, walls[i][0].x, walls[i][0].y, walls[i][1].x, walls[i][1].y);
    }
}

void DrawPlayer(SDL_Renderer * ren, PLAYER_STRUCT player)
{
    // Draw player
    COLOR_RED(ren);
    SDL_Point target = CreateTarget(player.x, player.y, player.dir, 20); // Calculate the end of the line used to indicate the players direction
    SDL_RenderDrawLine(ren, player.x, player.y, target.x, target.y);
}

void SendFrame(SDL_Renderer * ren)
{
    // Push the frame stored in the renderer to the screen
    COLOR_BLACK(ren);
    SDL_RenderPresent(ren);
    SDL_RenderClear(ren);
}