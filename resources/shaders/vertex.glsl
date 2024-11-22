#version 460 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in float a_ModelIndex;

out vec3 v_VertexColor;

uniform mat4 u_Models[1000];

void main()
{
    int modelIndex = int(a_ModelIndex);
    gl_Position = u_Models[modelIndex] * vec4(a_Pos, 1.0);
    v_VertexColor = a_Color;
}
