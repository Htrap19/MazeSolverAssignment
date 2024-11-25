#ifndef RECURSIVEBACKTRACKINGMAZE_H
#define RECURSIVEBACKTRACKINGMAZE_H

#include "maze.h"

class RecursiveBacktrackingMaze : public Maze
{
public:
    RecursiveBacktrackingMaze(uint32_t width = 21,
                              uint32_t height = 21);

    virtual void generate(uint32_t seed) override;

private:
    void recursiveBacktracking(int cx, int cy);
};

#endif // RECURSIVEBACKTRACKINGMAZE_H
