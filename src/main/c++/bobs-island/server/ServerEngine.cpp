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
#include <simplicity/terrain/API.h>

#include "bob/BobFactory.h"
#include "ServerEngine.h"

using namespace simplicity;
using namespace simplicity::terrain;
using namespace std;

namespace bobsisland
{
	namespace server
	{
		ServerEngine::ServerEngine()
		{
		}

		void ServerEngine::advance()
		{
		}

		bool ServerEngine::onClientConnected(const Message& message)
		{
			float unitLength = 1.0f;

			unique_ptr<Entity> bob = BobFactory::createBob(message.senderSystemId);
			//rotate(bob->getTransform(), MathConstants::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
			setPosition(bob->getTransform(), Vector3(9.0f * unitLength, 0.0f, 295.0f * unitLength));

			unique_ptr<Entity> terrain(new Entity(111));
			Resource* terrainFile = Resources::get("assets/island.terrain", Category::UNCATEGORIZED, true);
			//unique_ptr<TerrainStreamer> terrainStreamer(
			//		new TerrainStreamer(*terrainFile, Vector2ui(1024, 1024), Vector2ui(100, 200), unitLength));
			unique_ptr<LODTerrainStreamer> terrainStreamer(
					new LODTerrainStreamer(*terrainFile, Vector2ui(1024, 1024),
										   { Vector2ui(64, 64), Vector2ui(256, 256), Vector2ui(1024, 1024) },
										   { 1, 4, 16 }));
			terrainStreamer->setTrackedEntity(*bob);
			terrain->addUniqueComponent(move(terrainStreamer));
			Simplicity::getScene()->addEntity(move(terrain));

			Simplicity::getScene()->addEntity(move(bob));

			// The Sky!
			/////////////////////////
			unique_ptr<Entity> sky(new Entity);
			rotate(sky->getTransform(), MathConstants::PI * -0.5f, Vector3(1.0f, 0.0f, 0.0f));

			unique_ptr<Mesh> skyMesh = ModelFactory::getInstance()->createHemisphereMesh(1100.0f, 20,
																						 shared_ptr<MeshBuffer>(), Vector4(0.0f, 0.5f, 0.75f, 1.0f), true);

			sky->addUniqueComponent(move(skyMesh));
			Simplicity::getScene()->addEntity(move(sky));

			// The Ocean!
			/////////////////////////
			unique_ptr<Entity> ocean(new Entity);
			rotate(ocean->getTransform(), MathConstants::PI * -0.5f, Vector3(1.0f, 0.0f, 0.0f));

			unique_ptr<Mesh> oceanMesh =
					ModelFactory::getInstance()->createCylinderMesh(1200.0f, 500.0f, 20, shared_ptr<MeshBuffer>(),
																	Vector4(0.0f, 0.4f, 0.6f, 1.0f), true);

			ocean->addUniqueComponent(move(oceanMesh));
			Simplicity::getScene()->addEntity(move(ocean));

			return false;
		}

		void ServerEngine::onPlay()
		{
			Messages::registerRecipient(Subject::CLIENT_CONNECTED, bind(&ServerEngine::onClientConnected, this,
					placeholders::_1));

#ifdef BOB_AS_CLIENT
			onClientConnected(Message());
#endif
		}

		void ServerEngine::onStop()
		{
			Messages::deregisterRecipient(Subject::CLIENT_CONNECTED, bind(&ServerEngine::onClientConnected, this,
					placeholders::_1));
		}
	}
}
