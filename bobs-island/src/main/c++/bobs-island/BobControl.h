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
#ifndef BOBCONTROL_H_
#define BOBCONTROL_H_

#include <simplicity/API.h>

namespace bobsisland
{
	class BobControl : public simplicity::Script, private simplicity::NonCopyable
	{
		public:
			BobControl(const simplicity::Graph& world);

			void destroy();

			void execute();

			void fireGun();

			void init();

		private:
			std::map<simplicity::Keyboard::Button, simplicity::Button::State> buttonStates;

			bool falling;

			float fallTime;

			bool jumping;

			float jumpTime;

			const simplicity::Graph& world;

			int x;

			int y;

			std::unique_ptr<simplicity::Triangle> getGroundAtBobsPosition();

			float getYAtBobsPosition(const simplicity::Triangle& ground);

			float getYForBob(float bobY, float groundY);

			void onKeyboardButton(const void* message);

			void onMouseButton(const void* message);

			void onMouseMove(const void* message);

			void updateY();
	};
}

#endif /* BOBCONTROL_H_ */
