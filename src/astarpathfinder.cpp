#include "astarpathfinder.h"

#include <queue>

AStarPathFinder::AStarPathFinder() {}

std::pair<std::vector<Point>, std::vector<IterationData>>
AStarPathFinder::findPath(Point start, Point end)
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

int AStarPathFinder::calculateHeuristic(const Point &a,
                                        const Point &b) const
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
