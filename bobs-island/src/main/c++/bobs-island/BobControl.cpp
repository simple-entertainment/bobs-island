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

//#include <the-island/API.h>

#include "BobControl.h"

using namespace boost;
using namespace simplicity;
//using namespace theisland;

namespace bobsisland
{
	BobControl::BobControl(const simplicity::Graph& world) :
		buttonStates(),
		world(world),
		x(-1),
		y(-1)
	{
	}

	void BobControl::destroy()
	{
		Messages::deregisterRecipient(Events::KEYBOARD_BUTTON, bind(&BobControl::onKeyboardButton, this,
				placeholders::_1));
		Messages::deregisterRecipient(Events::MOUSE_MOVE, bind(&BobControl::onMouseMove, this,
				placeholders::_1));
	}

	void BobControl::execute()
	{
		if (buttonStates[Keyboard::Button::W] == Button::State::DOWN)
		{
			MathFunctions::translate(getEntity()->getTransformation(),
					Vector4(0.0f, 0.0f, -Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::A] == Button::State::DOWN)
		{
			MathFunctions::translate(getEntity()->getTransformation(),
					Vector4(-Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::S] == Button::State::DOWN)
		{
			MathFunctions::translate(getEntity()->getTransformation(),
					Vector4(0.0f, 0.0f, Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::D] == Button::State::DOWN)
		{
			MathFunctions::translate(getEntity()->getTransformation(),
					Vector4(Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		updateY();
	}

	void BobControl::init()
	{
		buttonStates[Keyboard::Button::W] = Button::State::UP;
		buttonStates[Keyboard::Button::A] = Button::State::UP;
		buttonStates[Keyboard::Button::S] = Button::State::UP;
		buttonStates[Keyboard::Button::D] = Button::State::UP;

		Messages::registerRecipient(Events::KEYBOARD_BUTTON, bind(&BobControl::onKeyboardButton, this,
				placeholders::_1));
		Messages::registerRecipient(Events::MOUSE_MOVE, bind(&BobControl::onMouseMove, this,
				placeholders::_1));
	}

	void BobControl::onKeyboardButton(any message)
	{
		KeyboardButtonEvent* event = any_cast<KeyboardButtonEvent*>(message);
		buttonStates[event->button] = event->buttonState;
	}

	void BobControl::onMouseMove(any message)
	{
		MouseMoveEvent* event = any_cast<MouseMoveEvent*>(message);

		if (x != -1)
		{
			int deltaX = event->x - x;
			int deltaY = event->y - y;
			MathFunctions::rotate(getEntity()->getTransformation(), deltaX * -Simplicity::getDeltaTime() * 0.1f,
					Vector4(0.0f, 1.0f, 0.0f, 1.0f));
			MathFunctions::rotate(getEntity()->getComponent<Camera>()->getTransformation(),
					deltaY * -Simplicity::getDeltaTime() * 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		}

		x = event->x;
		y = event->y;
	}

	void BobControl::updateY()
	{
		Vector3 position = MathFunctions::getTranslation3(getEntity()->getTransformation());
		vector<Entity*> entities = world.getEntitiesWithinBounds(Square(0.25f), position);

		for (Entity* entity : entities)
		{
			if (entity->getCategory() == 128)
			{
				if (updateY(*entity->getComponent<Mesh>(Categories::RENDER)))
				{
					return;
				}
			}
		}
	}

	bool BobControl::updateY(const Mesh& ground)
	{
		Vector3 position = MathFunctions::getTranslation3(getEntity()->getTransformation());
		Vector3 position2d = position;
		position2d.Y() = 0.0f;

		vector<unsigned int> indices = ground.getIndices();
		vector<Vertex> vertices = ground.getVertices();
		for (unsigned int triangleIndex = 0; triangleIndex < indices.size(); triangleIndex += 3)
		{
			Vector3 triangle2d0 = vertices[triangleIndex].position;
			triangle2d0.Y() = 0.0f;
			Vector3 triangle2d1 = vertices[triangleIndex + 1].position;
			triangle2d1.Y() = 0.0f;
			Vector3 triangle2d2 = vertices[triangleIndex + 2].position;
			triangle2d2.Y() = 0.0f;

			// Determine if Bob is inside the triangle ignoring the Y axis.
			if (Intersection::contains(Triangle(triangle2d0, triangle2d1, triangle2d2), position2d))
			{
				Vector3 edge0 = vertices[triangleIndex + 1].position - vertices[triangleIndex].position;
				Vector3 edge1 = vertices[triangleIndex + 2].position - vertices[triangleIndex].position;
				Vector3 normal = MathFunctions::crossProduct(edge0, edge1);

				// Solve the equation for the plane (ax + by + cz + d = 0) to find y.
				float d = dotProduct(normal, vertices[triangleIndex].position) * -1.0f;
				float y = ((normal.X() * position.X() + normal.Z() * position.Z() + d) / normal.Y()) * -1.0f;

				position.Y() = y + 1.0f; // Add the half extent of Bob's body.

				MathFunctions::setTranslation(getEntity()->getTransformation(), position);

				return true;
			}
		}

		return false;
	}
}
