#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"
//glfw include
#include <GLFW/glfw3.h>

InputCodes InputManager::toApplicationKey(int key) {
	switch (key) {
	case 48:
		return InputCodes::n0;
	case 49:
		return InputCodes::n1;
	case 50:
		return InputCodes::n2;
	case 51:
		return InputCodes::n3;
	case 52:
		return InputCodes::n4;
	case 53:
		return InputCodes::n5;
	case 54:
		return InputCodes::n6;
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
	case 16:
		return InputCodes::LShift;
	case 32:
		return InputCodes::Space;
	case 257:
		return InputCodes::Enter;
	case 77:
		return InputCodes::m;
	case 76:
		return InputCodes::l;
	case 80:
		return InputCodes::p;
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
		deltax = mouseX - lastMousePos.x;
		deltay = mouseY - lastMousePos.y;
	}
	else {
		deltax = 0;
		deltay = 0;
	}
	lastMousePos = glm::vec2(mouseX, mouseY);
}

void InputManager::mouseClicked(MouseButtonIndex code, float mouseX,
	float mouseY, State state) {
	mouseButtomState[code] = state;
	switch (code) {
	case RIGHT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case LEFT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		mouseButtomState[code] = state;
		break;
	case MIDDLE:
		break;
	}
}

void InputManager::startButton(int screen) {
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		int axesCount;
		int buttonCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (GLFW_PRESS == buttons[7])
			gameState = screen;
	}
	else {
		if (keyState[Enter])
			gameState = screen;
	}
}

void InputManager::mouseScroll(float yoffset) {
	scrollYoffset = yoffset;
}

// Car function

float InputManager::changeVals(float actual, float increment, float maxValue, bool pos) {
	float finalVal = 0.0f;
	if (pos) {
		finalVal = actual + increment;
		if (finalVal > maxValue)
			return maxValue;
	}
	else {
		finalVal = actual - increment;
		if (finalVal < maxValue)
			return maxValue;
	}
	return finalVal;
}

