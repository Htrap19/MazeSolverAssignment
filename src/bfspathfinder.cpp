#include "bfspathfinder.h"

#include <queue>
#include <algorithm>

BFSPathFinder::BFSPathFinder() {}

std::pair<std::vector<Point>, std::vector<IterationData>>
BFSPathFinder::findPath(Point start, Point end)
{
    std::queue<Point> queue;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::vector<IterationData> iterData;

    // Start BFS from the start point
    queue.push(start);
    cameFrom[start] = start; // Mark start as its own parent

    while (!queue.empty())
    {
        IterationData it;
        Point current = queue.front();
        queue.pop();
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
                queue.push(neighbor);
                cameFrom[neighbor] = current;
                it.neighbors[neighbor] = 1; // In BFS, all edges have the same weight
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

bool BFSPathFinder::isVisited(const std::unordered_map<Point, Point, PointHash> &visited,
                              const Point &point) const
{
    return visited.find(point) != visited.end();
}

std::vector<Point> BFSPathFinder::reconstructPath(const std::unordered_map<Point, Point, PointHash> &cameFrom,
                                                  Point current,
                                                  Point start) const
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
    if (current == start) {
        std::reverse(path.begin(), path.end());
        return path;
    }

    // If we couldn't reach the start point, return an empty path
    return std::vector<Point>();
}
