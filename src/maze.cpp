#include "maze.h"

#include <algorithm>
#include <iostream>

Maze::Maze(uint32_t width, uint32_t height)
{
    m_width = width % 2 == 0 ? width + 1 : width;
    m_height = height % 2 == 0 ? height + 1 : height;

    m_grid = std::vector(m_width, std::vector(m_height, 1));
}

void Maze::generate(uint32_t seed)
{
    clear();

    m_rng.seed(seed == 0 ? std::random_device{}() : seed);

    recursiveBacktracking(0, 0);
}

void Maze::clear()
{
    m_grid = std::vector(m_height, std::vector(m_width, 1));
}

bool Maze::isValidCell(int cx, int cy)
{
    return cx >= 0 && cy >= 0 && cx < m_width && cy < m_height;
}

bool Maze::isWall(int cx, int cy)
{
    return m_grid[cy][cx] == 1;
}

void Maze::recursiveBacktracking(int cx, int cy)
{
    static const int dx[] = { 0, 1,  0, -1 };
    static const int dy[] = { 1, 0, -1,  0 };

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

const
std::vector<std::vector<int>>& Maze::getGrid() const
{
    return m_grid;
}

void Maze::print() const
{
    for (const auto& row : m_grid) {
        for (int cell : row) {
            std::cout << (cell ? "█" : " ");
        }
        std::cout << std::endl;
    }
}
