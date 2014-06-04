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

	void BobLooker::execute(Entity& entity)
	{
		Vector<int, 2> delta = newMousePosition - mousePosition;
		rotate(entity.getTransform(), delta.X() * -Simplicity::getDeltaTime() * 0.1f,
			Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		rotate(entity.getComponent<Camera>()->getTransform(),
			-delta.Y() * Simplicity::getDeltaTime() * 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		rotate(entity.getComponents<Mesh>()[1]->getTransform(),
			-delta.Y() * Simplicity::getDeltaTime() * 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		mousePosition = newMousePosition;

		Simplicity::getScene()->updateGraphs(entity);
	}

	void BobLooker::onCloseScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::deregisterRecipient(Subject::MOUSE_MOVE, bind(&BobLooker::onMouseMove, this,
			placeholders::_1));
	}

	void BobLooker::onMouseMove(const void* message)
	{
		const MouseMoveEvent* event = static_cast<const MouseMoveEvent*>(message);

		newMousePosition.X() = event->x;
		newMousePosition.Y() = event->y;
	}

	void BobLooker::onOpenScene(Scene& /* scene */, Entity& /* entity */)
	{
		Messages::registerRecipient(Subject::MOUSE_MOVE, bind(&BobLooker::onMouseMove, this,
			placeholders::_1));
	}
}
