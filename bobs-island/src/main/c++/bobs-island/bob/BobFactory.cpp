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

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	namespace BobFactory
	{
		unique_ptr<Entity> createBob()
		{
			unique_ptr<Entity> bob(new Entity);

			unique_ptr<Mesh> bobBody = ModelFactory::getInstance()->createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
				shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			bob->addUniqueComponent(move(bobBody));

			unique_ptr<Mesh> bobGunArm = ModelFactory::getInstance()->createCylinderMesh(0.05f, 0.75f, 10,
					shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			setPosition(bobGunArm->getTransform(), Vector3(0.25f, 0.95f, 0.0f));
			bob->addUniqueComponent(move(bobGunArm));

			return move(bob);
		}
	}
}
