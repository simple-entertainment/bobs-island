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

#include "BobController.h"
#include "BobShooter.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobShooter::BobShooter() :
		firing(false)
	{
	}

	void BobShooter::execute(Entity& entity)
	{
		if (firing)
		{
			fireGun(entity);
		}
	}

	void BobShooter::fireGun(Entity& entity)
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

	void BobShooter::onCloseScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::deregisterRecipient(Action::SHOOT2, bind(&BobShooter::onShoot, this, placeholders::_1));
	}

	void BobShooter::onOpenScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::registerRecipient(Action::SHOOT2, bind(&BobShooter::onShoot, this, placeholders::_1));
	}

	void BobShooter::onShoot(const void* /* message */)
	{
		firing = true;
	}
}
