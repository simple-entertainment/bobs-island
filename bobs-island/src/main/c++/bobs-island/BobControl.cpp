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

using namespace simplicity;
using namespace std;
//using namespace theisland;

namespace bobsisland
{
	BobControl::BobControl(const simplicity::Graph& world) :
		buttonStates(),
		falling(false),
		fallTime(0.0f),
		firing(false),
		jumping(false),
		jumpTime(0.0f),
		mouseDelta(),
		world(world),
		x(-1),
		y(-1)
	{
	}

	void BobControl::execute(Entity& entity)
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

		//updateY(entity);
		turn(entity);

		if (firing)
		{
			fireGun(entity);
		}

		Simplicity::getScene()->updateGraphs(entity);
	}

	void BobControl::fireGun(Entity& entity)
	{
		unique_ptr<Entity> bullet(new Entity);
		bullet->setTransform(entity.getTransform() * entity.getComponents<Mesh>()[1]->getTransform());
		rotate(bullet->getTransform(), MathConstants::PI * -0.5f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		unique_ptr<Mesh> mesh = ModelFactory::getInstance().createPyramidMesh(0.1f, 0.5f,
				Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		mesh->init();

		unique_ptr<Model> bounds = ModelFunctions::getSquareBoundsXZ(mesh->getVertices(), mesh->getVertexCount());

		unique_ptr<Model> bodyModel(new Box(0.1f, 0.25f, 0.1f));

		Body::Material material;
		material.mass = 0.2f;
		material.friction = 0.5f;
		material.restitution = 0.1f;
		unique_ptr<Body> body = PhysicsFactory::getInstance()->createBody(material, bodyModel.get(),
				bullet->getTransform());

		Vector3 trajectory = getUp3(bullet->getTransform());
		trajectory.normalize();
		trajectory *= 50.0f;
		body->applyForce(trajectory, Vector3(0.0f, 0.0f, 0.0f));

		bullet->addUniqueComponent(move(mesh));
		bullet->addUniqueComponent(move(bounds));
		bullet->addUniqueComponent(move(body));
		bullet->addUniqueComponent(move(bodyModel));

		Simplicity::getScene()->addEntity(move(bullet));

		firing = false;
	}

	unique_ptr<Triangle> BobControl::getGroundAtBobsPosition(Entity& entity)
	{
		Vector3 position = getPosition3(entity.getTransform());
		Vector3 position2d = position;
		position2d.Y() = 0.0f;

		vector<Entity*> entities = world.getEntitiesWithinBounds(Square(0.25f), position);
		for (Entity* entity : entities)
		{
			if (entity->getCategory() == 128)
			{
				Mesh* groundMesh = entity->getComponent<Mesh>(Category::RENDER);
				Vertex* vertices = groundMesh->getVertices();
				for (unsigned int triangleIndex = 0; triangleIndex < groundMesh->getVertexCount(); triangleIndex += 3)
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
						return unique_ptr<Triangle>(new Triangle(vertices[triangleIndex].position,
								vertices[triangleIndex + 1].position, vertices[triangleIndex + 2].position));
					}
				}
			}
		}

		return unique_ptr<Triangle>();
	}

	float BobControl::getYAtBobsPosition(Entity& entity, const Triangle& ground)
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

	float BobControl::getYForBob(float bobY, float groundY)
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

	void BobControl::onCloseScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::deregisterRecipient(Subject::KEYBOARD_BUTTON, bind(&BobControl::onKeyboardButton, this,
			placeholders::_1));
		Messages::deregisterRecipient(Subject::MOUSE_BUTTON, bind(&BobControl::onMouseButton, this,
			placeholders::_1));
		Messages::deregisterRecipient(Subject::MOUSE_MOVE, bind(&BobControl::onMouseMove, this,
			placeholders::_1));
	}

	void BobControl::onKeyboardButton(const void* message)
	{
		const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message);
		buttonStates[event->button] = event->buttonState;

		if (event->button == Keyboard::Button::SPACE && event->buttonState == Button::State::UP)
		{
			jumping = true;
		}
	}

	void BobControl::onMouseButton(const void* message)
	{
		const MouseButtonEvent* event = static_cast<const MouseButtonEvent*>(message);
		if (event->button == Mouse::Button::LEFT && event->buttonState == Button::State::UP)
		{
			firing = true;
		}
	}

	void BobControl::onMouseMove(const void* message)
	{
		const MouseMoveEvent* event = static_cast<const MouseMoveEvent*>(message);

		mouseDelta.X() = static_cast<float>(event->x);
		mouseDelta.Y() = static_cast<float>(event->y);
	}

	void BobControl::onOpenScene(Scene& /* scene */, Entity& /* entity */)
	{
		buttonStates[Keyboard::Button::W] = Button::State::UP;
		buttonStates[Keyboard::Button::A] = Button::State::UP;
		buttonStates[Keyboard::Button::S] = Button::State::UP;
		buttonStates[Keyboard::Button::D] = Button::State::UP;

		Messages::registerRecipient(Subject::KEYBOARD_BUTTON, bind(&BobControl::onKeyboardButton, this,
			placeholders::_1));
		Messages::registerRecipient(Subject::MOUSE_BUTTON, bind(&BobControl::onMouseButton, this,
			placeholders::_1));
		Messages::registerRecipient(Subject::MOUSE_MOVE, bind(&BobControl::onMouseMove, this,
			placeholders::_1));
	}

	void BobControl::turn(Entity& entity)
	{
		int deltaX = static_cast<int>(mouseDelta.X()) - x;
		int deltaY = static_cast<int>(mouseDelta.Y()) - y;
		rotate(entity.getTransform(), deltaX * -Simplicity::getDeltaTime() * 0.1f,
			Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		rotate(entity.getComponent<Camera>()->getTransform(),
			deltaY * -Simplicity::getDeltaTime() * 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		rotate(entity.getComponents<Mesh>()[1]->getTransform(),
			deltaY * -Simplicity::getDeltaTime() * 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		x = static_cast<int>(mouseDelta.X());
		y = static_cast<int>(mouseDelta.Y());
		mouseDelta = Vector2(0.0f, 0.0f);
	}

	void BobControl::updateY(Entity& entity)
	{
		unique_ptr<Triangle> ground = getGroundAtBobsPosition(entity);

		float groundY = 0.0f;
		if (ground.get() != NULL)
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
