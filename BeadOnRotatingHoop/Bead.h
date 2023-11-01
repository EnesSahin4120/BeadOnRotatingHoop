#pragma once

#include <vector>
#include <GL\glew.h>

#include "Mesh.h"
#include "CommonValues.h"

class Bead
{
public:
	Bead();
	Bead(int sectorCount, int stackCount, float radius);
	void MovementOnHoop(float hoopRadius, float hoopVelocity, GLfloat deltaTime);

	//Physics Parameters
	float angle;
	float acceleration;
	float velocity;
	glm::vec3 pos;

	Mesh* mesh;

	~Bead();

private:
	//Mesh Parameters
	float radius;

	float x, y, z, xy;
	float nx, ny, nz, lengthInv;
	float s, t;

	int k1, k2;

	float sectorStep;
	float stackStep;
	float sectorAngle, stackAngle;

	GLfloat* vertices;
	unsigned int* indices;
	std::vector<GLfloat> verticesList;
	std::vector<unsigned int> indicesList;
};