void InputManager::do_movement(float deltaTime) {
	float cameraSpeed = 50.0f * deltaTime;
	//    float gravityVal = 0.12f;
	float rotVal = carSpeed * 4.0f;
	float valVel = 0.0f;
	float valBreak = 0.0f;

	// Case initialization
	float zoomLevel;
	float axesThreshold = 0.0f;
	float horizontalDistance;
	float verticalDistance;
	float theta = 0.0f;
	float offsetx;
	float offsetz;

	int axesCount;
	int buttonCount;
	const float *axes;
	const unsigned char *buttons;

	if (keyState[InputCodes::m])
		gameState = 0;

	if (keyState[InputCodes::l])
		gameState = WINNER;

	if (keyState[InputCodes::p]) {
		mapLoaded++;
		if (mapLoaded > 3)
			mapLoaded = 0;
	}

	if (keyState[InputCodes::n1]) {
		mapLoaded = 0;
	}
	else if (keyState[InputCodes::n2]) {
		mapLoaded = 1;
	}
	else if (keyState[InputCodes::n3]) {
		mapLoaded = 2;
	}
	else if (keyState[InputCodes::n4]) {
		mapLoaded = 3;
	}
	switch (gameState) {
	case SPLASHSCREEN:
		stutter = 0.0f;
		timer = 0.0f;
		startButton(TRANSITIONSCREEN);

		break;
	case MAINGAME:
		timer += deltaTime;

		if (paused || startCount || exploding) {
			if (exploding) {
				timeExploding = timer - explodingBuffer;
			}
			else {
				if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
					buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
					if (GLFW_PRESS == buttons[7] && stutter > 0.5f) {
						paused = !paused;
						carSpeed = carSpeedBuffer;
						timeBuffer = timer;
						stutter = 0.0f;
					}
					else {
						carSpeed = 0.0f;
						stutter = timer - timeBuffer;
					}
				}
				else {
					if (keyState[Enter] && stutter > 0.5f) {
						paused = !paused;
						carSpeed = carSpeedBuffer;
						timeBuffer = timer;
						stutter = 0.0f;
					}
					else {
						carSpeed = 0.0f;
						stutter = timer - timeBuffer;
					}
				}

				if (timer > 5.0f) {
					startCount = false;
				}
				if (timer < 2.0f) {
					//Calculate Horizontal distance
					horizontalDistance = initialMovement
						* glm::cos(glm::radians(pitch));
					//Calculate Vertical distance
					verticalDistance = initialMovement
						* glm::sin(glm::radians(pitch));

					initialMovement = changeVals(initialMovement, 10.0f, distanceFromPlayer, false);

					offsetx = horizontalDistance * glm::sin(glm::radians(theta));
					offsetz = horizontalDistance * glm::cos(glm::radians(theta));
					cameraPos.x = camera_look_at.x - offsetx;
					cameraPos.z = camera_look_at.z - offsetz;
					cameraPos.y = camera_look_at.y + verticalDistance;
				}
			}
		}
		else {
			gas = changeVals(gas, 0.0005f, 0.0f, false);

			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
				buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
				if (gas > 0.0f && !fallToDeath) {
					valVel = (axes[5] + 1) / 2;
					valBreak = (axes[4] + 1) / 2;
				}

				if (axes[2] > 0) 
					angleAroundPlayer = changeVals(angleAroundPlayer, axes[2] * cameraSpeed, 60.0f, true);
				else if (axes[2] < 0)
					angleAroundPlayer = changeVals(angleAroundPlayer, -axes[2] * cameraSpeed, -60.0f, false);
				else
					if (angleAroundPlayer > 0)
						angleAroundPlayer = changeVals(angleAroundPlayer, 2.0f * cameraSpeed, 0.0f, false);
					else if (angleAroundPlayer < 0)
						angleAroundPlayer = changeVals(angleAroundPlayer, 2.0f * cameraSpeed, 0.0f, true);

				if (GLFW_PRESS == buttons[0])
					if (initJump) {
						jumpSound = true;
						jumping = true;
						carYSpeed = gravityVal;
						initJump = false;
					}

				if (carSpeed > 0 && !fallToDeath)
					if (axes[0] < -axesThreshold) {
						carAngle = changeVals(carAngle, rotVal * 0.4f, carInitAngle + 10.0f, true);
						roty += (rotVal * 0.5f + 0.1f) * abs(axes[0]);
					}
					else if (axes[0] > axesThreshold) {
						carAngle = changeVals(carAngle, rotVal * 0.4f, carInitAngle - 10.0f, false);
						roty -= (rotVal * 0.5f + 0.1f) * abs(axes[0]);
					}
					else {
						if (carAngle > carInitAngle)
							carAngle = changeVals(carAngle, rotVal * 0.5f, carInitAngle, false);
						else if (carAngle < carInitAngle)
							carAngle = changeVals(carAngle, rotVal * 0.5f, 90.0f, true);
						else if (carSpeed < 0) {
							if (axes[0] < -axesThreshold)
								roty += rotVal * 0.5f;
							if (axes[0] > axesThreshold)
								roty -= rotVal * 0.5f;
						}
					}
				else if (carSpeed < 0 && !fallToDeath)
					roty += (rotVal * 0.5f + 0.1f) * axes[0];

				if (GLFW_PRESS == buttons[2])
					carSpeed = changeVals(carSpeed, 0.002f, 0.0f, 0);
			}
			else {
				if (gas > 0.0f && !fallToDeath) {
					if (keyState[InputCodes::W])
						valVel = 1.0f;
					if (keyState[InputCodes::S])
						valBreak = 1.0f;
				}
				if (keyState[InputCodes::Space] && !fallToDeath)
					if (initJump) {
						jumping = true;
						jumpSound = true;
						carYSpeed = gravityVal;
						initJump = false;
					}
				if (carSpeed > 0 && !fallToDeath) {
					if (keyState[InputCodes::A]) {
						roty += (rotVal * 0.5f + 0.1f);
						carAngle = changeVals(carAngle, rotVal * 0.4f, carInitAngle + 10.0f, true);
					}
					if (keyState[InputCodes::D]) {
						carAngle = changeVals(carAngle, rotVal * 0.4f, carInitAngle - 10.0f, false);
						roty -= (rotVal * 0.5f + 0.1f);
					}
					if ((!keyState[InputCodes::D] && !keyState[InputCodes::A]))
						if (carAngle > carInitAngle)
							carAngle = changeVals(carAngle, rotVal * 0.5f, carInitAngle, false);
						else if (carAngle < carInitAngle)
							carAngle = changeVals(carAngle, rotVal * 0.5f, 90.0f, true);
				}
				else if (carSpeed < 0 && !fallToDeath) {
					if (keyState[InputCodes::A])
						roty -= (rotVal * 0.5f + 0.1f);
					if (keyState[InputCodes::D])
						roty += (rotVal * 0.5f + 0.1f);
				}

				if (keyState[InputCodes::LShift])
					carSpeed = changeVals(carSpeed, 0.02f, 0.0f, 0);

			}
			if (jumping && !fallToDeath) {
				carYSpeed = changeVals(carYSpeed, 0.032, 0, false);
				carY = changeVals(carY, (carYSpeed * glm::sin(glm::radians(45.0f))), 50.0f, true);
				//carY += carYSpeed * sin(glm::radians(45.0f));
				if (carYSpeed == 0) {
					jumping = false;
					falling = true;
				}
			}
			else
				if (falling && !fallToDeath) {
					carYSpeed = changeVals(carYSpeed, 0.016, gravityVal, true);
					carY = changeVals(carY, carYSpeed * glm::sin(glm::radians(45.0f)), 0.0f, false);
					//carY -= carYSpeed * sin(glm::radians(45.0f));
					if (carYSpeed == gravityVal) {
						falling = false;
						carYSpeed = 0.0f;
						initJump = true;
					}
				}

			if (fallToDeath)
				carY -= 0.5f * sin(glm::radians(45.0f));

			// Calculate zoom
			zoomLevel = scrollYoffset * cameraSpeed;
			distanceFromPlayer -= zoomLevel;

			// Calculate pitch
			if (mouseButtomState[MouseButtonIndex::RIGHT])
				pitch -= deltay * cameraSpeed;

			// Calculate Angle Arround
			if (mouseButtomState[MouseButtonIndex::LEFT])
				angleAroundPlayer -= deltax * cameraSpeed;

			//Calculate Horizontal distance
			horizontalDistance = distanceFromPlayer
				* glm::cos(glm::radians(pitch));
			//Calculate Vertical distance
			verticalDistance = distanceFromPlayer * glm::sin(glm::radians(pitch));

			//Calculate camera position
			theta = roty + angleAroundPlayer;
			offsetx = horizontalDistance * glm::sin(glm::radians(theta));
			offsetz = horizontalDistance * glm::cos(glm::radians(theta));
			cameraPos.x = camera_look_at.x - offsetx;
			cameraPos.z = camera_look_at.z - offsetz;
			cameraPos.y = camera_look_at.y + verticalDistance;

			yaw = 180 - theta;

			scrollYoffset = 0;
			deltax = 0;
			deltay = 0;


			// Car values
			if (valVel > 0 && valBreak == 0) {
				if (gas > 0.0f)
					carSpeed = changeVals(carSpeed, valVel * 0.01f, 1.0f, 1);
				else {
					if (carSpeed > 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 0);
					else if (carSpeed < 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 1);
				}
			}
			else if (valVel == 0 && valBreak > 0) {
				if (gas > 0.0f)
					carSpeed = changeVals(carSpeed, valBreak * 0.01f, -0.3f, 0);
				else {
					if (carSpeed > 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 0);
					else if (carSpeed < 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 1);
				}
			}
			else if (valVel == 0 && valBreak == 0) {
				if (carSpeed > 0)
					carSpeed = changeVals(carSpeed, 0.005f, 0.0f, 0);
				else if (carSpeed < 0)
					carSpeed = changeVals(carSpeed, 0.005f, 0.0f, 1);
			}
			else {
				if (gas == 0.0f)
					if (carSpeed > 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 0);
					else if (carSpeed < 0)
						carSpeed = changeVals(carSpeed, 0.01f, 0.0f, 1);
			}

			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
				if (GLFW_PRESS == buttons[7] && stutter > 0.5f) {
					paused = !paused;
					carSpeedBuffer = carSpeed;
					timeBuffer = timer;
					stutter = 0.0f;
				}
				else {
					stutter = timer - timeBuffer;
				}
			}

			if (keyState[Enter] && stutter > 0.5f) {
				paused = !paused;
				carSpeedBuffer = carSpeed;
				timeBuffer = timer;
				stutter = 0.0f;
			}
			else {
				stutter = timer - timeBuffer;
			}

			if (timer <= 0.1f) {
				initialMovement = 400.0f;
				startCount = true;
			}
		}

		break;
	case TRANSITIONSCREEN:
		gameState = MAINGAME;

		stutter = 0.0f;
		timer = 0.0f;
		break;
	case STARTMENU:

		startButton(MAINGAME);
		stutter = 0.0f;
		timer = 0.0f;
		break;
	case GAMEOVER:
		stutter = 0.0f;
		timer = 0.0f;
		startButton(SPLASHSCREEN);
		break;
	case WINNER:
		stutter = 0.0f;
		timer = 0.0f;
		startButton(CREDITS);
		break;
	case CREDITS:
		stutter = 0.0f;
		timer = 0.0f;
		if (keyState[InputCodes::q])
			gameState = EXIT;
		break;
	case EXIT:
		exit(0);
	}

}
