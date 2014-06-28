/*
 * Copyright © 2014 Simple Entertainment Limited
 *
 * This file is part of Bob's Island.
 *
 * Bob's Island is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Bob's Island is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with Bob's Island. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <the-island/API.h>

#include <bobs-island/BobConstants.h>

#include "BobController.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobController::BobController() :
		keyboardButtonStates(),
		mousePosition(0, 0),
		newMousePosition(0, 0),
		shooting(false)
	{
	}

	void BobController::execute(Entity& /* entity */)
	{
		if (keyboardButtonStates[Keyboard::Button::SPACE] == Button::State::DOWN)
		{
			Messages::send(Message(Subject::JUMP, nullptr));
		}

		Vector<int, 2> delta = newMousePosition - mousePosition;
		if (delta != Vector<int, 2>(0, 0))
		{
			Messages::send(Message(Subject::LOOK, &delta));
			mousePosition = newMousePosition;
		}

		if (keyboardButtonStates[Keyboard::Button::W] == Button::State::DOWN)
		{
			Direction direction = Direction::FORWARD;
			Messages::send(Message(Subject::MOVE, &direction));
		}

		if (keyboardButtonStates[Keyboard::Button::A] == Button::State::DOWN)
		{
			Direction direction = Direction::LEFT;
			Messages::send(Message(Subject::MOVE, &direction));
		}

		if (keyboardButtonStates[Keyboard::Button::S] == Button::State::DOWN)
		{
			Direction direction = Direction::BACKWARD;
			Messages::send(Message(Subject::MOVE, &direction));
		}

		if (keyboardButtonStates[Keyboard::Button::D] == Button::State::DOWN)
		{
			Direction direction = Direction::RIGHT;
			Messages::send(Message(Subject::MOVE, &direction));
		}

		if (shooting)
		{
			Messages::send(Message(Subject::SHOOT, nullptr));
			shooting = false;
		}
	}

	void BobController::onAddEntity(Entity& /* entity */)
	{
		keyboardButtonStates[Keyboard::Button::W] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::A] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::S] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::D] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::SPACE] = Button::State::UP;

		Messages::registerRecipient(simplicity::Subject::KEYBOARD_BUTTON, bind(&BobController::onKeyboardButton, this,
			placeholders::_1));
		Messages::registerRecipient(simplicity::Subject::MOUSE_BUTTON, bind(&BobController::onMouseButton, this,
			placeholders::_1));
		Messages::registerRecipient(simplicity::Subject::MOUSE_MOVE, bind(&BobController::onMouseMove, this,
			placeholders::_1));
	}

	bool BobController::onKeyboardButton(const Message& message)
	{
		const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message.body);
		keyboardButtonStates[event->button] = event->buttonState;

		return false;
	}

	bool BobController::onMouseButton(const Message& message)
	{
		const MouseButtonEvent* event = static_cast<const MouseButtonEvent*>(message.body);
		if (event->button == Mouse::Button::LEFT && event->buttonState == Button::State::UP)
		{
			shooting = true;
		}

		return false;
	}

	bool BobController::onMouseMove(const Message& message)
	{
		const MouseMoveEvent* event = static_cast<const MouseMoveEvent*>(message.body);

		newMousePosition.X() = event->x;
		newMousePosition.Y() = event->y;

		return false;
	}

	void BobController::onRemoveEntity(Entity& /* entity */)
	{
		Messages::deregisterRecipient(simplicity::Subject::KEYBOARD_BUTTON, bind(&BobController::onKeyboardButton,
			this, placeholders::_1));
		Messages::deregisterRecipient(simplicity::Subject::MOUSE_BUTTON, bind(&BobController::onMouseButton, this,
			placeholders::_1));
		Messages::deregisterRecipient(simplicity::Subject::MOUSE_MOVE, bind(&BobController::onMouseMove, this,
			placeholders::_1));
	}
}
