#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"

#include "Hoop.h";
#include "Bead.h"

#include "Skybox.h"

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0, uniformFarPlane = 0;

Window mainWindow;
Mesh *pivotCubeMesh;

std::vector<Shader> shaderList;
Shader directionalShadowShader;

Camera camera;

Texture beadTexture;
Texture hoopTexture;
Texture cubeTexture;

Material material;

Hoop *hoop;
Bead *bead;

DirectionalLight mainLight;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObjects()
{
	unsigned int pivotCubeIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		1, 0, 4,
		5, 1, 4,
		2, 1, 5,
		2, 5, 6,
		3, 2, 7,
		2, 6, 7,
		5, 4, 6,
		6, 4, 7,
		0, 3, 4,
		4, 3, 7
	};

	GLfloat pivotCubeVertices[] =
	{
		0, 1, 1, 0, 0, 1, 1, 1,
		1, 1, 1, 0, 0, 1, 1, 1,
		1, 1, 0, 0, 0, 1, 1, 1,
		0, 1, 0, 0, 0, 1, 1, 1,
		0, 0, 1, 0, 0, 1, 1, 1,
		1, 0, 1, 0, 0, 1, 1, 1,
		1, 0, 0, 0, 0, 1, 1, 1,
		0, 0, 0, 0, 0, 1, 1, 1
	};

	pivotCubeMesh = new Mesh();
	pivotCubeMesh->CreateMesh(pivotCubeVertices, pivotCubeIndices, 64, 36);

	hoop = new Hoop(60, 0.95f, 0.05f, glm::vec3(0.0f, 5.0f, 0.0f));
	bead = new Bead(15, 15, 0.15f);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);
	model = glm::translate(model, hoop->pivotPoint);
	model = glm::rotate(model, hoop->angle * toRadians, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(0.0f, -(hoop->innerRadius + hoop->thickness), 0.0f));
	hoop->center = glm::vec3(model[3]);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	hoopTexture.UseTexture();
	material.UseMaterial(uniformSpecularIntensity, uniformShininess);
	hoop->mesh->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, hoop->center);
	model = glm::rotate(model, bead->angle * toRadians, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(0.0f, -(hoop->innerRadius + hoop->thickness), 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	beadTexture.UseTexture();
	bead->mesh->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, hoop->pivotPoint + glm::vec3(-0.1f, -0.1f, -0.1f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	cubeTexture.UseTexture();
	pivotCubeMesh->RenderMesh();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.getShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	shaderList[0].Validate();

	RenderScene();
}

int main()
{
	mainWindow = Window(1366, 768); 
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 4.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	beadTexture = Texture("Textures/beadTexture.jpg");
	beadTexture.LoadTexture();
	hoopTexture = Texture("Textures/hoopTexture.jpg");
	hoopTexture.LoadTexture();
	cubeTexture = Texture("Textures/cubeTexture.jpg");
	cubeTexture.LoadTexture();

	material = Material(0.3f, 4);

	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.75f, 0.25f,
		0.0f, 0.0f, -1.0f);

	std::vector<std::string> skyboxFaces;

	skyboxFaces.push_back("Textures/Skybox/Left_Tex.png");
	skyboxFaces.push_back("Textures/Skybox/Right_Tex.png");
	skyboxFaces.push_back("Textures/Skybox/Up_Tex.png");
	skyboxFaces.push_back("Textures/Skybox/Down_Tex.png");
	skyboxFaces.push_back("Textures/Skybox/Front_Tex.png");
	skyboxFaces.push_back("Textures/Skybox/Back_Tex.png");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); 
		deltaTime = now - lastTime; 
		lastTime = now;

		glfwPollEvents();

		hoop->ControllingHoop(mainWindow.getsKeys(), deltaTime);
		bead->MovementOnHoop(hoop->innerRadius + hoop->thickness, hoop->velocity, deltaTime);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		DirectionalShadowMapPass(&mainLight);
		RenderPass(camera.calculateViewMatrix(), projection);

		mainWindow.swapBuffers();
	}
	return 0;
}