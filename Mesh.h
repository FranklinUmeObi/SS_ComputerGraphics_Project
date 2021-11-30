#pragma once
// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.


// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

//My GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"

//Image includes
#include "stb_image.h"

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
} ModelData;
#pragma endregion SimpleTypes


class Mesh {
private:
	ModelData modelData;
	GLuint loc1, loc2, loc3;
	GLuint vao = 0;
	GLuint vertexBufferP = 0;
	GLuint vertexBufferN = 0;
	unsigned int texture;



	void bind();
	ModelData load_mesh(const char* file_name);

public:
	int x;
	int y;
	int z;

	GLuint shaderProgram;
	void create(const char* mesh_name, GLuint shader, const char* textureName);
	void init(const char* file_name, const char* textureName);
	void setShaders(GLuint programID);

	void displayIce(int matrix_location);
	void displayTrash(int matrix_location);

	void setData();
	void render();
	void cleanup();
};