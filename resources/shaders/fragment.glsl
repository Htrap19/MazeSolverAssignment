#version 460 core

in vec3 v_VertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(v_VertexColor, 1.0);
}
