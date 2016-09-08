/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

Sphere sp(0.2, 15, 15);
Shader lightingShader;
Shader lampShader;

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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	sp.init();
	sp.load();

	lightingShader.initialize("Shaders/lightColorShader.vs",
			"Shaders/lightColorShader.fs");
	lampShader.initialize("Shaders/lampShader.vs", "Shaders/lampShader.fs");

}
void GLApplication::applicationLoop() {
	bool processInput = true;

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model;
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(45.0f,
				(GLfloat) WindowManager::screenWidth
						/ (GLfloat) WindowManager::screenHeight, 0.1f, 100.0f);

		lightingShader.turnOn();

		GLint modelLoc = lightingShader.getUniformLocation("model");
		GLint viewLoc = lightingShader.getUniformLocation("view");
		GLint projLoc = lightingShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		GLint objectColorLoc = lightingShader.getUniformLocation("objectColor");
		GLint lightColorLoc = lightingShader.getUniformLocation("lightColor");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);

		sp.render();

		lightingShader.turnOff();

		lampShader.turnOn();

		// Create transformations

		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::translate(model, lightPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		lampShader.turnOff();

		windowManager->swapTheBuffers();
	}
}

void GLApplication::destroy() {
	if (windowManager) {
		delete windowManager;
		windowManager = nullptr;
	}
}
