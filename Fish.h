#pragma once
// Windows includes (For Time, IO, etc.)
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
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

#include "Mesh.h"


class Fish {
public:
	Fish() { }
	Mesh fish;
	Mesh fishTail;
	Mesh fishFinR;
	Mesh fishFinL;
	Mesh trashDanger[30];

	int N;
	//--------------------
	//Boid Stuff
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	float perceptionRadius;
	float maxForce;
	float maxSpeed;
	bool isDead;

	void create(Mesh main, Mesh finL, Mesh finR, Mesh tail, int n, Mesh trash[20]);
	void setInitPosition();
	void display(int matrix_location, float delta, Fish boids[]);

	void boidUpdate();
	void boidSteer();
	void boidClip();
	void boidFlock(Fish boids[]);
	vec3 boidAlign(Fish boids[]);
	vec3 boidSeperation(Fish boids[]);
	vec3 boidCohesion(Fish boids[]);
	void collisionWithTrash();

	vec3 limitForce(vec3 force, float maxForce);
};

