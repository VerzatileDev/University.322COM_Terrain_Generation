#version 420 core

#define TERRAIN 0
#define BACKGROUND 1


layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;
layout(location=2) in vec2 terrainTexCoords;

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

uniform uint object;

out vec3 normalExport;
out vec2 texCoordsExport;
out float yValue; // 

void main(void)
{
   normalExport = terrainNormals;
   normalExport = normalize(normalMat * normalExport);

   if(object == TERRAIN)
   {
		yValue = terrainCoords.y; //
		texCoordsExport = terrainTexCoords;
   }
   
   
   
   gl_Position = projMat * modelViewMat * terrainCoords;
   
}