#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;

out vec4 colorsExport;

struct Light
{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

uniform Light light0;
uniform vec4 globAmb;
uniform sampler2D grassTex;

struct Material
{
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shininess;

};
uniform Material terrainFanB;

vec3 normal, lightDirection;
vec4 frontAmbDiffExport, fieldTexColor;

void main(void)
{
	normal = normalize(normalExport);
	lightDirection = normalize(vec3(light0.coords));
	frontAmbDiffExport = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFanB.difRefl);
	fieldTexColor = texture(grassTex, texCoordsExport);
	//colorsExport = frontAmbDiffExport * fieldTexColor; // prodcuses darkness no idea how to fix.
	colorsExport = fieldTexColor;
}