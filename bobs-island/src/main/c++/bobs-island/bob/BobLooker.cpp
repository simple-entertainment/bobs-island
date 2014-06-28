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

#include "../BobConstants.h"
#include "BobLooker.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobLooker::BobLooker(unsigned long systemId) :
		delta(0, 0),
		systemId(systemId)
	{
	}

	void BobLooker::execute(Entity& entity)
	{
		if (delta == Vector<int, 2>(0, 0))
		{
			return;
		}

		Vector2 lookDelta(static_cast<float>(delta.X()), static_cast<float>(delta.Y()));
		lookDelta *= -Simplicity::getDeltaTime() * 0.1f;

		rotate(entity.getTransform(), lookDelta.X(), Vector3(0.0f, 1.0f, 0.0f));

		Camera* camera = entity.getComponent<Camera>();
		if (camera != nullptr)
		{
			rotate(camera->getTransform(), lookDelta.Y(), Vector3(1.0f, 0.0f, 0.0f));
		}

		rotate(entity.getComponents<Mesh>()[1]->getTransform(), lookDelta.Y(), Vector3(1.0f, 0.0f, 0.0f));

		Simplicity::getScene()->updateGraphs(entity);

		delta = Vector<int, 2>(0, 0);
	}

	void BobLooker::onAddEntity(Entity& /* entity */)
	{
		Messages::registerRecipient(Subject::LOOK, bind(&BobLooker::onLook, this, placeholders::_1));
	}

	bool BobLooker::onLook(const Message& message)
	{
		if (message.senderSystemId == systemId)
		{
			delta = *static_cast<const Vector<int, 2>*>(message.body);

			return true;
		}

		return false;
	}

	void BobLooker::onRemoveEntity(Entity& /* entity */)
	{
		Messages::deregisterRecipient(Subject::LOOK, bind(&BobLooker::onLook, this, placeholders::_1));
	}
}
