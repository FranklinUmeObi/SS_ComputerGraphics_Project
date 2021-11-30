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


class Shader {
private:
	char* readShaderSource(const char* shaderFile);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	
public:
	GLuint shaderProgramId;
	GLuint CompileShader(const char* vertexShader, const char* fragShader);
};