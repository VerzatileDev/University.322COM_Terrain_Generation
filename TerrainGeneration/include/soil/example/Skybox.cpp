#include "Skybox.h"

void Skybox::Setup()
{

	CreateShader("Shaders/SkyboxVertexShader.glsl", "Shaders/SkyboxFragmentShader.glsl");
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &buffer);
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	SetVAOVBO();
	SetCubeMap();

}

void Skybox::Close()
{

}

void Skybox::Update()
{

}

void Skybox::Bind() {

}

void Skybox::Draw()
{
	glDepthFunc(GL_LEQUAL);
	glUseProgram(programID);
	glBindVertexArray(vao);
	unsigned int loc = glGetUniformLocation(programID, "skyboxTexture");
	glUniform1i(glGetUniformLocation(programID, "skyboxTexture"), myTextureIDs[0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	//glm::rotate()
}

void Skybox::SetVAOVBO()
{
	glUseProgram(programID);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &buffer);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Skybox::SetCubeMap()
{
	std::string cubemapTexs[] = {
		"Textures/SkyBox/SkyBoxRight.jpg",
		"Textures/SkyBox/SkyBoxLeft.jpg",
		"Textures/SkyBox/SkyBoxTop.jpg",
		"Textures/SkyBox/SkyBoxBottom.jpg",
		"Textures/SkyBox/SkyBoxFront.jpg",
		"Textures/SkyBox/SkyBoxBack.jpg"
	};

	//std::string cubemapTexs[] = {
	//"Textures/Testbox/1.png",
	//"Textures/Testbox/2.png",
	//"Textures/Testbox/3.png",
	//"Textures/Testbox/4.png",
	//"Textures/Testbox/6.png",
	//"Textures/Testbox/5.png"
	//};

	glUseProgram(programID);
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0 + tex);
	myTextureIDs[0] = tex;
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	int width, height;
	for (int i = 0; i < 6; i++) {
		unsigned char* data = SOIL_load_image(cubemapTexs[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		SOIL_free_image_data(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	++tex;
}
