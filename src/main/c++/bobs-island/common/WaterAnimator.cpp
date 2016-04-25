/*      _                 _ _      _ _
 *     (_)               | (_)    (_) |
 *  ___ _ _ __ ___  _ __ | |_  ___ _| |_ _   _
 * / __| | '_ ` _ \| '_ \| | |/ __| | __| | | |
 * \__ \ | | | | | | |_) | | | (__| | |_| |_| |
 * |___/_|_| |_| |_| .__/|_|_|\___|_|\__|\__, |
 *                 | |                    __/ |
 *                 |_|                   |___/
 *
 * This file is part of simplicity. See the LICENSE file for the full license governing this code.
 */
#include "WaterAnimator.h"

using namespace simplicity;

namespace bobsisland
{
	void WaterAnimator::execute()
	{
		Pipeline* pipeline = getEntity()->getComponent<Model>()->getMesh()->getBuffer()->getPipeline();
		pipeline->apply();
		pipeline->set("time", Simplicity::getTotalTime());
	}

	void WaterAnimator::onAddEntity()
	{
		execute();
	}
}
