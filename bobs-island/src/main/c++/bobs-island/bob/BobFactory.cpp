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
		unique_ptr<Entity> createBob()
		{

			unique_ptr<Model> bobBody = ModelFactory::getInstance().createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
				Vector4(1.0f, 0.0f, 0.0f, 1.0f));

			unique_ptr<Model> bobGunArm = ModelFactory::getInstance().createCylinderMesh(0.05f, 0.75f, 10,
				Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			setPosition(bobGunArm->getTransform(), Vector3(0.25f, 0.95f, 0.0f));

			unique_ptr<Script> bobController(new BobController);
			unique_ptr<Script> bobLooker(new BobLooker);
			unique_ptr<Script> bobMover(new BobMover(*Simplicity::getScene()->getGraph<QuadTree>()));
			unique_ptr<Script> bobShooter(new BobShooter);

			unique_ptr<Entity> bob(new Entity);
			bob->addUniqueComponent(move(bobBody));
			bob->addUniqueComponent(move(bobGunArm));
			bob->addUniqueComponent(move(bobController));
			bob->addUniqueComponent(move(bobLooker));
			bob->addUniqueComponent(move(bobMover));
			bob->addUniqueComponent(move(bobShooter));

			return move(bob);
		}
	}
}
