#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"

InputCodes InputManager::toApplicationKey(int key) {
	switch (key) {
	case 256:
		return InputCodes::kEscape;
	case 87:
		return InputCodes::W;
	case 83:
		return InputCodes::S;
	case 65:
		return InputCodes::A;
	case 68:
		return InputCodes::D;
	case 265:
		return InputCodes::Up;
	case 264:
		return InputCodes::Down;
	case 263:
		return InputCodes::Left;
	case 262:
		return InputCodes::Right;
	}
}

State InputManager::toApplicationState(int state) {
	switch (state) {
	case 0:
		return State::RELESED;
	case 1:
		return State::PRESSED;
	}
}

MouseButtonIndex InputManager::toMouseButtonIndex(int button) {
	switch (button) {
	case 0:
		return MouseButtonIndex::LEFT;
	case 1:
		return MouseButtonIndex::RIGHT;
	case 2:
		return MouseButtonIndex::MIDDLE;
	}
}

void InputManager::keyPressed(InputCodes code, float deltaTime, State state) {
	keyState[code] = state;
}

void InputManager::mouseMoved(float mouseX, float mouseY) {
	if (mouseButtomState[MouseButtonIndex::LEFT]) {
		dx = mouseX - lastMousePos.x;
	}
	if (mouseButtomState[MouseButtonIndex::RIGHT]) {
		dy = lastMousePos.y - mouseY;
	}

	lastMousePos.x = mouseX;
	lastMousePos.y = mouseY;
}

void InputManager::mouseClicked(MouseButtonIndex code, float mouseX,
		float mouseY, State state) {
	switch (code) {
	case RIGHT:
	case LEFT:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		mouseButtomState[code] = state;
		break;
	case MIDDLE:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		break;
	}
}

void InputManager::mouseScroll(float yoffset) {
	ds -= yoffset;
}

void InputManager::do_movement(float deltaTime) {
	float cameraSpeed = 10.0f * deltaTime;
	float cameraSpeedS = 100.0f * deltaTime;

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	theta += cameraSpeed * dx;
	phi += cameraSpeed * dy;
	ratio += cameraSpeedS * ds;

	if (phi >= 2 * M_PI)
		phi = 0;
	if (phi <= -2 * M_PI)
		phi = 0;
	if (phi == 0)
		phi = 0.001;
	if (phi == M_PI)
		phi = M_PI + 0.001;
	if (ratio < 0)
		ratio = 0;

	cameraPos.x = cameraTarget.x + ratio * cos(theta) * sin(phi);
	cameraPos.y = cameraTarget.y + ratio * cos(phi);
	cameraPos.z = cameraTarget.z + ratio * sin(theta) * sin(phi);

	cameraDir = glm::normalize(cameraTarget - cameraPos);

	if ((phi <= 0 && phi >= -M_PI) || phi >= M_PI)
		cameraUP = glm::vec3(0, -1, 0);
	else
		cameraUP = glm::vec3(0, 1, 0);

	dx = 0;
	dy = 0;
	ds = 0;

}
