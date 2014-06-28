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
#include <bobs-island/bob/BobFactory.h>
#include <bobs-island/bob/BobLooker.h>
#include <bobs-island/bob/BobMover.h>
#include <bobs-island/bob/BobsCodec.h>
#include <bobs-island/bob/BobShooter.h>
#include <bobs-island/BobConstants.h>

#include "ServerEngine.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	namespace server
	{
		ServerEngine::ServerEngine() :
			bobs()
		{
		}

		void ServerEngine::advance()
		{
			vector<BobState> bobData;

			for (auto bob : bobs)
			{
				BobState bobState;
				bobState.entityTransform = bob.second->getTransform();
				bobState.lookRotation = bob.second->getComponents<Mesh>()[1]->getTransform();
				setPosition(bobState.lookRotation, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
				bobState.systemId = bob.first;
				bobData.push_back(bobState);
			}

			Messages::send(Message(Subject::BOBS, &bobData));
		}

		bool ServerEngine::onClientConnected(const Message& message)
		{
			unique_ptr<Entity> bob = BobFactory::createBob();
			rotate(bob->getTransform(), MathConstants::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
			setPosition(bob->getTransform(), Vector3(0.0f, 0.0f, 63.0f));

			unique_ptr<Component> looker(new BobLooker(message.senderSystemId));
			bob->addUniqueComponent(move(looker));

			unique_ptr<Component> mover(new BobMover(message.senderSystemId,
				*Simplicity::getScene()->getGraph<QuadTree>()));
			bob->addUniqueComponent(move(mover));

			unique_ptr<Component> shooter(new BobShooter(message.senderSystemId));
			bob->addUniqueComponent(move(shooter));

			bobs[message.senderSystemId] = bob.get();

			Simplicity::getScene()->addEntity(move(bob));

			return false;
		}

		void ServerEngine::onPlay()
		{
			Messages::registerRecipient(simplicity::Subject::CLIENT_CONNECTED, bind(&ServerEngine::onClientConnected,
				this, placeholders::_1));

			Messages::registerRecipient(Subject::BOBS, RecipientCategory::CLIENT);

			unique_ptr<Codec> bobsCodec(new BobsCodec);
			Messages::setCodec(Subject::BOBS, move(bobsCodec));
			unique_ptr<Codec> jumpCodec(new SimpleCodec<nullptr_t>);
			Messages::setCodec(Subject::JUMP, move(jumpCodec));
			unique_ptr<Codec> lookCodec(new SimpleCodec<Vector<int, 2>>);
			Messages::setCodec(Subject::LOOK, move(lookCodec));
			unique_ptr<Codec> moveCodec(new SimpleCodec<Direction>);
			Messages::setCodec(Subject::MOVE, move(moveCodec));
			unique_ptr<Codec> shootCodec(new SimpleCodec<nullptr_t>);
			Messages::setCodec(Subject::SHOOT, move(shootCodec));
		}

		void ServerEngine::onStop()
		{
			Messages::deregisterRecipient(simplicity::Subject::CLIENT_CONNECTED, bind(&ServerEngine::onClientConnected,
				this, placeholders::_1));

			Messages::deregisterRecipient(Subject::BOBS, RecipientCategory::CLIENT);
		}
	}
}
