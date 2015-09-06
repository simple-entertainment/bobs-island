/*
* Copyright � 2014 Simple Entertainment Limited
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

#include "BobFactory.h"
#include "BobLooker.h"
#include "BobMover.h"
#include "BobShooter.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	namespace BobFactory
	{
		unique_ptr<Entity> createBob(unsigned long senderSystemId)
		{
			unique_ptr<Entity> bob(new Entity);

			ModelFactory::Recipe bodyRecipe;
			bodyRecipe.shape = ModelFactory::Recipe::Shape::BOX;
			bodyRecipe.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			bodyRecipe.dimensions = Vector3(0.5f, 2.0f, 0.2f);
			unique_ptr<Mesh> bobBody = ModelFactory::cookMesh(bodyRecipe);
			bob->addUniqueComponent(move(bobBody));

			ModelFactory::Recipe gunArmRecipe;
			gunArmRecipe.shape = ModelFactory::Recipe::Shape::CYLINDER;
			gunArmRecipe.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			gunArmRecipe.dimensions = Vector3(1.0f, 0.75f, 0.0f);
			unique_ptr<Mesh> bobGunArm = ModelFactory::cookMesh(gunArmRecipe);
			setPosition(bobGunArm->getTransform(), Vector3(0.25f, 0.95f, 0.0f));
			bob->addUniqueComponent(move(bobGunArm));

			unique_ptr<Component> looker(new BobLooker(senderSystemId));
			bob->addUniqueComponent(move(looker));

			unique_ptr<Component> mover(new BobMover(senderSystemId,
				*Simplicity::getScene()->getGraph<QuadTree>()));
			bob->addUniqueComponent(move(mover));

			unique_ptr<Component> shooter(new BobShooter(senderSystemId));
			bob->addUniqueComponent(move(shooter));

			unique_ptr<Camera> camera(new Camera);
			camera->setFarClippingDistance(2000.0f);
			camera->setPerspective(60.0f, 4.0f / 3.0f);
			translate(camera->getTransform(), Vector3(0.0f, 1.11f, -0.21f)); // <- Camera position is relative to Bob.
			bob->addUniqueComponent(move(camera));

			// Yes, this is odd... Bob's bounds are used as the camera bounds... for now...
			unique_ptr<Model> cameraBounds(new Square(32.0f));
			setPosition(cameraBounds->getTransform(), Vector3(0.0f, 0.0f, -32.0f));
			cameraBounds->setCategory(Category::BOUNDS);
			bob->addUniqueComponent(move(cameraBounds));

			// Switch to Bob's camera
			Simplicity::getEngine<RenderingEngine>()->setCamera(bob.get());

			return move(bob);
		}
	}
}
