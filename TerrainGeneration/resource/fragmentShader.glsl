#version 430 core

#define TERRAIN 0
#define WATER 1 // ADDED

in vec3 normalExport;
in vec2 texCoordsExport; // Vertex texture coordinates 
in float yValue;

out vec4 colorsExport;

struct Light
{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

float maxRange = 27.0f;
float upRange = 1.0f;
float downRange = -6.0f;

float blendFactor = 0.5f;
float blendRange = 1.0f;


//Lighting
uniform Light light0;
uniform vec4 globAmb;

// Takes from send to shader Textures ( See Load Image Parts 1, 2)
uniform sampler2D grassTex; // Texture Unit 0
uniform sampler2D sandTex;	// Texture Unit 1
uniform sampler2D snowTex;	// Texture Unit 2
uniform sampler2D waterTex;	// Texture Unit 3
uniform uint object;        // Tells shader the object currently being processed

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
vec4 frontAmbDiffExport, fieldTexColor, sandTexColor, snowTexColor, waterTexColor;

void main(void)
{
	fieldTexColor = texture(grassTex, texCoordsExport);
	sandTexColor = texture(sandTex, texCoordsExport); // vec4(1.0, 1.0, 1.0, 1.0) Here to Test The workings of it
	snowTexColor = texture(snowTex, texCoordsExport);
	waterTexColor = texture(waterTex, texCoordsExport);
	vec4 texColor;
	
	if( object == TERRAIN )
	{
		if(yValue < downRange ) // Bottom Half
		{
			texColor = sandTexColor; 
		}
		else if(yValue > downRange - blendRange && yValue < downRange + blendRange) // Blending area
		{ 
			texColor = mix(sandTexColor, fieldTexColor, blendFactor); 
		}
		else if (yValue > downRange && yValue < upRange) // Top Half
		{
			texColor = fieldTexColor; 
		}
		else if (yValue > upRange - blendRange && yValue < upRange + blendRange) // Since we do not have Rock at the moment Avoid this
		{ 
			texColor = mix(fieldTexColor, snowTexColor, blendFactor);
		} // Sand should be rock here.
	
		else if (yValue > upRange && yValue < maxRange) // Top Hill SNow area
		{ 
			texColor = snowTexColor; 
		}
	
		else if( yValue > maxRange - blendRange && yValue < maxRange + blendRange) // Snow on top of hills blending area
		{
			texColor = mix(fieldTexColor, snowTexColor, blendFactor);
		}

		else if(yValue > maxRange) 
		{ 
			texColor = snowTexColor; 
		}

		else  // If no areas fall under the Previous described attach Default
		{
			texColor = fieldTexColor;
		}
		normal = normalize(normalExport);
		lightDirection = normalize(vec3(light0.coords));
		frontAmbDiffExport = max(dot(normal, lightDirection), 0.0f) * texColor;// * (light0.difCols * terrainFanB.difRefl);
		//colorsExport = vec4(vec3(min(frontAmbDiffExport, vec4(1.0))), 1.0) * texColor * frontAmbDiffExport;
		colorsExport = frontAmbDiffExport; // * fieldTexColor;
		
	}

	if(object == WATER)
	{
		colorsExport = waterTexColor;
		colorsExport.a = 0.7f; // ALPHA
	}

}