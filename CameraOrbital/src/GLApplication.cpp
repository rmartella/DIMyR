/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

Sphere sphere(1, 15, 15);
Shader shaderColor;

GLuint VBO, VAO, EBO;

struct VertexTexture {
	glm::vec3 m_Pos;
	glm::vec2 m_TexCoord;
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

	sphere.init();
	sphere.load();

	shaderColor.initialize("Shaders/Color.vs", "Shaders/Color.fs");

}

void GLApplication::applicationLoop() {
	bool processInput = true;
	double lastTime = TimeManager::Instance().GetTime();
	glm::vec3 cubePositions[] =
			{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
					glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f,
							-12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(
							-1.7f, 3.0f, -7.5f), glm::vec3(1.3f, -2.0f, -2.5f),
					glm::vec3(1.5f, 2.0f, -2.5f), glm::vec3(1.5f, 0.2f, -1.5f),
					glm::vec3(-1.3f, 1.0f, -1.5f) };
	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		GLfloat timeValue = TimeManager::Instance().GetTime() - lastTime;

		// Create transformations
		glm::mat4 view;
		glm::mat4 projection;

		view = glm::lookAt(windowManager->inputManager.getCameraPos(),
				windowManager->inputManager.getCameraPos()
						+ windowManager->inputManager.getCameraDir(),
				windowManager->inputManager.getCameraUp());
		projection = glm::perspective(45.0f,
				(GLfloat) WindowManager::screenWidth
						/ (GLfloat) WindowManager::screenHeight, 0.1f, 100.0f);

		shaderColor.turnOn();

		// Get their uniform location
		shaderColor.getUniformLocation("model");
		GLint modelLoc = shaderColor.getUniformLocation("model");
		GLint viewLoc = shaderColor.getUniformLocation("view");
		GLint projLoc = shaderColor.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model;
		/*model = glm::rotate(model, (GLfloat) timeValue * 1.0f,
		 glm::vec3(0.0f, 1.0f, 0.0f));*/
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//sphere.render();

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		for (GLuint i = 0; i < 10; i++) {
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT,
					(GLvoid*) (sizeof(GLuint) * 0));
		}

		shaderColor.turnOff();

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	shaderColor.destroy();
}
