#ifndef PRIMMAZE_H
#define PRIMMAZE_H

#include "maze.h"

class PrimMaze : public Maze
{
    struct Wall
    {
        int x, y;       // Wall coordinates
        int nx, ny;     // Next cell coordinates

        Wall(int x_, int y_, int nx_, int ny_)
            : x(x_), y(y_), nx(nx_), ny(ny_) {}

        // Optional: Add operator for priority queue if you want to randomize selection
        bool operator<(const Wall& other) const
        {
            // Random comparison to randomize wall selection
            return (x + y) < (other.x + other.y);
        }
    };

public:
    PrimMaze(uint32_t width = 21,
             uint32_t height = 21);

    virtual void generate(uint32_t seed) override;

private:
    // Helper method to add walls around an unvisited cell
    void addWalls(std::vector<Wall>& walls, int x, int y);
};

#endif // PRIMMAZE_H
