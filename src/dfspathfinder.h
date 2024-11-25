#ifndef DFSPATHFINDER_H
#define DFSPATHFINDER_H

#include "pathfinder.h"

class DFSPathFinder : public PathFinder
{
public:
    DFSPathFinder();

    virtual
    std::pair<std::vector<Point>, std::vector<IterationData>>
    findPath(Point start, Point end) override;

private:
    bool isVisited(const std::unordered_map<Point, Point, PointHash>& visited,
                   const Point& point) const;

    std::vector<Point> reconstructPath(const std::unordered_map<Point, Point, PointHash>& cameFrom,
                                       Point current,
                                       Point start) const;
};

#endif // DFSPATHFINDER_H
