#include "kruskalmaze.h"

#include <algorithm>

KruskalMaze::KruskalMaze(uint32_t width,
                                   uint32_t height)
    : Maze(width, height)
{
}

void KruskalMaze::generate(uint32_t seed)
{
    Maze::generate(seed);

    // Create a list of all possible walls
    std::vector<Wall> walls;
    for (int y = 0; y < getHeight(); y += 2)
    {
        for (int x = 0; x < getWidth(); x += 2)
        {
            // Add horizontal walls
            if (x + 2 < getWidth())
            {
                walls.emplace_back(x, y, x + 2, y, x + 1, y);
            }
            // Add vertical walls
            if (y + 2 < getHeight())
            {
                walls.emplace_back(x, y, x, y + 2, x, y + 1);
            }
        }
    }

    // Create cells
    auto& grid = const_cast<std::vector<std::vector<int>>&>(getGrid());
    for (int y = 0; y < getHeight(); y += 2)
    {
        for (int x = 0; x < getWidth(); x += 2)
        {
            grid[y][x] = 0;
        }
    }

    // Shuffle walls
    std::shuffle(walls.begin(), walls.end(), m_rng);

    // Initialize disjoint set
    int cellCount = ((getWidth() + 1) / 2) * ((getHeight() + 1) / 2);
    DisjointSet ds(cellCount);

    // Process each wall
    for (const Wall& wall : walls)
    {
        // Convert coordinates to cell indices
        int cell1 = (wall.y1 / 2) * ((getWidth() + 1) / 2) + (wall.x1 / 2);
        int cell2 = (wall.y2 / 2) * ((getWidth() + 1) / 2) + (wall.x2 / 2);

        // If cells are not connected, remove wall
        if (!ds.connected(cell1, cell2))
        {
            grid[wall.wy][wall.wx] = 0;  // Remove wall
            ds.unite(cell1, cell2);      // Unite cells
        }
    }
}
