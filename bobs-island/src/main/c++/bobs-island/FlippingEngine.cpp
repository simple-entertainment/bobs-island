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
#include "FlippingEngine.h"

using namespace simplicity;

namespace bobsisland
{
	FlippingEngine::FlippingEngine(Shader& shader) :
		shader(shader)
	{
	}

	void FlippingEngine::addEntity(Entity& /*entity*/)
	{
	}

	void FlippingEngine::advance()
	{
	}

	void FlippingEngine::destroy()
	{
		Messages::deregisterRecipient(Events::APPLY_SHADER, bind(&FlippingEngine::onApplyShader, this,
				placeholders::_1));
	}

	void FlippingEngine::init()
	{
		Messages::registerRecipient(Events::APPLY_SHADER, bind(&FlippingEngine::onApplyShader, this,
				placeholders::_1));
	}

	void FlippingEngine::onApplyShader(const void* message)
	{
		shader.setVar("effectOrigin", Vector3(0.0f, 0.0f, 0.0f));
		shader.setVar("effectSpeed", 5.0f);

		float timeToTravel100m = 100.0f / 5.0f;
		float timesTravelled100m = floor(Simplicity::getTotalTime() / timeToTravel100m);
		shader.setVar("effectTime", Simplicity::getTotalTime() - timesTravelled100m * timeToTravel100m);

		shader.setVar("effectVisibleBeforeFlip", true);
	}

	void FlippingEngine::removeEntity(const Entity& /*entity*/)
	{
	}
}