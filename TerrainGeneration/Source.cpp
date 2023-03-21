#include <iostream>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLEWLIB/GL/glew.h>
#include <freeglut/GL/freeglut.h>
#include <vector>
//#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 

#include "getbmp.h" // Load Images Data
#include "KeyBoardFunctions.h"

using namespace std;
using namespace glm;

#pragma region PROPERTIES

// Size of the terrain STEP 2
const int MAP_SIZE = 33;

// Window Properties.
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int SCREEN_PLACEMENT_X = 100;
const int SCREEN_PLACEMENT_Y = 300;

static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0); 
static mat4 modelViewMat = mat4(1.0);
static mat4 projMat = mat4(1.0);
static mat3 normalMat = mat3(1.0);  ///create normal matrix

struct Vertex
{
	glm::vec4 coords;
	glm::vec3 normals;
	glm::vec2 texcoords;
};

struct Matrix4x4
{
	float entries[16];
};

struct Material
{
	glm::vec4 ambRefl;
	glm::vec4 difRefl;
	glm::vec4 specRefl;
	glm::vec4 emitCols;
	float shininess;
};

struct Light
{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};


static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};

// Front and back material properties.
static const Material terrainFandB =
{
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0f
};


static const Light light0 =
{
	vec4(0.0, 0.0, 0.0, 1.0), // Ambient
	vec4(1.0, 1.0, 1.0, 1.0), // specular
	vec4(1.0, 1.0, 1.0, 1.0), // diffuse
	vec4(1.0, 1.0, 0.0, 0.0) // direction
};

#pragma endregion

static enum buffer { TERRAIN_VERTICES };
static enum object { TERRAIN };

// Globals
static Vertex terrainVertices[MAP_SIZE * MAP_SIZE] = {};
const int numStripsRequired = MAP_SIZE - 1; // Step size ?
const int verticesPerStrip = 2 * MAP_SIZE;
unsigned int terrainIndexData[numStripsRequired][verticesPerStrip];



static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
normalMatLoc,
projMatLoc,
buffer[2],     // VBO
vao[2];       // VAO


static BitMapFile* image[3];
static unsigned int
texture[3], // Array of texture ids.
grassTexLoc,
sandTexLoc,
snowTexLoc,
objectLoc;

// CAMERA PROPRTIES 
float d = 0.0; //Camera position sideways
float w = 0.0; //Camera position up down
float s = 0.0; //Camera position up down

//float camPosX = 0.0f;
//float camPosY = 10.0f;
//float camPosZ = 15.0f;


static vec3 eye = vec3(0.0, 10.0, 15.0);
static vec3 cen = vec3(0.0, 10.0, 0.0);
float zVal = 0; // Z Co-ordinates of the ball.
float xVal = 0; // X Co-ordinates of the hover.

#pragma endregion


// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

// Step 5. Session 7, Shader error checking.
void shaderCompileTest(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;
}

