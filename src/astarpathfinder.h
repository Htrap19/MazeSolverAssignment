#ifndef ASTARPATHFINDER_H
#define ASTARPATHFINDER_H

#include "pathfinder.h"

class AStarPathFinder : public PathFinder
{
public:
    AStarPathFinder();

    virtual
    std::pair<std::vector<Point>, std::vector<IterationData>>
    findPath(Point start, Point end) override;

private:
    int calculateHeuristic(const Point& a, const Point& b) const;
};

#endif // ASTARPATHFINDER_H
