#version 420 core

#define TERRAIN 0
#define WATER 1 // ADDED


layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;
layout(location=2) in vec2 terrainTexCoords;

layout(location=3) in vec4 waterCoords; // WATER Position
layout(location=4) in vec3 waterNormals;
layout(location=5) in vec2 waterTexCoords; // Texture

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
uniform float waveTime; // WATER

uniform uint object;

out vec3 normalExport; //
//out vec3 waternormalExport;
out vec2 texCoordsExport;
out float yValue;

vec4 coords;

void main(void)
{
   normalExport = terrainNormals;
   normalExport = normalize(normalMat * normalExport);

   if(object == TERRAIN)
   {
		yValue = terrainCoords.y;
		texCoordsExport = terrainTexCoords;
		gl_Position = projMat * modelViewMat * terrainCoords;

   }

   else if(object == WATER)
   {
		coords = vec4(waterCoords);
		coords.y += 0.8f * (sin(coords.x + waveTime) + cos(coords.z + waveTime)) + 0.2f;

		texCoordsExport = waterTexCoords;
		gl_Position = projMat * modelViewMat * coords;
   }
   
   else 
   {
		gl_Position = projMat * modelViewMat * terrainCoords;
   }
   
   //gl_Position = projMat * modelViewMat * terrainCoords;
   
}