// Initialization routine.
void setup(void)
{
	float h1, h2, h3, h4, avarage, h;
	
	///Generate random numbers ///
	srand(3); //Seed
	h1 = (rand() % 10) / 5.0 - 1.0;
	h2 = (rand() % 10) / 5.0 - 1.0;
	h3 = (rand() % 10) / 5.0 - 1.0;
	h4 = (rand() % 10) / 5.0 - 1.0;


	// Initialise terrain - set values in the height map to 0
	// Height Values Initialized to zero
	// Element  (x, z) of array represents height Y of terrain given point.
	// Data stored in multi Array Terrain[x.. 
	float terrain[MAP_SIZE][MAP_SIZE] = {};
	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			terrain[x][z] = 0;
		}
	}
	// Diamond Square
	terrain[0][0] = h1 * 10.0;
	terrain[MAP_SIZE - 1][0] = h2 * 10.0;
	terrain[MAP_SIZE - 1][MAP_SIZE - 1] = h3 * 10.0;
	terrain[0][MAP_SIZE - 1] = h4 * 10.0;
	int step_size, tt, H, count;
	float rand_max;
	tt = MAP_SIZE;
	step_size = tt - 1;
	H = 1;
	rand_max = 1.0;

	while (step_size > 1)
	{
		for (int x = 0; x < MAP_SIZE - 1; x += step_size)
			for (int y = 0; y < MAP_SIZE - 1; y += step_size)
			{
				//diamond_step(x, y, stepsize)
				h1 = terrain[x][y];
				h2 = terrain[x + step_size][y];
				h3 = terrain[x][y + step_size];
				h4 = terrain[x + step_size][y + step_size];
				avarage = (h1 + h2 + h3 + h4) / 4.0;
				h = (rand() % 10) / 5.0 - 1.0;
				avarage = avarage + h * 10.0 * rand_max;
				terrain[x + step_size / 2][y + step_size / 2] = avarage;
			}

		for (int x = 0; x < MAP_SIZE - 1; x += step_size)
			for (int y = 0; y < MAP_SIZE - 1; y += step_size)
			{
				//square_step(x, y)
				count = 0;
				h1 = terrain[x][y];  count++;
				h2 = terrain[x][y + step_size];  count++;
				if ((x - step_size / 2) >= 0) { h3 = terrain[x - step_size / 2][y + step_size / 2]; count++; }
				else { h3 = 0.0; }
				if ((x + step_size / 2) < MAP_SIZE) { h4 = terrain[x + step_size / 2][y + step_size / 2]; count++; }
				else { h4 = 0.0; }
				avarage = (h1 + h2 + h3 + h4) / (float)count;
				h = (rand() % 10) / 5.0 - 1.0;
				avarage = avarage + h * 10.0 * rand_max;
				terrain[x][y + step_size / 2] = avarage;

				//second one
				count = 0;
				h1 = terrain[x][y];  count++;
				h2 = terrain[x + step_size][y];  count++;
				if ((y - step_size / 2) >= 0) { h3 = terrain[x + step_size / 2][y - step_size / 2]; count++; }
				else { h3 = 0.0; }
				if ((y + step_size / 2) < MAP_SIZE) { h4 = terrain[x + step_size / 2][y + step_size / 2]; count++; }
				else { h4 = 0.0; }
				avarage = (h1 + h2 + h3 + h4) / (float)count;
				h = (rand() % 10) / 5.0 - 1.0;
				avarage = avarage + h * 10.0 * rand_max;
				terrain[x + step_size / 2][y] = avarage;

				//third one
				count = 0;
				h1 = terrain[x + step_size][y];  count++;
				h2 = terrain[x + step_size][y + step_size];  count++;
				h3 = terrain[x + step_size / 2][y + step_size / 2]; count++;
				if ((x + 3 * step_size / 2) < MAP_SIZE) { h4 = terrain[x + 3 * step_size / 2][y + step_size / 2]; count++; }
				else { h4 = 0.0; }
				avarage = (h1 + h2 + h3 + h4) / (float)count;
				h = (rand() % 10) / 5.0 - 1.0;
				avarage = avarage + h * 10.0 * rand_max;
				terrain[x + step_size][y + step_size / 2] = avarage;

				//fourth one
				count = 0;
				h1 = terrain[x][y + step_size];  count++;
				h2 = terrain[x + step_size][y + step_size];  count++;
				h3 = terrain[x + step_size / 2][y + step_size / 2]; count++;
				if ((y + 3 * step_size / 2) < MAP_SIZE) { h4 = terrain[x + step_size / 2][y + 3 * step_size / 2]; count++; }
				else { h4 = 0.0; }
				avarage = (h1 + h2 + h3 + h4) / (float)count;
				h = (rand() % 10) / 5.0 - 1.0;
				avarage = avarage + h * 10.0 * rand_max;
				terrain[x + step_size / 2][y + step_size] = avarage;
			}

		rand_max = rand_max * pow(2, -H);
		step_size = step_size / 2;
	}


	// Opengl expect a single arrayof Vertex Data Initialization
	// Vertex Buffer Object (TerrainVertices).
	// Vertices of Height map are added to a single dim Array in order 1- 15.
	int i = 0;
	for (int z = 0; z < MAP_SIZE; z++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			terrainVertices[i].coords.x = (float)x;
			terrainVertices[i].coords.y = terrain[x][z];
			terrainVertices[i].coords.z = (float)z;
			terrainVertices[i].coords.w = 1.0;

			terrainVertices[i].normals.x = 0.0;
			terrainVertices[i].normals.y = 0.0;
			terrainVertices[i].normals.z = 0.0;

			i++;
		}
	}

	// OpenGL need to know how to connect the vertices
	// Connect them as a triangle strip and Build index buffer data for each row of data.
	// Data is stored in Array  (TerrainIndexData) and the connections illustrated below.
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
	}

	///compute normal vectors for each vertices
	int index1, index2, index3;
	float dot_value;
	vec3 Pt1, Pt2, Pt3, ttVec, edgeVec1, edgeVec2, norVec, upvec;
	upvec.x = 0.0; upvec.y = 1.0; upvec.z = 0.0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		for (int x = 0; x < (MAP_SIZE * 2 - 2); x++)
		{
			index1 = terrainIndexData[z][x];
			index2 = terrainIndexData[z][x + 1];
			index3 = terrainIndexData[z][x + 2];

			Pt1.x = terrainVertices[index1].coords.x;
			Pt1.y = terrainVertices[index1].coords.y;
			Pt1.z = terrainVertices[index1].coords.z;

			Pt2.x = terrainVertices[index2].coords.x;
			Pt2.y = terrainVertices[index2].coords.y;
			Pt2.z = terrainVertices[index2].coords.z;

			Pt3.x = terrainVertices[index3].coords.x;
			Pt3.y = terrainVertices[index3].coords.y;
			Pt3.z = terrainVertices[index3].coords.z;

			edgeVec1 = Pt2 - Pt1;
			edgeVec2 = Pt3 - Pt1;
			if (x % 2 == 1)
				ttVec = cross(edgeVec2, edgeVec1);
			else
				ttVec = cross(edgeVec1, edgeVec2);
			//norVec = normalize(ttVec);
			dot_value = dot(ttVec, upvec);
			if (dot_value < 0.0000001)
				norVec = -ttVec;
			else
				norVec = ttVec;

			terrainVertices[index1].normals = norVec + terrainVertices[index1].normals;
			terrainVertices[index2].normals = norVec + terrainVertices[index2].normals;
			terrainVertices[index3].normals = norVec + terrainVertices[index3].normals;
		}
	}

	///smooth the normal vectors 
	//vector<vec3> ttVecArr;
	int total;
	total = MAP_SIZE * MAP_SIZE;
	for (i = 0; i < (total - 1); i++)
	{
		ttVec = terrainVertices[i].normals;
		norVec = normalize(ttVec);
		terrainVertices[i].normals = norVec;
	}

	// generate Texture co-ordinates
	// Calculate Scale
	float fTextureS = float(MAP_SIZE) * 3.05f;
	float fTextureT = float(MAP_SIZE) * 3.05f;
	i = 0;
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			//terrainVertices[i].coords = { (float)x, terrain[x][y], (float)y,1.0 }; // No idea what this was

			// normalize
			float fScaleC = float(x) / float(MAP_SIZE - 1); // Devided by mapSize to become normalized
			float fScaleR = float(x) / float(MAP_SIZE - 1);
			terrainVertices[i].texcoords = glm::vec2(fTextureS * fScaleC, fTextureT * fScaleR); // send to texturecoordinates
			i++;
		}
	}

	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("./resource/vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);
	// Test for vertex shader compilation errors
	std::cout << "VERTEX::" << std::endl;
	shaderCompileTest(vertexShaderId); // Error Testing Will Display if there are issues

	char* fragmentShader = readTextFile("./resource/fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	// Test for vertex shader compilation errors
	std::cout << "FRAGMENT::" << std::endl;
	shaderCompileTest(vertexShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	///////////////////////////////////////

	////Add for workshop 7
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.ambRefl"), 1,
		&terrainFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.difRefl"), 1,
		&terrainFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.specRefl"), 1,
		&terrainFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.emitCols"), 1,
		&terrainFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(programId, "terrainFandB.shininess"),
		terrainFandB.shininess);

	glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

	glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1,
		&light0.ambCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1,
		&light0.difCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1,
		&light0.specCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1,
		&light0.coords[0]);


	// LOAD IMAGE 1

	image[0] = getbmp("./Textures/grass.bmp");									// Load image file Data
	image[1] = getbmp("./Textures/sand.bmp");									// Load image file Data
	image[2] = getbmp("./Textures/snow.bmp");									// Load image file Data
	glGenTextures(3, texture);													// Create texture id.


	glActiveTexture(GL_TEXTURE0); 												// Bind grass image.
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);								// Convert Data of the image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Parameters for MipMaps and Generate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			// Image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	grassTexLoc = glGetUniformLocation(programId, "grassTex");					
	glUniform1i(grassTexLoc, 0);												// send to shader
	

	// IMAGE 2
	glActiveTexture(GL_TEXTURE1);												// Bind grass image.
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);								// Convert Data of the image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Parameters for MipMaps and Generate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			// Image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	sandTexLoc = glGetUniformLocation(programId, "sandTex");
	glUniform1i(sandTexLoc, 1);													// Send to Shader


	// IMAGE 3
	glActiveTexture(GL_TEXTURE2);												// Bind grass image.
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);								// Convert Data of the image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Parameters for MipMaps and Generate
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			// Image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	snowTexLoc = glGetUniformLocation(programId, "snowTex");
	glUniform1i(snowTexLoc, 2);													// Send to Shader



	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate data with vertex shader.
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	glBindVertexArray(vao[TERRAIN]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TERRAIN_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]),
		(GLvoid*)(sizeof(terrainVertices[0].coords) + sizeof(terrainVertices[0].normals)));
	glEnableVertexAttribArray(2);


	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId, "projMat");
	projMat = perspective(radians(60.0), (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 0.1, 100.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	///////////////////////////////////////

	// model view matrix
	mat4 modelViewMat = mat4(1.0);
	modelViewMat = lookAt(eye, cen, vec3(0.0, 1.0, 0.0));
	modelViewMat = translate(modelViewMat, vec3(-15.0f, -1.0f, -40.0f)); // <-- TERRAIN IN VIEW.  Translate updates camera location
	

	// Projection Matrix
	modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

	// Set Uniform Variable
	normalMatLoc = glGetUniformLocation(programId, "normalMat");
	normalMat = transpose(inverse(mat3(modelViewMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));

	objectLoc = glGetUniformLocation(programId, "object");  //uniform uint object;

	// Apparently with Instructions this Increases Efficiency of Cull back faces of the Terrain are not rendered.. (I have no idea what that means, but I will pretend that I do).
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//GLenum error = glGetError();
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Calculate the new camera position
	glm::mat4 viewMatrix = glm::lookAt(eye, cen, glm::vec3(0.0f, 1.0f, 0.0f)); // Position of the camera, Camera looking at, Up vector (Defines which point is up down as I understand it)
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-15.0f, -1.0f, -40.0f));

	// Set the model-view matrix
	glm::mat4 modelViewMat = viewMatrix;
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, glm::value_ptr(modelViewMat));

	// Calculate and set the normal matrix uniform variable
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

	// For each row - draw the triangle strip
	glUniform1ui(objectLoc, TERRAIN); // send object ID to the shader if object == Terrain
	glBindVertexArray(vao[TERRAIN]);
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
	}

	


	glFlush();
}

