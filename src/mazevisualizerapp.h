#ifndef MAZEVISUALIZERAPP_H
#define MAZEVISUALIZERAPP_H

#include "application.h"
#include "renderer.h"
#include "maze.h"

#include <memory>

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
    void onPlay();
    void onPause();
    void onNext();
    void onPrev();

    virtual void onKeyInput(int key,
                            int scancode,
                            int action,
                            int mods) override;

private:
    Renderer m_renderer;
    std::shared_ptr<Maze> m_maze; // Selected maze
    std::shared_ptr<PathFinder> m_finder;
    std::vector<Point> m_path;
    Point m_start, m_end;
    glm::vec3 m_startPointColor, m_endPointColor;
    std::pair<uint32_t, uint32_t> m_mazeSize;

    bool m_isPlaying = false;
    uint32_t m_iterIndex = 0;
    std::vector<IterationData> m_iteration;
    bool m_showFinalPath = true;
    bool m_visualize = false;

    std::array<std::shared_ptr<Maze>, 3> m_mazes;
    std::array<std::shared_ptr<PathFinder>, 3> m_pathFinders;
};

#endif // MAZEVISUALIZERAPP_H
