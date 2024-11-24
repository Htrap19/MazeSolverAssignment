#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "maze.h"

#include <unordered_map>

struct Point
{
    int x, y;

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct PointHash
{
    size_t operator()(const Point& p) const
    {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

struct IterationData
{
    Point currentPoint;
    std::vector<Point> path;
    std::unordered_map<Point, int, PointHash> neighbors;
};

class PathFinder
{
public:
    PathFinder(const Maze& m);

    std::pair<std::vector<Point>,
              std::vector<IterationData>> findPath(Point start, Point end);
    void printPath(const std::vector<Point>& path) const;
    const Maze& getMaze() const;

private:
    int calculateHeuristic(const Point& a, const Point& b) const;
    std::vector<Point> getNeighbors(const Point& p) const;
    std::vector<Point> reconstructPath(
        std::unordered_map<Point, Point, PointHash>& cameFrom,
        Point current);

private:
    const Maze& m_maze;
};

#endif // PATHFINDER_H
