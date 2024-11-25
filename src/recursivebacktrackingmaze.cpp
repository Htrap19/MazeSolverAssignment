#include "recursivebacktrackingmaze.h"

#include <array>
#include <algorithm>

RecursiveBacktrackingMaze::RecursiveBacktrackingMaze(uint32_t width,
                                                     uint32_t height)
    : Maze(width, height)
{
}

void RecursiveBacktrackingMaze::generate(uint32_t seed)
{
    Maze::generate(seed);
    recursiveBacktracking(0, 0);
}

void RecursiveBacktrackingMaze::recursiveBacktracking(int cx, int cy)
{
    std::array<int, 4> directions = { 0, 1, 2, 3 };
    std::shuffle(directions.begin(),
                 directions.end(),
                 m_rng);

    m_grid[cy][cx] = 0;

    for (auto dir : directions)
    {
        int nx = cx + dx[dir] * 2;
        int ny = cy + dy[dir] * 2;

        if (isValidCell(nx, ny) && m_grid[ny][nx] == 1)
        {
            m_grid[cy + dy[dir]][cx + dx[dir]] = 0;

            recursiveBacktracking(nx, ny);
        }
    }
}
