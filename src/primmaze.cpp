#include "primmaze.h"

PrimMaze::PrimMaze(uint32_t width,
                   uint32_t height)
    : Maze(width, height)
{
}

void PrimMaze::generate(uint32_t seed)
{
    Maze::generate(seed);

    // Get modifiable grid
    auto& grid = const_cast<std::vector<std::vector<int>>&>(getGrid());

    // Vector to store walls
    std::vector<Wall> walls;

    // Start from cell (0,0)
    int startX = 0;
    int startY = 0;

    // Mark the starting cell as part of the maze
    grid[startY][startX] = 0;

    // Add the walls around the starting cell
    addWalls(walls, startX, startY);

    // While there are walls to process
    while (!walls.empty())
    {
        // Randomly select a wall
        std::uniform_int_distribution<size_t> dist(0, walls.size() - 1);
        size_t wallIndex = dist(m_rng);
        Wall currentWall = walls[wallIndex];

        // Remove the selected wall from the vector
        walls[wallIndex] = walls.back();
        walls.pop_back();

        // If the cell on the other side of the wall isn't in the maze yet
        if (isWall(currentWall.nx, currentWall.ny))
        {
            // Add the cell to the maze
            grid[currentWall.ny][currentWall.nx] = 0;
            // Remove the wall between the cells
            grid[currentWall.y][currentWall.x] = 0;

            // Add new walls to consider
            addWalls(walls, currentWall.nx, currentWall.ny);
        }
    }
}

void PrimMaze::addWalls(std::vector<Wall> &walls, int x, int y)
{
    // Use the direction arrays from base class
    for (int i = 0; i < 4; i++)
    {
        // Calculate next cell coordinates using base class dx and dy arrays
        int nx = x + (dx[i] * 2);  // Next cell x
        int ny = y + (dy[i] * 2);  // Next cell y

        // Check if the next cell is within bounds
        if (nx >= 0 && nx < getWidth() && ny >= 0 && ny < getHeight())
        {
            // If the next cell is a wall (unvisited)
            if (isWall(nx, ny))
            {
                // Add the wall between current cell and next cell
                // Wall coordinates are halfway between current and next cell
                walls.emplace_back(x + dx[i], y + dy[i], nx, ny);
            }
        }
    }
}
