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

#include "BobShooter.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobShooter::BobShooter() :
		firing(false)
	{
	}

	void BobShooter::execute()
	{
		if (firing)
		{
			fireGun();
		}
	}

	void BobShooter::fireGun()
	{
		unique_ptr<Entity> bullet(new Entity);
		bullet->setTransform(getEntity()->getTransform() * getEntity()->getComponents<Model>()[1]->getTransform());
		rotate(bullet->getTransform(), MathConstants::PI * -0.5f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		ModelFactory::Recipe recipe;
		recipe.shape = ModelFactory::Recipe::Shape::PYRAMID;
		recipe.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		recipe.dimensions = Vector3(0.2f, 0.5f, 0.0f);
		shared_ptr<Mesh> mesh = ModelFactory::cookMesh(recipe);

		const MeshData& meshData = mesh->getData();
		unique_ptr<Shape> bounds = ModelFunctions::getSquareBoundsXZ(meshData.vertexData, meshData.vertexCount);
		mesh->releaseData();

		Body::Material material;
		material.mass = 0.2f;
		material.friction = 0.5f;
		material.restitution = 0.1f;

		Box bodyBounds(0.1f, 0.25f, 0.1f);

		unique_ptr<Body> body = PhysicsFactory::createBody(material, *mesh, bodyBounds, bullet->getTransform());

		Vector3 trajectory = getUp3(bullet->getTransform());
		trajectory.normalize();
		trajectory *= 50.0f;
		body->applyForce(trajectory, Vector3(0.0f, 0.0f, 0.0f));

		unique_ptr<Model> model(new Model);
		model->setBounds(move(bounds));
		model->setMesh(move(mesh));

		bullet->addComponent(move(model));
		bullet->addComponent(move(body));

		Simplicity::getScene()->addEntity(move(bullet));

		firing = false;
	}

	void BobShooter::onAddEntity()
	{
		Messages::registerRecipient(Subject::MOUSE_BUTTON, bind(&BobShooter::onMouseButton, this, placeholders::_1));
	}

	bool BobShooter::onMouseButton(const Message& message)
	{
		const MouseButtonEvent* event = static_cast<const MouseButtonEvent*>(message.body);
		if (event->button == Mouse::Button::LEFT && event->buttonState == Button::State::UP)
		{
			firing = true;
		}

		return false;
	}

	void BobShooter::onRemoveEntity()
	{
		Messages::deregisterRecipient(Subject::MOUSE_BUTTON, bind(&BobShooter::onMouseButton, this, placeholders::_1));
	}
}
