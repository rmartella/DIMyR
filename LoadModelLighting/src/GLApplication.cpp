/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

Sphere sphere(2.0, 25, 25, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Texture textureDifuse(GL_TEXTURE_2D, "Textures/container2.png");
Texture textureSpecular(GL_TEXTURE_2D, "Textures/container2_specular.png");

GLApplication::GLApplication() :
		windowManager(nullptr), camera(nullptr) {
}

GLApplication::~GLApplication() {
	destroy();
}

void GLApplication::GLMain() {
	initialize();
	applicationLoop();
}

void GLApplication::initialize() {
	if (!windowManager
			|| !windowManager->initialize(800, 700, "Window GLFW", false)) {
		this->destroy();
		exit(-1);
	}

	glViewport(0, 0, WindowManager::screenWidth, WindowManager::screenHeight);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	shader.initialize("Shaders/lightingSpecularMap.vs",
			"Shaders/lightingSpecularMap.fs");
	sphere.init();
	sphere.load();

	textureDifuse.load();
	textureSpecular.load();

	objModel.loadModel("objects/nanosuit/nanosuit.obj");

}
void GLApplication::applicationLoop() {
	bool processInput = true;

	glm::vec3 lightPos(0.0f, 0.0f, 10.0f);

	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.turnOn();

		GLint viewPosLoc = shader.getUniformLocation("viewPos");
		glUniform3f(viewPosLoc, camera->Position.x, camera->Position.y,
				camera->Position.z);

		// Set material properties
		GLint matShineLoc = shader.getUniformLocation("material.shininess");
		glUniform1f(matShineLoc, 32.0f);

		// Set lights properties
		GLint lightAmbientLoc = shader.getUniformLocation("light.ambient");
		GLint lightDiffuseLoc = shader.getUniformLocation("light.diffuse");
		GLint lightSpecularLoc = shader.getUniformLocation("light.specular");
		GLint lightPosLoc = shader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		// Transformation matrices
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(45.0f,
				(GLfloat) WindowManager::screenWidth
						/ (GLfloat) WindowManager::screenHeight, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = shader.getUniformLocation("model");
		GLint viewLoc = shader.getUniformLocation("view");
		GLint projLoc = shader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		objModel.render(&shader);
		shader.turnOff();

		windowManager->swapTheBuffers();
	}
}

void GLApplication::destroy() {
	if (windowManager) {
		delete windowManager;
		windowManager = nullptr;
	}

	objModel.destroy();

}
