#ifndef KRUSKALMAZE_H
#define KRUSKALMAZE_H

#include "maze.h"

class KruskalMaze : public Maze
{
    class DisjointSet
    {
    public:
        explicit DisjointSet(int size) : parent(size), rank(size, 0)
        {
            for (int i = 0; i < size; i++)
            {
                parent[i] = i;
            }
        }

        int find(int x)
        {
            if (parent[x] != x)
            {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        void unite(int x, int y)
        {
            int rx = find(x);
            int ry = find(y);

            if (rx == ry) return;

            if (rank[rx] < rank[ry])
            {
                parent[rx] = ry;
            }
            else if (rank[rx] > rank[ry])
            {
                parent[ry] = rx;
            }
            else
            {
                parent[ry] = rx;
                rank[rx]++;
            }
        }

        bool connected(int x, int y)
        {
            return find(x) == find(y);
        }

    private:
        std::vector<int> parent;
        std::vector<int> rank;
    };

    struct Wall
    {
        int x1, y1;  // First cell coordinates
        int x2, y2;  // Second cell coordinates
        int wx, wy;  // Wall coordinates

        Wall(int x1_, int y1_, int x2_, int y2_, int wx_, int wy_)
            : x1(x1_), y1(y1_), x2(x2_), y2(y2_), wx(wx_), wy(wy_) {}
    };

public:
    KruskalMaze(uint32_t width = 21,
                uint32_t height = 21);

    virtual void generate(uint32_t seed) override;
};

#endif // KRUSKALMAZE_H
