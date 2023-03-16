#version 420 core

layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;

struct Material
{
vec4 ambRefl;
vec4 difRefl;
vec4 specRefl;
vec4 emitCols;
float shininess;
};

uniform vec4 globAmb;
uniform mat4 projMat;
uniform mat4 modelViewMat;
uniform mat3 normalMat;

out vec3 normalExport;

void main(void)
{
   normalExport = terrainNormals;
   normalExport = normalize(normalMat * normalExport);
   
   gl_Position = projMat * modelViewMat * terrainCoords;
}