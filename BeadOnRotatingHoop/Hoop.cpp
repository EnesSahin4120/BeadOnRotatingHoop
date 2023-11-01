#include "Hoop.h"



Hoop::Hoop()
{
}

Hoop::Hoop(int segmentCount, float innerRadius, float thickness, glm::vec3 pivotPoint)
{
	this->segmentCount = segmentCount;
	this->innerRadius = innerRadius;
	this->thickness = thickness;
	this->pivotPoint = pivotPoint;

	angle = 0.0f;
	previousAngle = 0.0f;

	SetVertices();
	SetIndices();

	vertices = &verticesList[0];
	indices = &indicesList[0];

	mesh = new Mesh();
	mesh->CreateMesh(vertices, indices, verticesList.size(), indicesList.size());
}

void Hoop::SetVertices()
{
	for (int i = 0; i < segmentCount + 1; i++) {
		float t = i / (float)segmentCount;
		float angrad = t * 2.0f * pi;
		glm::vec3 dir = GetUnitVectorByAngle(angrad);

		verticesList.push_back((dir * (innerRadius + thickness)).x);
		verticesList.push_back((dir * (innerRadius + thickness)).y);
		verticesList.push_back((dir * (innerRadius + thickness)).z);

		verticesList.push_back(t);
		verticesList.push_back(1);

		verticesList.push_back(0);
		verticesList.push_back(0);
		verticesList.push_back(1);
		
		verticesList.push_back((dir * innerRadius).x);
		verticesList.push_back((dir * innerRadius).y);
		verticesList.push_back((dir * innerRadius).z);

		verticesList.push_back(t);
		verticesList.push_back(0);

		verticesList.push_back(0);
		verticesList.push_back(0);
		verticesList.push_back(1);
	}
}

void Hoop::SetIndices()
{
	for (int i = 0; i < segmentCount; i++) {
		int indexRoot = i * 2;
		int indexInnerRoot = indexRoot + 1;
		int indexOuterNext = indexRoot + 2;
		int indexInnerNext = indexRoot + 3;

		indicesList.push_back(indexRoot);
		indicesList.push_back(indexOuterNext);
		indicesList.push_back(indexInnerNext);

		indicesList.push_back(indexRoot);
		indicesList.push_back(indexInnerNext);
		indicesList.push_back(indexInnerRoot);
	}
}

glm::vec3 Hoop::GetUnitVectorByAngle(float angle)
{
	return glm::vec3(glm::cos(angle), glm::sin(angle), 0.0f);
}

void Hoop::ControllingHoop(bool * keys, GLfloat deltaTime)
{
	if (keys[GLFW_KEY_RIGHT])
		angle += deltaTime * 125.0f;

	if (keys[GLFW_KEY_LEFT])
		angle -= deltaTime * 125.0f;

	UpdateHoop(deltaTime);
}

void Hoop::UpdateHoop(GLfloat deltaTime)
{
	velocity = (angle - previousAngle) / deltaTime;

	velocity *= 0.03f;
	previousAngle = angle;
}


Hoop::~Hoop()
{
}
