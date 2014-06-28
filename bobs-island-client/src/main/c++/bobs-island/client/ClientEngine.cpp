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
#include <bobs-island/bob/BobsCodec.h>
#include <bobs-island/BobConstants.h>

#include "bob/BobController.h"
#include "ClientEngine.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	namespace client
	{
		ClientEngine::ClientEngine() :
			bobs(),
			bobStates()
		{
		}

		void ClientEngine::advance()
		{
			for (BobState bobState : bobStates)
			{
				if (bobs.find(bobState.systemId) == bobs.end())
				{
					unique_ptr<Entity> bob = BobFactory::createBob();

					unique_ptr<Component> controller(new BobController);
					bob->addUniqueComponent(move(controller));

					if (bobState.systemId == Simplicity::getId())
					{
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
					}

					bobs[bobState.systemId] = bob.get();
					Simplicity::getScene()->addEntity(move(bob));
				}

				Entity* bob = bobs[bobState.systemId];
				bob->setTransform(bobState.entityTransform);

				Mesh* arm = bob->getComponents<Mesh>()[1];
				Matrix44 armTransform = bobState.lookRotation;
				setPosition(armTransform, getPosition4(arm->getTransform()));
				arm->setTransform(armTransform);

				Camera* camera = bob->getComponent<Camera>();
				if (camera != nullptr)
				{
					Matrix44 cameraTransform = bobState.lookRotation;
					setPosition(cameraTransform, getPosition4(camera->getTransform()));
					camera->setTransform(cameraTransform);
				}

				Simplicity::getScene()->updateGraphs(*bob);
			}
		}

		bool ClientEngine::onBobs(const Message& message)
		{
			bobStates = *static_cast<const vector<BobState>*>(message.body);

			return false;
		}

		void ClientEngine::onPlay()
		{
			Messages::registerRecipient(Subject::BOBS, bind(&ClientEngine::onBobs, this, placeholders::_1));

			Messages::registerRecipient(Subject::JUMP, RecipientCategory::SERVER);
			Messages::registerRecipient(Subject::LOOK, RecipientCategory::SERVER);
			Messages::registerRecipient(Subject::MOVE, RecipientCategory::SERVER);
			Messages::registerRecipient(Subject::SHOOT, RecipientCategory::SERVER);

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

		void ClientEngine::onStop()
		{
			Messages::deregisterRecipient(Subject::BOBS, bind(&ClientEngine::onBobs, this, placeholders::_1));

			Messages::deregisterRecipient(Subject::JUMP, RecipientCategory::SERVER);
			Messages::deregisterRecipient(Subject::LOOK, RecipientCategory::SERVER);
			Messages::deregisterRecipient(Subject::MOVE, RecipientCategory::SERVER);
			Messages::deregisterRecipient(Subject::SHOOT, RecipientCategory::SERVER);
		}
	}
}
