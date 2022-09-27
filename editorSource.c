#include <SDL2/SDL.h>   // Library for drawing
#include <stdio.h>
#include <string.h>

#define WINDOW_TITLE "Raycasting with SDL2 (MapEditor)"
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define TRUE 1
#define FALSE 0

// Macros to set the color of the renderer
#define COLOR_WHITE(x) SDL_SetRenderDrawColor(x, 255, 255, 255, 255);
#define COLOR_BLACK(x) SDL_SetRenderDrawColor(x, 0, 0, 0, 255);

#define MAX_WALL_COUNT 128
#define MAX_BUFFER_SIZE 20
#define MAP_DATA_SEPERATOR "," // Character used to split the data in map.txt

void DrawWalls(SDL_Renderer * ren, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount);
void SendFrame(SDL_Renderer * ren);

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
    SDL_Point stored[2]; // Var to store the start and end of a line
    SDL_Point walls[MAX_WALL_COUNT][2];

    int firstClick = TRUE;
    int running = TRUE;
    int wallsCount = 0;

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
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {   
                if (firstClick == TRUE)
                {
                    SDL_GetMouseState(&stored[0].x, &stored[0].y); // Store the postion of the mouse at the moment of the click
                    firstClick = FALSE;
                }
                else
                {
                    if (wallsCount +1 != MAX_WALL_COUNT) // If we still have wall slots left
                    {
                        firstClick = TRUE; // Reset firstClick
                        SDL_GetMouseState(&stored[1].x, &stored[1].y); // Store the postion of the mouse at the moment of the click

                        // Use the stored postions to add a new wall
                        walls[wallsCount][0] = stored[0];
                        walls[wallsCount][1] = stored[1];
                        wallsCount++;
                    }
                }
            }
        }

        // Render world
        DrawWalls(ren, walls, wallsCount);
        SendFrame(ren);
    }

    // Store new map data
    fp = fopen("map.txt", "w");

    // Loop over each wall and add it to the file in the format "x1,y1,x2,y2" where ',' is the seperation character
    for (int i = 0; i < wallsCount; i++)
    {
        fprintf(fp,"%d%s%d%s%d%s%d\n", walls[i][0].x, MAP_DATA_SEPERATOR, walls[i][0].y, MAP_DATA_SEPERATOR, walls[i][1].x, MAP_DATA_SEPERATOR, walls[i][1].y);
    }

    // Clean up
    fclose(fp);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    return 0;
}

void DrawWalls(SDL_Renderer * ren, SDL_Point walls[MAX_WALL_COUNT][2], int wallCount)
{
    COLOR_WHITE(ren);

    // Draw all the walls by looping over them
    for (int i = 0; i < wallCount; i++)
    {
        SDL_RenderDrawLine(ren, walls[i][0].x, walls[i][0].y, walls[i][1].x, walls[i][1].y);
    }
}

void SendFrame(SDL_Renderer * ren)
{
    // Push the frame stored in the renderer to the screen
    COLOR_BLACK(ren);
    SDL_RenderPresent(ren);
    SDL_RenderClear(ren);
}