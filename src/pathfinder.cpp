#include "pathfinder.h"
#include "renderer.h"

#include <GLFW/glfw3.h>


#include <iostream>
#include <algorithm>

PathFinder::PathFinder(const std::shared_ptr<Maze>& maze)
    : m_maze(maze)
{}

void PathFinder::printPath(const std::vector<Point> &path) const
{
    auto height = m_maze->getHeight();
    auto width = m_maze->getWidth();
    const auto& grid = m_maze->getGrid();
    std::vector<std::vector<char>> display(height, std::vector<char>(width, ' '));

    // Fill with maze
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            display[y][x] = grid[y][x] ? '|' : ' ';
        }
    }

    // Mark path
    for (const auto& p : path)
    {
        display[p.y][p.x] = '*';
    }

    // Print
    for (const auto& row : display)
    {
        for (char cell : row)
        {
            std::cout << cell;
        }
        std::cout << std::endl;
    }

}

void PathFinder::setMaze(const std::shared_ptr<Maze> &maze)
{
    m_maze = maze;
}

const std::shared_ptr<Maze>& PathFinder::getMaze() const
{
    return m_maze;
}

std::vector<Point> PathFinder::getNeighbors(const Point &p) const
{
    std::vector<Point> neighbors;

    for (uint8_t i = 0; i < 4; i++)
    {
        auto nx = p.x + Maze::dx[i];
        auto ny = p.y + Maze::dy[i];

        if (m_maze->isValidCell(nx, ny) &&
            !m_maze->isWall(nx, ny))
        {
            neighbors.push_back(Point{nx, ny});
        }
    }

    return neighbors;
}

std::vector<Point>
PathFinder::reconstructPath(std::unordered_map<Point, Point, PointHash> &cameFrom,
                            Point current)
{
    std::vector<Point> path;
    path.push_back(current);

    while (cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom[current];
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}
