#include "dfspathfinder.h"

#include <stack>
#include <algorithm>

DFSPathFinder::DFSPathFinder() {}

std::pair<std::vector<Point>, std::vector<IterationData>>
DFSPathFinder::findPath(Point start,
                        Point end)
{
    std::stack<Point> stack;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::vector<IterationData> iterData;

    // Start DFS from the start point
    stack.push(start);
    cameFrom[start] = start; // Mark start as its own parent

    while (!stack.empty())
    {
        IterationData it;
        Point current = stack.top();
        stack.pop();
        it.currentPoint = current;

        // If we reached the end point, reconstruct and return the path
        if (current == end)
        {
            return std::make_pair(reconstructPath(cameFrom, current, start),
                                  iterData);
        }

        // Get all valid neighbors
        for (const Point& neighbor : getNeighbors(current))
        {
            // If we haven't visited this neighbor yet
            if (!isVisited(cameFrom, neighbor))
            {
                stack.push(neighbor);
                cameFrom[neighbor] = current;
                it.neighbors[neighbor] = 1; // In DFS, all edges have the same weight
            }
        }

        // Record the current state of the path for visualization
        it.path = reconstructPath(cameFrom, current, start);
        iterData.push_back(it);
    }

    // If no path is found, return empty vectors
    return std::make_pair(std::vector<Point>(),
                          std::vector<IterationData>());
}

bool DFSPathFinder::isVisited(const std::unordered_map<Point, Point,PointHash> &visited,
                              const Point &point) const
{
    return visited.find(point) != visited.end();
}

std::vector<Point> DFSPathFinder::reconstructPath(const std::unordered_map<Point, Point, PointHash> &cameFrom, Point current, Point start) const
{
    std::vector<Point> path;
    path.push_back(current);

    // Reconstruct path until we reach the start point
    while (current != start && cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom.at(current);
        path.push_back(current);
    }

    // Only include the path if we successfully reached the start point
    if (current == start)
    {
        std::reverse(path.begin(), path.end());
        return path;
    }

    // If we couldn't reach the start point, return an empty path
    return std::vector<Point>();
}
