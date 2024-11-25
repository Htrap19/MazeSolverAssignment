#include "mazevisualizerapp.h"

#include "recursivebacktrackingmaze.h"
#include "kruskalmaze.h"
#include "primmaze.h"

#include <imgui.h>

#include <iostream>
#include <sstream>

MazeVisualizerApp::MazeVisualizerApp()
{
    m_startPointColor = glm::vec3(0.7f, 0.5f, 0.3f);
    m_endPointColor = glm::vec3(0.5f, 0.3f, 0.5f);

    m_mazes[0] = std::make_shared<RecursiveBacktrackingMaze>(51, 51);
    m_mazes[1] = std::make_shared<KruskalMaze>(51, 51);
    m_mazes[2] = std::make_shared<PrimMaze>(51, 51);

    m_maze = m_mazes[0];
    m_finder.setMaze(m_maze);
}

void MazeVisualizerApp::onCreate()
{
    m_renderer.init();
    m_maze->generate(time(nullptr));

    m_start = {0, 0};
    m_end = {50, 50};
    // m_path = m_finder.findPath(m_start, m_end);
}

void MazeVisualizerApp::onUpdate()
{
    m_renderer.drawMaze(*m_maze);
    if (m_showFinalPath)
        m_renderer.drawPath(m_path, m_finder);

    const auto& grid = m_maze->getGrid();
    auto rows = grid.size();
    auto cells = grid[0].size();

    // Calculate quad size based on window dimensions
    float quadWidth = 2.0f / rows;
    float quadHeight = 2.0f / cells;

    for (uint8_t i = 0; i < 2; i++)
    {
        auto point = i == 0 ? m_start : m_end;
        auto color = i == 0 ? m_startPointColor : m_endPointColor;
        float xPos = -1.0f + point.x * quadWidth + quadWidth / 2.0f;
        float yPos = -1.0f + point.y * quadHeight + quadHeight / 2.0f;
        m_renderer.drawQuad({xPos, yPos, 0.0},
                            {quadWidth * 0.6f, quadHeight * 0.6f, 0.0f},
                            color);
    }

    if (!m_iteration.empty() && m_iterate)
    {
        auto& iterData = m_iteration[m_iterIndex];
        auto point = iterData.currentPoint;

        float xCurrPos = -1.0f + point.x * quadWidth + quadWidth / 2.0f;
        float yCurrPos = -1.0f + point.y * quadHeight + quadHeight / 2.0f;
        m_renderer.drawQuad({xCurrPos, yCurrPos, 0.0},
                            {quadWidth, quadHeight, 0.0f},
                            glm::vec3(0.5f, 0.2f, 0.3f));

        m_renderer.drawPath(iterData.path, m_finder);

        for (auto& [n, fScore] : iterData.neighbors)
        {
            float xPos = -1.0f + n.x * quadWidth + quadWidth / 2.0f;
            float yPos = -1.0f + n.y * quadHeight + quadHeight / 2.0f;
            m_renderer.drawQuad({xPos, yPos, 0.0},
                                {quadWidth, quadHeight, 0.0f},
                                glm::vec3(0.2f, 0.5f, 0.3f));
        }
    }

    // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
    // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

    m_renderer.flush();

    if (m_isPlaying && m_iterate)
    {
        static uint32_t playTimeout = 0;
        playTimeout++;

        if (playTimeout >= 2)
        {
            onNext();
            playTimeout = 0;
        }
    }
}

