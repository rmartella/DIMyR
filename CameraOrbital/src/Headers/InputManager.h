#ifndef _InputManager_H
#define _InputManager_H

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standar GLFW for the management of inputs codes keyboards
enum InputCodes {
	kEscape = 27,
	Space = 32,
	Left = 37,
	Up = 38,
	Right = 39,
	Down = 40,
	a = 97,
	A = 65,
	b = 98,
	B = 66,
	c = 99,
	C = 67,
	d = 100,
	D = 68,
	e = 101,
	E = 69,
	f = 102,
	F = 70,
	g = 103,
	G = 71,
	h = 104,
	H = 72,
	i = 105,
	I = 73,
	j = 106,
	J = 74,
	k = 107,
	K = 75,
	l = 108,
	L = 76,
	m = 109,
	M = 77,
	n = 110,
	N = 78,
	o = 111,
	O = 79,
	p = 112,
	P = 80,
	q = 113,
	Q = 81,
	r = 114,
	R = 82,
	s = 115,
	S = 83,
	t = 116,
	T = 84,
	u = 117,
	U = 85,
	v = 118,
	V = 86,
	w = 119,
	W = 87,
	x = 120,
	X = 88,
	y = 121,
	Y = 89,
	z = 122,
	Z = 90
};

enum MouseButtonIndex {
	LEFT = 0, RIGHT = 1, MIDDLE = 2
};

enum State {
	RELESED = 0, PRESSED = 1, REPEATED = 2
};

class InputManager {
public:

	InputManager() :
			lastMousePos(glm::ivec2(0.0)), cameraPos(glm::vec3(0, 0, 0)), cameraDir(
					glm::vec3(0, 0, -1)), cameraUP(glm::vec3(0, 1, 0)), theta(
					1.5708), phi(1.5708), ratio(5), dx(0), dy(0), ds(0) {
		mouseButtomState = new bool[3];
		keyState = new bool[1024];
		for (int i = 0; i < 3; i++)
			mouseButtomState[i] = false;
		for (int i = 0; i < 1024; i++)
			keyState[i] = false;
	}

	~InputManager() {
		delete[] mouseButtomState;
		delete[] keyState;
	}

	InputCodes toApplicationKey(int key);
	State toApplicationState(int state);
	MouseButtonIndex toMouseButtonIndex(int button);

	void keyPressed(InputCodes code, float deltaTime, State state);
	void mouseMoved(float mouseX, float mouseY);
	void mouseClicked(MouseButtonIndex code, float mouseX, float mouseY,
			State state);
	void mouseScroll(float yoffset);
	void do_movement(float deltaTime);

	glm::ivec2 getLastMousePos() {
		return lastMousePos;
	}

	bool* getKeyState() {
		return keyState;
	}

	glm::vec3 getCameraDir() {
		return cameraDir;
	}

	void setCameraDir(glm::vec3 cameraDir) {
		this->cameraDir = cameraDir;
	}

	glm::vec3 getCameraPos() {
		return cameraPos;
	}

	void setCameraPos(glm::vec3 cameraPos) {
		this->cameraPos = cameraPos;
	}

	glm::vec3 getCameraUp() {
		return cameraUP;
	}

	void setCameraUp(glm::vec3 cameraUp) {
		cameraUP = cameraUp;
	}

protected:
	glm::ivec2 lastMousePos;
	bool * mouseButtomState;
	bool * keyState;

	glm::vec3 cameraPos;
	glm::vec3 cameraDir;
	glm::vec3 cameraUP;

	float theta, phi, ratio;
	float dx, dy, ds;

};

#endif
