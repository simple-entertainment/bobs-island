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

	BobMover::BobMover(unsigned long systemId, const Graph& world) :
		falling(false),
		fallTime(0.0f),
		jumping(false),
		jumpTime(0.0f),
		keyboardButtonStates(),
		systemId(systemId),
		world(world)
	{
	}

	void BobMover::execute(Entity& entity)
	{
		if (keyboardButtonStates[Keyboard::Button::SPACE] == Button::State::DOWN)
		{
			jumping = true;
		}

		if (keyboardButtonStates[Keyboard::Button::W] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(0.0f, 0.0f, -Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::A] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(-Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::S] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(0.0f, 0.0f, Simplicity::getDeltaTime() * 5.0f, 1.0f));
		}

		if (keyboardButtonStates[Keyboard::Button::D] == Button::State::DOWN)
		{
			translate(entity.getTransform(), Vector4(Simplicity::getDeltaTime() * 5.0f, 0.0f, 0.0f, 1.0f));
		}

		if (test)
		{
			translate(entity.getTransform(), Vector4(1.0f, 0.0f, -1.0f, 1.0f));
			test = false;
		}

		updateY(entity);

		Simplicity::getScene()->updateGraphs(entity);
	}

	/*unique_ptr<Triangle> BobMover::getGroundAtBobsPosition(Entity& entity)
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
				const MeshData& groundMeshData = groundMesh->getData(); // TODO ScopedMeshData !!!
				for (unsigned int index = 0; index < groundMeshData.size(); index += 3)
				{
					Vector3 triangle2d0 = groundMeshData[index].position;
					triangle2d0.Y() = 0.0f;
					Vector3 triangle2d1 = groundMeshData[index + 1].position;
					triangle2d1.Y() = 0.0f;
					Vector3 triangle2d2 = groundMeshData[index + 2].position;
					triangle2d2.Y() = 0.0f;

					// Determine if Bob is inside the triangle ignoring the Y axis.
					if (Intersection::contains(Triangle(triangle2d0, triangle2d1, triangle2d2), position2d))
					{
						unique_ptr<Triangle> groundAtBobsPosition(new Triangle(groundMeshData[index].position,
								groundMeshData[index + 1].position, groundMeshData[index + 2].position));

						groundMesh->releaseData();
						return groundAtBobsPosition;
					}
				}
				groundMesh->releaseData();
			}
		}

		return unique_ptr<Triangle>();
	}*/

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

	void BobMover::onAddEntity(Entity& /* entity */)
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
		if (message.senderSystemId == systemId)
		{
			const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message.body);
			keyboardButtonStates[event->button] = event->buttonState;

			if (event->button == Keyboard::Button::SPACE && event->buttonState == Button::State::UP)
			{
				test = true;
			}
		}

		return false;
	}

	void BobMover::onRemoveEntity(Entity& /* entity */)
	{
		Messages::deregisterRecipient(Subject::KEYBOARD_BUTTON, bind(&BobMover::onKeyboardButton, this,
				placeholders::_1));
	}

	void BobMover::updateY(Entity& entity)
	{
		/*unique_ptr<Triangle> ground = getGroundAtBobsPosition(entity);

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
		}*/
		float groundY = Simplicity::getScene()->getEntities(111)[0]->getComponent<LODTerrainStreamer>()->getHeight(getPosition3(entity.getTransform()));
		groundY += 1.0f;

		Vector3 position = getPosition3(entity.getTransform());
		position.Y() = getYForBob(position.Y(), groundY);

		setPosition(entity.getTransform(), position);
	}
}