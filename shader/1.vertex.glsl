#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec4 aColor;

out vec4 ourColor;

uniform float xOffset;
uniform vec3 color;
uniform mat4 transformationMatrix;

void main()
{
//    gl_Position = vec4(aPos.xyz, 1.0f);
    gl_Position = vec4(aPos.xyz, 1.0f)*transformationMatrix;
    ourColor = vec4(color.xyz, 1.0);
}
