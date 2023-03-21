#pragma once

#include <map>
#include <GLEWLIB/GL/glew.h>
#include <freeglut/GL/freeglut.h>
#include <glm/glm.hpp>

class KeyBoardFunctions
{
public:
	KeyBoardFunctions();
	KeyBoardFunctions(glm::vec3 pos);
	~KeyBoardFunctions();

	//3D position of the object
	glm::vec3 position;

	//keyboard input
	static std::map<char, bool> keys;
	static std::map<char, bool> specialKeys;
};