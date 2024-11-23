#ifndef MAZEVISUALIZERAPP_H
#define MAZEVISUALIZERAPP_H

#include "application.h"
#include "renderer.h"
#include "maze.h"

class MazeVisualizerApp : public Application
{
public:
    MazeVisualizerApp();

    virtual void onCreate() override;
    virtual void onUpdate() override;
    virtual void onImGuiUpdate() override;
    virtual void onDestroy() override;

protected:
    void onRandomize();

    void onFind();
    void onClear();

private:
    Renderer m_renderer;
    Maze m_maze;
    PathFinder m_finder;
    std::vector<Point> m_path;
    Point m_start, m_end;
    glm::vec3 m_startPointColor, m_endPointColor;
    std::pair<uint32_t, uint32_t> m_mazeSize;
};

#endif // MAZEVISUALIZERAPP_H
