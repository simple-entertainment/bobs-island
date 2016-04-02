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
#include <functional>

#include <simplicity/terrain/API.h>

#include "BobMover.h"

using namespace simplicity;
using namespace simplicity::terrain;
using namespace std;

namespace bobsisland
{
	bool test = false;

	BobMover::BobMover() :
		falling(false),
		fallTime(0.0f),
		jumping(false),
		jumpTime(0.0f),
		keyboardButtonStates()
	{
	}

	void BobMover::execute()
	{
		if (keyboardButtonStates[Keyboard::Button::SPACE] == Button::State::DOWN)
		{
			jumping = true;
		}

		if (keyboardButtonStates[Keyboard::Button::W] == Button::State::DOWN)
		{
			getEntity()->translate(Vector3(0.0f, 0.0f, -Simplicity::getDeltaTime() * 5.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::A] == Button::State::DOWN)
		{
			getEntity()->translate(Vector3(-Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::S] == Button::State::DOWN)
		{
			getEntity()->translate(Vector3(0.0f, 0.0f, Simplicity::getDeltaTime() * 5.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::D] == Button::State::DOWN)
		{
			getEntity()->translate(Vector3(Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f));
		}

		if (test)
		{
			getEntity()->translate(Vector3(1.0f, 0.0f, -1.0f));
			test = false;
		}

		updateY();
	}

	float BobMover::getYForBob(float bobY, float groundY)
	{
		float newBobY = bobY;

		if (jumping)
		{
			newBobY += cos(jumpTime * MathConstants::PI * 0.5f);

			jumpTime += Simplicity::getDeltaTime();
			if (jumpTime >= 1.0f)
			{
				falling = true;
				fallTime = 0.0f;
				jumping = false;
				jumpTime = 0.0f;
			}
		}
		else if (falling)
		{
			newBobY -= sin(min(fallTime * MathConstants::PI * 0.5f, MathConstants::PI * 0.5f));

			fallTime += Simplicity::getDeltaTime();
		}
		else
		{
			newBobY = groundY;
		}

		if (groundY > newBobY)
		{
			falling = false;
			fallTime = 0.0f;

			newBobY = groundY;
		}

		return newBobY;
	}

	void BobMover::onAddEntity()
	{
		keyboardButtonStates[Keyboard::Button::W] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::A] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::S] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::D] = Button::State::UP;
		keyboardButtonStates[Keyboard::Button::SPACE] = Button::State::UP;

		Messages::registerRecipient(Subject::KEYBOARD_BUTTON, bind(&BobMover::onKeyboardButton, this,
				placeholders::_1));
	}

	bool BobMover::onKeyboardButton(const Message& message)
	{
		const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message.body);
		keyboardButtonStates[event->button] = event->buttonState;

		if (event->button == Keyboard::Button::SPACE && event->buttonState == Button::State::UP)
		{
			test = true;
		}

		return false;
	}

	void BobMover::onRemoveEntity()
	{
		Messages::deregisterRecipient(Subject::KEYBOARD_BUTTON, bind(&BobMover::onKeyboardButton, this,
				placeholders::_1));
	}

	void BobMover::updateY()
	{
		Entity* terrain = Simplicity::getScene()->getEntities(111)[0];
		float groundY = terrain->getComponent<TerrainStreamer>()->getHeight(getEntity()->getPosition());
		groundY += 1.0f;

		Vector3 position = getPosition3(getEntity()->getTransform());
		position.Y() = getYForBob(position.Y(), groundY);

		setPosition(getEntity()->getTransform(), position);
	}
}
