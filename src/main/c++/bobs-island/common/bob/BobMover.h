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
#ifndef BOBMOVER_H_
#define BOBMOVER_H_

#include <simplicity/API.h>

namespace bobsisland
{
	class BobMover : public simplicity::Script
	{
		public:
			BobMover();

			void execute() override;

			void onAddEntity() override;

			void onRemoveEntity() override;

		private:
			bool falling;

			float fallTime;

			bool jumping;

			float jumpTime;

			std::map<simplicity::Keyboard::Button, simplicity::Button::State> keyboardButtonStates;

			float getYForBob(float bobY, float groundY);

			bool onKeyboardButton(const simplicity::Message& message);

			void updateY();
	};
}

#endif /* BOBMOVER_H_ */
