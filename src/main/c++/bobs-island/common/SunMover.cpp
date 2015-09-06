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
#include "SunMover.h"

using namespace simplicity;

namespace bobsisland
{
	void SunMover::execute(Entity& entity)
	{
		float toPosition = 2.0f * MathConstants::PI * Simplicity::getTotalTime() / 60.0f;
		toPosition -= 0.75f * MathConstants::PI;
		//toPosition = 0.0f;
		Vector3 position(sin(toPosition), cos(toPosition), 0.0f);
		Vector3 direction = position;
		direction.negate();
		position *= 1000.0f;

		setPosition(entity.getTransform(), position);
		entity.getComponent<Light>()->setDirection(direction);
	}

	void SunMover::onAddEntity(Entity& entity)
	{
		execute(entity);
	}
}
