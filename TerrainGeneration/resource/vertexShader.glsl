#version 420 core

layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec4 terrainColors;

uniform mat4 projMat;
uniform mat4 modelViewMat;

flat out vec4 colorsExport;

void main(void)
{
   gl_Position = projMat * modelViewMat * terrainCoords;
   colorsExport = terrainColors;
}