void MazeVisualizerApp::onImGuiUpdate()
{
    ImGui::Begin("Maze");
    static const char* mazeAlgos[] =
    {   "Recursive Backtracking",
        "Kruskal’s algorithm",
        "Prim’s algorithm"
    };
    static int currentAlgo = 0;
    if (ImGui::Combo("Algo", &currentAlgo, mazeAlgos, IM_ARRAYSIZE(mazeAlgos)))
    {
        std::cout << "Selected: " << mazeAlgos[currentAlgo] << std::endl;
        auto prevSelectedMaze = m_maze;
        m_maze = m_mazes[currentAlgo];
        m_maze->setSize(prevSelectedMaze->getWidth(),
                        prevSelectedMaze->getHeight());
        m_finder.setMaze(m_maze);
        onClear();
    }

    static const char* mazeSizes[] =
    {
        "11x11",
        "21x21",
        "31x31",
        "51x51",
        "101x101",
        "Custom"
    };
    static int currentSize = 3;
    if (ImGui::Combo("Size", &currentSize, mazeSizes, IM_ARRAYSIZE(mazeSizes)))
    {
        std::cout << "Selected: " << mazeSizes[currentSize] << std::endl;

        if (currentSize != 5)
        {
            std::vector<std::string> widthXheight;
            std::stringstream ss(mazeSizes[currentSize]);
            std::string token;

            while (std::getline(ss, token, 'x'))
            {
                widthXheight.push_back(token);
            }

            uint32_t mazeWidth = std::stoi(widthXheight[0]);
            uint32_t mazeHeight = std::stoi(widthXheight[1]);

            m_maze->setSize(mazeWidth, mazeHeight);
        }
    }

    if (currentSize == 5)
    {
        static int width = m_maze->getWidth();
        static int height = m_maze->getHeight();
        if (ImGui::InputInt("Width", &width))
        {
            m_maze->setSize(width, height);
        }

        if (ImGui::InputInt("Height", &height))
        {
            m_maze->setSize(width, height);
        }
    }

    if (ImGui::Button("Randomize"))
        onRandomize();

    ImGui::End();

    ImGui::Begin("Path Finder");
    static const char* pathFinderAlgos[] =
        {   "Breadth-First Search (BFS)",
            "Depth-First Search (DFS)",
            "A*"
        };
    static int currentPathFinderAlgo = 2;
    if (ImGui::Combo("Algo", &currentPathFinderAlgo, pathFinderAlgos, IM_ARRAYSIZE(pathFinderAlgos)))
    {
        std::cout << "Selected: " << pathFinderAlgos[currentPathFinderAlgo] << std::endl;
    }
    ImGui::InputInt2("Start Pos", &m_start.x);
    ImGui::SameLine();
    ImGui::ColorEdit4("Start Point Color",
                      &m_startPointColor.x,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_NoLabel);
    ImGui::InputInt2("End Pos", &m_end.x);
    ImGui::SameLine();
    ImGui::ColorEdit4("End Point Color",
                      &m_endPointColor.x,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_NoLabel);
    if (ImGui::Button("Find"))
        onFind();
    ImGui::SameLine();
    ImGui::Checkbox("Show final path", &m_showFinalPath);

    ImGui::SeparatorText("Iteration");
    ImGui::Checkbox("Iterate", &m_iterate);
    if (ImGui::Button("< Prev"))
        onPrev();
    ImGui::SameLine();
    if (ImGui::Button(m_isPlaying ? "Pause" : "Play"))
    {
        if (m_isPlaying)
            onPause();
        else
            onPlay();
    }
    ImGui::SameLine();
    if (ImGui::Button("Next >"))
        onNext();

    // Calculate the position of from grid based on the mouse pos
    // if (m_isViewportFocused)
    // {
    //     ImVec2 windowSize = ImGui::GetWindowSize();  // Get the window size
    //     ImVec2 textSize = ImGui::CalcTextSize("1000x1000");  // Get the text size

    //     // Set the cursor position to the bottom-right corner
    //     ImGui::SetCursorPos(ImVec2(10, windowSize.y - textSize.y - 10));

    //     // Render the text
    //     ImVec2 mousePos = ImGui::GetMousePos();
    //     ImVec2 windowPos = m_viewportPos;
    //     ImVec2 relativeMousePos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

    //     const auto& grid = m_maze.getGrid();
    //     auto rows = grid.size();
    //     auto cells = grid[0].size();

    //     // Calculate quad size based on window dimensions
    //     float quadWidth = 2.0f / rows;
    //     float quadHeight = 2.0f / cells;

    //     float xPos = -1.0f + std::round(relativeMousePos.x) * quadWidth + quadWidth / 2.0f;
    //     float yPos = -1.0f + std::round(relativeMousePos.y) * quadHeight + quadHeight / 2.0f;

    //     ImGui::Text("%1f x %2f", xPos, yPos);
    // }

    ImGui::End();
}

void MazeVisualizerApp::onDestroy()
{

}

void MazeVisualizerApp::onRandomize()
{
    m_maze->generate(time(nullptr));
    m_path.clear();
    m_iteration.clear();
}

void MazeVisualizerApp::onFind()
{
    auto [path, iterData] = m_finder.findPath(m_start, m_end);
    m_path = path;
    m_iteration = iterData;
    m_iterIndex = 0;
}

void MazeVisualizerApp::onClear()
{
    m_path.clear();
}

void MazeVisualizerApp::onPlay()
{
    m_isPlaying = true;
}

void MazeVisualizerApp::onPause()
{
    m_isPlaying = false;
}

void MazeVisualizerApp::onNext()
{
    if (m_iterIndex >= (m_iteration.size() - 1))
        return;

    m_iterIndex++;
}

void MazeVisualizerApp::onPrev()
{
    if (m_iterIndex == 0)
        return;

    m_iterIndex--;
}
