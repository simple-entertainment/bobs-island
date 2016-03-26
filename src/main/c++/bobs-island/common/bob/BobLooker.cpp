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

#include "BobLooker.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobLooker::BobLooker() :
		mousePosition(0, 0),
		newMousePosition(0, 0)
	{
	}

	void BobLooker::execute()
	{
		Vector2i delta = newMousePosition - mousePosition;
		mousePosition = newMousePosition;

		if (delta == Vector2i(0, 0))
		{
			return;
		}

		Vector2 lookDelta(static_cast<float>(delta.X()), static_cast<float>(delta.Y()));
		lookDelta *= -Simplicity::getDeltaTime() * 0.1f;

		getEntity()->rotate(lookDelta.X(), Vector3(0.0f, 1.0f, 0.0f));

		Camera* camera = getEntity()->getComponent<Camera>();
		if (camera != nullptr)
		{
			rotate(camera->getTransform(), lookDelta.Y(), Vector3(1.0f, 0.0f, 0.0f));
		}

		rotate(getEntity()->getComponents<Mesh>()[1]->getTransform(), lookDelta.Y(), Vector3(1.0f, 0.0f, 0.0f));
	}

	void BobLooker::onAddEntity()
	{
		Messages::registerRecipient(Subject::MOUSE_MOVE, bind(&BobLooker::onMouseMove, this, placeholders::_1));
	}

	bool BobLooker::onMouseMove(const Message& message)
	{
		const MouseMoveEvent* event = static_cast<const MouseMoveEvent*>(message.body);

		newMousePosition.X() = event->x;
		newMousePosition.Y() = event->y;

		return false;
	}

	void BobLooker::onRemoveEntity()
	{
		Messages::deregisterRecipient(Subject::MOUSE_MOVE, bind(&BobLooker::onMouseMove, this, placeholders::_1));
	}
}
