#include "maze.h"

#include <algorithm>
#include <iostream>

Maze::Maze(uint32_t width, uint32_t height)
{
    m_width = width % 2 == 0 ? width + 1 : width;
    m_height = height % 2 == 0 ? height + 1 : height;

    m_grid = std::vector(m_width, std::vector(m_height, 0));
}

void Maze::generate(uint32_t seed)
{
    clear();

    m_rng.seed(seed == 0 ? std::random_device{}() : seed);

    recursiveBacktracking(0, 0);
}

void Maze::clear()
{
    m_grid = std::vector(m_height, std::vector(m_width, 0));
}

bool Maze::isValidCell(int cx, int cy)
{
    return cx >= 0 && cy >= 0 && cx < m_width && cy < m_height;
}

bool Maze::isWall(int cx, int cy) const
{
    return m_grid[cy][cx] == 0;
}

void Maze::recursiveBacktracking(int cx, int cy)
{
    std::array<int, 4> directions = {N, S, E, W};
    std::shuffle(directions.begin(),
                 directions.end(),
                 m_rng);

    for (int direction : directions) {
        int nx = cx + DX.at(direction);
        int ny = cy + DY.at(direction);

        // Check bounds and whether the cell has been visited
        if (ny >= 0 && ny < m_grid.size() &&
            nx >= 0 && nx < m_grid[ny].size()
            && m_grid[ny][nx] == 0) {
            m_grid[cy][cx] |= direction;
            m_grid[ny][nx] |= OPPOSITE.at(direction);
            recursiveBacktracking(nx, ny);
        }
    }

    // //                        N   E   S   W
    // static const int dx[] = { 0,  1,  0, -1 };
    // static const int dy[] = { 1,  0, -1,  0 };
    // static std::unordered_map<int, int> opposite;
    // opposite[0] = 2;
    // opposite[1] = 3;
    // opposite[2] = 0;
    // opposite[3] = 1;

    // std::array<int, 4> directions = { 0, 1, 2, 3 };
    // std::shuffle(directions.begin(),
    //              directions.end(),
    //              m_rng);

    // for (auto dir : directions)
    // {
    //     int nx = cx + dx[dir];
    //     int ny = cy + dy[dir];

    //     if (isValidCell(nx, ny) && m_grid[ny][nx] == 1)
    //     {
    //         m_grid[cy][cx] |= dir;
    //         m_grid[ny][nx] |= opposite[dir];

    //         recursiveBacktracking(nx, ny);
    //     }
    // }
}

const
std::vector<std::vector<int>>& Maze::getGrid() const
{
    return m_grid;
}

void Maze::print() const
{
    int height = m_grid.size();
    int width = m_grid[0].size();

    // For each cell, decide whether to display a wall or a path
    for (int y = 0; y < height * 2 + 1; ++y)
    {
        for (int x = 0; x < width * 2 + 1; ++x)
        {
            if (y % 2 == 0 || x % 2 == 0)
            {
                // Wall logic: check if it's between paths or at the edge
                if (y % 2 == 0 && x % 2 == 0)
                {
                    std::cout << "█";  // Corner of walls
                }
                else if (y % 2 == 0)
                {
                    int cx = x / 2;
                    int cy = (y - 1) / 2;
                    std::cout << ((cy >= 0 && cy < height && (m_grid[cy][cx] & S)) ? " " : "█");  // Horizontal wall or path
                }
                else if (x % 2 == 0)
                {
                    int cx = (x - 1) / 2;
                    int cy = y / 2;
                    std::cout << ((cx >= 0 && cx < width && (m_grid[cy][cx] & E)) ? " " : "█");  // Vertical wall or path
                }
                else
                {
                    std::cout << "█";  // Edge wall
                }
            }
            else
            {
                // Path logic: print spaces for paths
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}
