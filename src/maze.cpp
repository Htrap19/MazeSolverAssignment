#include "maze.h"

#include <iostream>

Maze::Maze(uint32_t width, uint32_t height)
{
    m_width = width % 2 == 0 ? width + 1 : width;
    m_height = height % 2 == 0 ? height + 1 : height;

    m_grid = std::vector(m_height, std::vector(m_width, 1));
}

void Maze::generate(uint32_t seed)
{
    clear();

    m_rng.seed(seed == 0 ? std::random_device{}() : seed);
}

void Maze::clear()
{
    m_grid = std::vector(m_height, std::vector(m_width, 1));
}

bool Maze::isValidCell(int cx, int cy) const
{
    return cx >= 0 && cy >= 0 && cx < m_width && cy < m_height;
}

bool Maze::isWall(int cx, int cy) const
{
    return m_grid[cy][cx] == 1;
}

void Maze::setSize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}

uint32_t Maze::getWidth() const
{
    return m_width;
}

uint32_t Maze::getHeight() const
{
    return m_height;
}

const
std::vector<std::vector<int>>& Maze::getGrid() const
{
    return m_grid;
}

void Maze::print() const
{
    for (const auto& row : m_grid)
    {
        for (int cell : row)
        {
            std::cout << (cell ? "█" : " ");
        }
        std::cout << std::endl;
    }
}
