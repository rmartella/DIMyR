/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

Shader shader;

struct VertexTexture {
	glm::vec3 position;
	glm::vec2 texture;
};

GLuint VAO, VBO, EBO, texture;

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

	shader.initialize("Shaders/cube.vs", "Shaders/cube.fs");

	VertexTexture vertex[24] = {
			// Face front
			{ glm::vec3(1, 1, 1), glm::vec2(1, 1) }, { glm::vec3(-1, 1, 1),
					glm::vec2(0, 1) },
			{ glm::vec3(-1, -1, 1), glm::vec2(0, 0) }, { glm::vec3(1, -1, 1),
					glm::vec2(1, 0) },
			// Face right
			{ glm::vec3(1, 1, -1), glm::vec2(1, 1) }, { glm::vec3(1, 1, 1),
					glm::vec2(0, 1) }, { glm::vec3(1, -1, 1), glm::vec2(0, 0) },
			{ glm::vec3(1, -1, -1), glm::vec2(1, 0) },
			// Face back
			{ glm::vec3(-1, 1, -1), glm::vec2(1, 1) }, { glm::vec3(1, 1, -1),
					glm::vec2(0, 1) },
			{ glm::vec3(1, -1, -1), glm::vec2(0, 0) }, { glm::vec3(-1, -1, -1),
					glm::vec2(1, 0) },
			//Face Left
			{ glm::vec3(-1, 1, 1), glm::vec2(1, 1) }, { glm::vec3(-1, 1, -1),
					glm::vec2(0, 1) },
			{ glm::vec3(-1, -1, -1), glm::vec2(0, 0) }, { glm::vec3(-1, -1, 1),
					glm::vec2(1, 0) },
			// Face up
			{ glm::vec3(1, 1, 1), glm::vec2(1, 1) }, { glm::vec3(1, 1, -1),
					glm::vec2(0, 1) },
			{ glm::vec3(-1, 1, -1), glm::vec2(0, 0) }, { glm::vec3(-1, 1, 1),
					glm::vec2(1, 0) },
			// Face down
			{ glm::vec3(1, -1, 1), glm::vec2(1, 1) }, { glm::vec3(-1, -1, 1),
					glm::vec2(0, 1) },
			{ glm::vec3(-1, -1, -1), glm::vec2(0, 0) }, { glm::vec3(1, -1, -1),
					glm::vec2(1, 0) } };

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
			(GLvoid *) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
			(GLvoid*) sizeof(vertex[0].position));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//This for load a texture with FreeImage lib
	// Se obtiene del nombre del archivo a cargar en la GPU como un apuntador a un arreglo de chars
	const char* filename = "Textures/test.png";

	// Determina el formato de la imagen
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	// Si la imagen no es encontrada termina el programa
	if (format == -1) {
		std::cout << "No se encontró la imagen: " << filename << " - Abortando."
				<< std::endl;
		exit(-1);
	}
	// Si el formato no es soportado por FreeImage termina el programa
	if (format == FIF_UNKNOWN) {
		std::cout
				<< "No se puede determinar el formato de imagen - validarla extensión del archivo..."
				<< std::endl;

		// Se obtiene el formato del archivo
		format = FreeImage_GetFIFFromFilename(filename);

		// Revisa si la librería es capaz de leer el formato
		if (!FreeImage_FIFSupportsReading(format)) {
			std::cout << "El formato de la imagen no puede ser leeído!"
					<< std::endl;
			exit(-1);
		}
	}
	// Si es valida la imagen y puede ser leeído, se carga la imagen en un bitap
	FIBITMAP* bitmap = FreeImage_Load(format, filename);
	FreeImage_FlipVertical(bitmap);

	// Obtiene el número de bits por pixel de la imagen
	int bitsPerPixel = FreeImage_GetBPP(bitmap);

	// Convierte la imagen a 32 bits (8 bits por canal).
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32) {
		/*std::cout << "Source image has " << bitsPerPixel
		 << " bits per pixel. Skipping conversion." << std::endl;*/
		bitmap32 = bitmap;
	} else {
		/*std::cout << "Source image has " << bitsPerPixel
		 << " bits per pixel. Converting to 32-bit colour." << std::endl;*/
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}
	// Se obtiene las dimensiones de la imagen.
	int imageWidth = FreeImage_GetWidth(bitmap32);
	int imageHeight = FreeImage_GetHeight(bitmap32);
	/*std::cout << "Image: " << m_fileName << " is size: " << imageWidth << "x"
	 << imageHeight << "." << std::endl;*/

	// Se obtiene un apuntador a los datos de la textura como un arreglo de unsigned bytes.
	GLubyte* textureData = FreeImage_GetBits(bitmap32);

	// Load and create a texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGRA,
	GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unload the 32-bit colour bitmap
	// Se desecha los datos de la textura
	FreeImage_Unload(bitmap32);

	// Si se convirtio la imagen a 32 bits por pixel eliminamos el bitmap
	if (bitsPerPixel != 32) {
		FreeImage_Unload(bitmap);
	}

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
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

		glBindTexture(GL_TEXTURE_2D, texture);
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

	glDeleteTextures(1, &texture);

	shader.destroy();
}
