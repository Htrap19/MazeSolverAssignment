#include "pathfinder.h"
#include "renderer.h"

#include <GLFW/glfw3.h>

#include <queue>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

static Renderer s_renderer;

PathFinder::PathFinder(const Maze& maze)
    : m_maze(maze)
{}

std::pair<std::vector<Point>, std::vector<IterationData>>
PathFinder::findPath(Point start,
                     Point end)
{
    auto comp = [](const std::pair<int, Point>& a,
                   const std::pair<int, Point>& b)
    {
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, Point>,
                        std::vector<std::pair<int, Point>>,
                        decltype(comp)> openSet(comp);

    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::unordered_map<Point, int, PointHash> gScore;

    openSet.push({0, start});
    gScore[start] = 0;

    std::vector<IterationData> iterData;

    while (!openSet.empty())
    {
        IterationData it;

        Point current = openSet.top().second;
        openSet.pop();
        it.currentPoint = current;

        if (current == end)
        {
            return std::make_pair(reconstructPath(cameFrom, current),
                                  iterData);
        }

        for (const Point& n : getNeighbors(current))
        {
            int tentativeGScore = gScore[current] + 1;

            if (gScore.find(n) == gScore.end() ||
                tentativeGScore < gScore[n])
            {
                cameFrom[n] = current;
                gScore[n] = tentativeGScore;
                int fScore = tentativeGScore + calculateHeuristic(n, end);
                openSet.push({fScore, n});
                it.neighbors[n] = fScore;
            }
        }

        it.path = reconstructPath(cameFrom, current);

        iterData.push_back(it);
    }

    return std::make_pair(std::vector<Point>(),
                          std::vector<IterationData>());
}

void PathFinder::printPath(const std::vector<Point> &path) const
{
    auto height = m_maze.getHeight();
    auto width = m_maze.getWidth();
    const auto& grid = m_maze.getGrid();
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

const Maze& PathFinder::getMaze() const
{
    return m_maze;
}

int PathFinder::calculateHeuristic(const Point &a,
                                   const Point &b) const
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point> PathFinder::getNeighbors(const Point &p) const
{
    std::vector<Point> neighbors;

    for (uint8_t i = 0; i < 4; i++)
    {
        auto nx = p.x + Maze::dx[i];
        auto ny = p.y + Maze::dy[i];

        if (m_maze.isValidCell(nx, ny) &&
            !m_maze.isWall(nx, ny))
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
