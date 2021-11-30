//I don't own any models used or any images used

//Advanced Features implemented
//1. Complex boids animation
//2. Hierarchical creatures that move realistically in your crowd
//3. Collision detection with trash kills fish
//4. Intelligent Characters ( Fish avoid dead fish and don't 
//	 flock with them )
//5. Partially transparent geometry using alpha-blending (jellyfish light source)

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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//My Classes
#include "Shader.h"
#include "Mesh.h"
#include "Fish.h"
//-----------------------------------------------------

using namespace std;

//MESHS TO LOAD
#define MESH_TERRAIN "models/terrain2.dae"
#define MESH_PIPE "models/pipes.dae"
#define MESH_RIG "models/oilrig.dae"
#define MESH_BULB "models/bulb.dae"
#define MESH_JELLY "models/jelly.dae"
#define MESH_NAME "models/body2.dae"
#define MESH_NAME3 "models/tail.dae"
#define MESH_NAME4 "models/left.dae"
#define MESH_NAME5 "models/right.dae"
#define MESH_ICEBERG "models/iceberg.dae"
#define MESH_TRASH1 "models/trashModels/bag.dae"
#define MESH_TRASH2 "models/trashModels/cup.dae"
#define MESH_TRASH3 "models/trashModels/bottle.dae"
#define MESH_TRASH4 "models/trashModels/straw.dae"


#define TEXTURE_TERRAIN "textures/sand.jpg"
#define TEXTURE_PIPE "textures/pipe.jpg"
#define TEXTURE_RIG "textures/rig.jpg"
#define TEXTURE1 "textures/fish2.jpg"
#define TEXTURE2 "textures/ice.jpg"
#define TEXTURE3 "textures/trash.jpg"
#define TEXTURE4 "textures/cup.jpg"
#define TEXTURE5 "textures/straw.jpg"
#define TEXTURE6 "textures/bag.jpg"

#define NUM_FISH 110
#define NUM_ICE 4
#define NUM_TRASH 30

Mesh fishInit;
Mesh fishTailInit;
Mesh fishFinRInit;
Mesh fishFinLInit;

//Variables
int width = 900;
int height = 500;
float scaleTo = 10.0;
float timeSince = 0.0f;
float camPosX = 0.0f,	camPosY = 0.0f,		camPosZ = 10.0f;
float camLookX = 0.0f,	camLookY = 0.0f,	camLookZ = -1.0;


Mesh ground;
Mesh rig;
Mesh pipe;
Mesh jelly;
Mesh bulb;
Fish allFish[NUM_FISH];
Mesh allIce[NUM_ICE];
Mesh allTrash[NUM_TRASH];


void createMeshsForFish(GLuint fishShader)
{
	fishInit.create(MESH_NAME, fishShader, TEXTURE1);
	fishTailInit.create(MESH_NAME3, fishShader, TEXTURE1);
	fishFinLInit.create(MESH_NAME4, fishShader, TEXTURE1);
	fishFinRInit.create(MESH_NAME5, fishShader, TEXTURE1);
}


void display() {
	//---------------------------
	//Set Up GL Stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//---------------------------


	//---------------------------
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(ground.shaderProgram, "model");
	int view_mat_location = glGetUniformLocation(ground.shaderProgram, "view");
	int proj_mat_location = glGetUniformLocation(ground.shaderProgram, "proj");
	//---------------------------
	glUseProgram(ground.shaderProgram);

	//---------------------------
	//Set Up Camera
	glm::mat4 view;
	glm::vec3 cameraPos = glm::vec3(camPosX, camPosY, camPosZ);
	glm::vec3 cameraFront = glm::vec3(camLookX, camLookY, camLookZ);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	//---------------------------


	//---------------------------
	//Draw Ground Object
	mat4 modelGround = identity_mat4();
	modelGround = translate(modelGround, vec3(0.0, -8.0, 0.0));
	modelGround = scale(modelGround, vec3(scaleTo, scaleTo/2, scaleTo));

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelGround.m);
	ground.render();

	//---------------------------
	//Draw Rig Object
	mat4 modelRig = identity_mat4();
	modelRig = translate(modelRig, vec3(0.0, 20.0, -40.0));
	modelRig = scale(modelRig, vec3(10,10,10));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelRig.m);
	rig.render();
	//---------------------------

	//---------------------------
	//Draw Pipe Object
	mat4 modelPipe = identity_mat4();
	modelPipe = translate(modelPipe, vec3(0.0, -2.0, -20.0));
	modelPipe = scale(modelPipe, vec3(10, 10, 10));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelPipe.m);
	pipe.render();
	//---------------------------
	//Draw Iceberg Objects

	for (int i = 0; i < NUM_ICE; i++) {
		allIce[i].displayIce(matrix_location);
	}
	
	//---------------------------
	//Draw Trash
	for (int i = 0; i < NUM_TRASH; i++)
	{
		allTrash[i].displayTrash(matrix_location);
	}


	//---------------------------
	//Draw Fish Objects
	for (int i = 0; i < NUM_FISH; i++)
	{
		allFish[i].display(matrix_location, timeSince, allFish);
	}
	//---------------------------


	//LightUp Jelly
	glUseProgram(jelly.shaderProgram);
	int matrix_location2 = glGetUniformLocation(jelly.shaderProgram, "model");
	int view_mat_location2 = glGetUniformLocation(jelly.shaderProgram, "view");
	int proj_mat_location2 = glGetUniformLocation(jelly.shaderProgram, "proj");

	mat4 modelJelly = identity_mat4();
	modelJelly = translate(modelJelly, vec3(0,0,-15));
	glUniformMatrix4fv(proj_mat_location2, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location2, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrix_location2, 1, GL_FALSE, modelJelly.m);
	jelly.render();



	//LightUp Sun
	glUseProgram(bulb.shaderProgram);
	int matrix_location3 = glGetUniformLocation(bulb.shaderProgram, "model");
	int view_mat_location3 = glGetUniformLocation(bulb.shaderProgram, "view");
	int proj_mat_location3 = glGetUniformLocation(bulb.shaderProgram, "proj");

	mat4 modelBulb = identity_mat4();

	modelBulb = scale(modelBulb, vec3(0.7, 0.7, 0.7));
	modelBulb = translate(modelBulb, vec3(-100,300,-200));
	glUniformMatrix4fv(proj_mat_location3, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location3, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrix_location3, 1, GL_FALSE, modelBulb.m);
	bulb.render();




	glutSwapBuffers();
}


