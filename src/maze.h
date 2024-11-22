#ifndef MAZE_H
#define MAZE_H

#include <stdint.h>
#include <vector>
#include <random>
#include <map>

static const int N = 1, S = 2, E = 4, W = 8;
static const std::map<int, int> DX = { {E, 1}, {W, -1}, {N, 0}, {S, 0} };
static const std::map<int, int> DY = { {E, 0}, {W, 0}, {N, -1}, {S, 1} };
static const std::map<int, int> OPPOSITE = { {E, W}, {W, E}, {N, S}, {S, N} };

class Maze
{
public:
    Maze(uint32_t width = 21,
         uint32_t height = 21);

    void generate(uint32_t seed = 0);
    void clear();
    bool isValidCell(int cx, int cy);
    bool isWall(int cx, int cy) const;

    const std::vector<std::vector<int>>& getGrid() const;

    void print() const;
private:
    void recursiveBacktracking(int cx, int cy);

private:
    std::vector<std::vector<int>> m_grid;
    uint32_t m_width, m_height;
    std::mt19937 m_rng;
};

#endif // MAZE_H
