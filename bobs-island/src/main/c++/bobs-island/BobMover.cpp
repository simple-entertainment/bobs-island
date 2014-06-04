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

#include <the-island/API.h>

#include "BobMover.h"

using namespace simplicity;
using namespace std;
using namespace theisland;

namespace bobsisland
{
	BobMover::BobMover(const Graph& world) :
		buttonStates(),
		falling(false),
		fallTime(0.0f),
		jumping(false),
		jumpTime(0.0f),
		world(world)
	{
	}

	void BobMover::execute(Entity& entity)
	{
		if (buttonStates[Keyboard::Button::W] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(0.0f, 0.0f, -Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::A] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(-Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::S] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(0.0f, 0.0f, Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (buttonStates[Keyboard::Button::D] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		updateY(entity);

		Simplicity::getScene()->updateGraphs(entity);
	}

	unique_ptr<Triangle> BobMover::getGroundAtBobsPosition(Entity& entity)
	{
		Vector3 position = getPosition3(entity.getTransform());
		Vector3 position2d = position;
		position2d.Y() = 0.0f;

		vector<Entity*> entities = world.getEntitiesWithinBounds(Square(0.25f), position);
		for (Entity* entity : entities)
		{
			if (entity->getCategory() == EntityCategories::GROUND)
			{
				Mesh* groundMesh = entity->getComponent<Mesh>(Category::RENDER);
				unsigned int* indices = groundMesh->getIndices();
				Vertex* vertices = groundMesh->getVertices();
				for (unsigned int index = 0; index < groundMesh->getIndexCount(); index += 3)
				{
					Vector3 triangle2d0 = vertices[indices[index]].position;
					triangle2d0.Y() = 0.0f;
					Vector3 triangle2d1 = vertices[indices[index + 1]].position;
					triangle2d1.Y() = 0.0f;
					Vector3 triangle2d2 = vertices[indices[index + 2]].position;
					triangle2d2.Y() = 0.0f;

					// Determine if Bob is inside the triangle ignoring the Y axis.
					if (Intersection::contains(Triangle(triangle2d0, triangle2d1, triangle2d2), position2d))
					{
						return unique_ptr<Triangle>(new Triangle(vertices[index].position,
							vertices[index + 1].position, vertices[index + 2].position));
					}
				}
			}
		}

		return unique_ptr<Triangle>();
	}

	float BobMover::getYAtBobsPosition(Entity& entity, const Triangle& ground)
	{
		Vector3 position2d = getPosition3(entity.getTransform());
		position2d.Y() = 0.0f;

		Vector3 edge0 = ground.getPointB() - ground.getPointA();
		Vector3 edge1 = ground.getPointC() - ground.getPointA();
		Vector3 normal = crossProduct(edge0, edge1);

		// Solve the equation for the plane (ax + by + cz + d = 0) to find y.
		float d = dotProduct(normal, ground.getPointA()) * -1.0f;
		float y = ((normal.X() * position2d.X() + normal.Z() * position2d.Z() + d) / normal.Y()) * -1.0f;

		return y + 1.0f; // Add the half extent of Bob's body.
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
			newBobY= groundY;
		}

		if (groundY > newBobY)
		{
			falling = false;
			fallTime = 0.0f;

			newBobY = groundY;
		}

		return newBobY;
	}

	void BobMover::onCloseScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::deregisterRecipient(Subject::KEYBOARD_BUTTON, bind(&BobMover::onKeyboardButton, this,
			placeholders::_1));
	}

	void BobMover::onKeyboardButton(const void* message)
	{
		const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message);
		buttonStates[event->button] = event->buttonState;

		if (event->button == Keyboard::Button::SPACE && event->buttonState == Button::State::UP)
		{
			jumping = true;
		}
	}

	void BobMover::onOpenScene(Scene& /* scene */, Entity& /* entity */)
	{
		buttonStates[Keyboard::Button::W] = Button::State::UP;
		buttonStates[Keyboard::Button::A] = Button::State::UP;
		buttonStates[Keyboard::Button::S] = Button::State::UP;
		buttonStates[Keyboard::Button::D] = Button::State::UP;

		Messages::registerRecipient(Subject::KEYBOARD_BUTTON, bind(&BobMover::onKeyboardButton, this,
			placeholders::_1));
	}

	void BobMover::updateY(Entity& entity)
	{
		unique_ptr<Triangle> ground = getGroundAtBobsPosition(entity);

		float groundY = 0.0f;
		if (ground.get() != nullptr)
		{
			groundY = getYAtBobsPosition(entity, *ground);
		}
		else
		{
			groundY = -100.0f;

			if (!jumping)
			{
				falling = true;
			}
		}

		Vector3 position = getPosition3(entity.getTransform());
		position.Y() = getYForBob(position.Y(), groundY);

		setPosition(entity.getTransform(), position);
	}
}