void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	// Rotate the model slowly around the y axis at 20 degrees per second
	timeSince +=  delta;

	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	Shader PhongShader;
	GLuint shaderPhongSea = PhongShader.CompileShader("PhongVS.txt", "PhongFS.txt");
	GLuint activeShader = shaderPhongSea;

	// load meshs into a vertex buffer array
	ground.create(MESH_TERRAIN, activeShader, TEXTURE_TERRAIN);
	rig.create(MESH_RIG, activeShader, TEXTURE_RIG);
	pipe.create(MESH_PIPE, activeShader, TEXTURE_PIPE);
	
	for (int i = 0; i < NUM_ICE; i++)
	{
		allIce[i].create(MESH_ICEBERG, activeShader, TEXTURE2);
	}

	for (int i = 0; i < NUM_TRASH; i++)
	{
		
		if (i % 4 == 0) allTrash[i].create(MESH_TRASH1, activeShader, TEXTURE6);
		if (i % 4 == 1) allTrash[i].create(MESH_TRASH2, activeShader, TEXTURE4);
		if (i % 4 == 2) allTrash[i].create(MESH_TRASH3, activeShader, TEXTURE3);
		if (i % 4 == 3) allTrash[i].create(MESH_TRASH4, activeShader, TEXTURE5);
	}
	createMeshsForFish(activeShader);
	for (int i = 0; i < NUM_FISH; i++)
	{
		allFish[i].create(fishInit, fishFinLInit, fishFinRInit, fishTailInit, NUM_FISH, allTrash);
	}


	Shader JellyShader;
	GLuint jellyShader = JellyShader.CompileShader("jellyVS.txt", "jellyFS.txt");
	jelly.create(MESH_JELLY, jellyShader, TEXTURE1);

	Shader BulbShader;
	GLuint bulbShader = BulbShader.CompileShader("bulbVS.txt", "bulbFS.txt");

	bulb.create(MESH_BULB, bulbShader, TEXTURE1);
}


void keypress(unsigned char key, int x, int y) {
	float scaleOffset = 1 / scaleTo;
	float offset = (1 * scaleOffset);

	     if (key == 'w') camPosZ = camPosZ - offset ;
		 if (key == 's') camPosZ = camPosZ + offset ;
		 if (key == 'a') camPosX = camPosX - offset ;
		 if (key == 'd') camPosX = camPosX + offset ;
		 if (key == 'q') camPosY = camPosY + offset ;
		 if (key == 'e') camPosY = camPosY - offset ;

}

void mouseMove(int x, int y) {
	float horiazontal = width / 2 - x;
	float vertical = height / 2 - y;
	camLookX = horiazontal * -0.01;
	camLookY = vertical*0.01;
	glm::normalize((camLookX, camLookY, camLookZ));
}


int main(int argc, char** argv) {
	
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("MidTerm Assignment");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);			//KeyboardControls
	glutPassiveMotionFunc(mouseMove);//Mouse Controls
	glutMotionFunc(mouseMove);

	// A call to glewInit() must be done after glut is initialized!
	glewExperimental = true;
	GLenum res = glewInit();
	
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}


