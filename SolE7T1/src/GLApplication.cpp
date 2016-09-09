/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

Shader shader;

GLuint VAO, VBO, EBO;

struct VertexColor {
	glm::vec3 position;
	glm::vec3 color;
};

GLApplication::GLApplication() :
		windowManager(nullptr) {
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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shader.initialize("Shaders/cube.vs", "Shaders/cube.fs");

	VertexColor vertex[24] = {
	// Face front
			{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 0.0) }, { glm::vec3(-1, 1,
					1), glm::vec3(0.0, 1.0, 0.0) }, { glm::vec3(-1, -1, 1),
					glm::vec3(0.0, 0.0, 1.0) }, { glm::vec3(1, -1, 1),
					glm::vec3(1.0, 1.0, 0.0) },
			// Face right
			{ glm::vec3(1, 1, -1), glm::vec3(0.0, 0.0, 1.0) }, { glm::vec3(1, 1,
					1), glm::vec3(1.0, 0.0, 1.0) }, { glm::vec3(1, -1, 1),
					glm::vec3(0.0, 1.0, 0.0) }, { glm::vec3(1, -1, -1),
					glm::vec3(0.0, 1.0, 1.0) },
			// Face back
			{ glm::vec3(-1, 1, -1), glm::vec3(0.0, 1.0, 1.0) }, { glm::vec3(1,
					1, -1), glm::vec3(1.0, 0.0, 1.0) }, { glm::vec3(1, -1, -1),
					glm::vec3(1.0, 0.0, 1.0) }, { glm::vec3(-1, -1, -1),
					glm::vec3(1.0, 0.0, 1.0) },
			//Face Left
			{ glm::vec3(-1, 1, 1), glm::vec3(1.0, 0.0, 0.0) }, { glm::vec3(-1,
					1, -1), glm::vec3(1.0, 1.0, 0.0) }, { glm::vec3(-1, -1, -1),
					glm::vec3(0.0, 0.0, 1.0) }, { glm::vec3(-1, -1, 1),
					glm::vec3(0.0, 1.0, 0.0) },
			// Face up
			{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 1.0) }, { glm::vec3(1, 1,
					-1), glm::vec3(0.0, 0.0, 1.0) }, { glm::vec3(-1, 1, -1),
					glm::vec3(1.0, 1.0, .0) }, { glm::vec3(-1, 1, 1), glm::vec3(
					0.0, 1.0, 1.0) },
			// Face down
			{ glm::vec3(1, -1, 1), glm::vec3(1.0, 0.0, 1.0) }, { glm::vec3(-1,
					-1, 1), glm::vec3(1.0, 1.0, 1.0) }, { glm::vec3(-1, -1, -1),
					glm::vec3(0.0, 1.0, 1.0) }, { glm::vec3(1, -1, -1),
					glm::vec3(1.0, 1.0, 0.0) } };

	GLuint index[36] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11,
			8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22,
			23, 20 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index,
	GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
			(GLvoid*) 0);
	glEnableVertexAttribArray(0);
// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
			(GLvoid*) sizeof(vertex[0].position));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

}

void GLApplication::applicationLoop() {
	bool processInput = true;
	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.turnOn();

		glm::mat4 transform;
		transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5))
				* glm::toMat4(windowManager->inputManager.getRotation());

		shader.setMatrix4(shader.getUniformLocation("transform"), 1, GL_FALSE,
				glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT,
				(GLvoid*) (sizeof(GLuint) * 0));
		glBindVertexArray(0);

		shader.turnOff();

		windowManager->swapTheBuffers();
	}
}

void GLApplication::destroy() {
	if (windowManager) {
		delete windowManager;
		windowManager = nullptr;
	}

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	shader.destroy();
}
