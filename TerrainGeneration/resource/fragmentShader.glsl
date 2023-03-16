#version 430 core

in vec3 normalExport;

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
  
struct Material
{
   vec4 ambRefl;
   vec4 difRefl;
   vec4 specRefl;
   vec4 emitCols;
   float shininess;
};
uniform Material terrainFandB;

vec3 normal, lightDirection;
vec4 fAndBDif;

void main(void)
{
   normal = normalize(normalExport);
   lightDirection = normalize(vec3(light0.coords));
   fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols *
   terrainFandB.difRefl); 
   colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0);  
}