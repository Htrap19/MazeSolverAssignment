#include "mazevisualizerapp.h"

#include <imgui.h>

#include <iostream>
#include <sstream>

MazeVisualizerApp::MazeVisualizerApp()
    : m_maze(51, 51),
    m_finder(m_maze)
{
    m_startPointColor = glm::vec3(0.7f, 0.5f, 0.3f);
    m_endPointColor = glm::vec3(0.5f, 0.3f, 0.5f);
}

void MazeVisualizerApp::onCreate()
{
    m_renderer.init();
    m_maze.generate(time(nullptr));

    m_start = {0, 0};
    m_end = {50, 50};
    m_path = m_finder.findPath(m_start, m_end);
}

void MazeVisualizerApp::onUpdate()
{
    m_renderer.drawMaze(m_maze);
    m_renderer.drawPath(m_path, m_finder);

    const auto& grid = m_maze.getGrid();
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
        m_renderer.drawQuad({xPos, xPos, 0.0},
                            {quadWidth * 0.6f, quadHeight * 0.6f, 0.0f},
                            color);
    }

    // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
    // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

    m_renderer.flush();
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

            m_maze.setSize(mazeWidth, mazeHeight);
        }
    }

    if (currentSize == 5)
    {
        static int width = m_maze.getWidth();
        static int height = m_maze.getHeight();
        if (ImGui::InputInt("Width", &width))
        {
            m_maze.setSize(width, height);
        }

        if (ImGui::InputInt("Height", &height))
        {
            m_maze.setSize(width, height);
        }
    }

    if (ImGui::Button("Randomize"))
        onRandomize();

    ImGui::End();

    ImGui::Begin("Path Finder");
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
    if (ImGui::Button("Clear"))
        onClear();

    ImGui::End();
}

void MazeVisualizerApp::onDestroy()
{

}

void MazeVisualizerApp::onRandomize()
{
    m_maze.generate(time(nullptr));
    onFind();
}

void MazeVisualizerApp::onFind()
{
    m_path = m_finder.findPath(m_start, m_end);
}

void MazeVisualizerApp::onClear()
{
    m_path.clear();
}