void animate() 
{

	
	cen = vec3(xVal + d, 0 + w, zVal);
	eye = vec3(xVal, 7.0, zVal + 15.0);

	glutPostRedisplay(); // Keeps the window active
}

void timerFunc(int value) {
	animate();
	glutTimerFunc(16, timerFunc, 0); // call the timer function again after 16 milliseconds
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void KeyInputDown(unsigned char key, int x, int y)
{
	KeyBoardFunctions::keys[key] = true;
	//std::cout << "Key pressed: " << key << " : " << KeyBoardFunctions::keys[key] << std::endl;

	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		zVal -= 0.5f;
		break;
	case 's':
		zVal += 0.5f;
		break;
	case 'a':
		xVal -= 0.5f;
		break;
	case 'd':
		xVal += 0.5f;
		break;
	
	default:
		break;
	}
}

void KeyInputUp(unsigned char key, int x, int y)
{
	KeyBoardFunctions::keys[key] = false;
	//std::cout << "Key de-pressed: " << key << " : " << KeyBoardFunctions::keys[key] << std::endl;
}

void SpecialKeyInputDown(int key, int x, int y)
{
	KeyBoardFunctions::specialKeys[key] = true;
	//std::cout << "Special key pressed: " << key << " : " << KeyBoardFunctions::specialKeys[key] << std::endl;

	// CAMERA LOOK AT FUNCTIONS
	if (key == GLUT_KEY_LEFT)
	{
		if (d > -50.0) d -= 0.1;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		if (d < 15.0) d += 0.1;
	}

	if (key == GLUT_KEY_UP)
	{
		if (w  < 50.0) w += 0.1;
	}
	if (key == GLUT_KEY_DOWN)
	{
		if (w > -15.0) w -= 0.1;
	}


	glutPostRedisplay();
}


void SpecialKeyInputUp(int key, int x, int y)
{
	KeyBoardFunctions::specialKeys[key] = false;
	//std::cout << "Special key de-pressed: " << key << " : " << KeyBoardFunctions::specialKeys[key] << std::endl;
}

// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(SCREEN_PLACEMENT_X, SCREEN_PLACEMENT_Y);
	glutCreateWindow("Terrain Generation Application Task 2");
	glEnable(GL_DEPTH_TEST);

	// Set OpenGL to render in wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);

	glutTimerFunc(0, timerFunc, 0); // start the timer function
	glutIdleFunc(animate); /// KEEPS WINDOW ACTIVE
	glutKeyboardFunc(KeyInputDown);
	glutKeyboardUpFunc(KeyInputUp);
	glutSpecialFunc(SpecialKeyInputDown);
	glutSpecialUpFunc(SpecialKeyInputUp);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}