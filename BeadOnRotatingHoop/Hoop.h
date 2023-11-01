#pragma once

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Mesh.h"

#include <GLFW\glfw3.h>

#include "CommonValues.h"

class Hoop
{
public:
	Hoop();
	Hoop(int segmentCount, float innerRadius, float thickness, glm::vec3 pivotPoint);
	void ControllingHoop(bool* keys, GLfloat deltaTime);
	void UpdateHoop(GLfloat deltaTime);

	float innerRadius;
	float thickness;

	//Physics Parameters
	float angle;
	float velocity;
	glm::vec3 pivotPoint;
	glm::vec3 center;

	Mesh* mesh;

	~Hoop();

private:
	float previousAngle;

	int segmentCount;
	glm::vec3 GetUnitVectorByAngle(float angle);

	void SetVertices();
	void SetIndices();

	std::vector<float> verticesList;
	std::vector<unsigned int> indicesList;
	float* vertices;
	unsigned int* indices;
};

