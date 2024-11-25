#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "maze.h"

#include <unordered_map>
#include <memory>

struct Point
{
    int x, y;

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return x != other.x || y != other.y;
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
    PathFinder() = default;
    PathFinder(const std::shared_ptr<Maze>& maze);
    virtual ~PathFinder() = default;

    virtual std::pair<std::vector<Point>,
                      std::vector<IterationData>>
    findPath(Point start, Point end) = 0;
    void printPath(const std::vector<Point>& path) const;
    void setMaze(const std::shared_ptr<Maze>& maze);
    const std::shared_ptr<Maze>& getMaze() const;

protected:
    std::vector<Point> getNeighbors(const Point& p) const;
    std::vector<Point> reconstructPath(
        std::unordered_map<Point, Point, PointHash>& cameFrom,
        Point current);

private:
    std::shared_ptr<Maze> m_maze;
};

#endif // PATHFINDER_H
