#ifndef BFSPATHFINDER_H
#define BFSPATHFINDER_H

#include "pathfinder.h"

class BFSPathFinder : public PathFinder
{
public:
    BFSPathFinder();

    virtual
    std::pair<std::vector<Point>, std::vector<IterationData>>
    findPath(Point start, Point end) override;

private:
    // Helper method to check if a point has been visited
    bool isVisited(const std::unordered_map<Point, Point, PointHash>& visited,
                   const Point& point) const;

    // Override the base class's reconstructPath for BFS-specific path reconstruction
    std::vector<Point> reconstructPath(const std::unordered_map<Point, Point, PointHash>& cameFrom,
                                       Point current,
                                       Point start) const;
};

#endif // BFSPATHFINDER_H
