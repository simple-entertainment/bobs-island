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
#ifndef BOBCONTROLLER_H_
#define BOBCONTROLLER_H_

#include <simplicity/API.h>

namespace bobsisland
{
	class BobController : public simplicity::Script
	{
		public:
			BobController();

			void execute(simplicity::Entity& entity) override;

			void onAddEntity(simplicity::Entity& entity) override;
			
			void onRemoveEntity(simplicity::Entity& entity) override;

		private:
			std::map<simplicity::Keyboard::Button, simplicity::Button::State> keyboardButtonStates;

			simplicity::Vector<int, 2> mousePosition;

			simplicity::Vector<int, 2> newMousePosition;

			bool shooting;

			bool onKeyboardButton(const simplicity::Message& message);

			bool onMouseButton(const simplicity::Message& message);

			bool onMouseMove(const simplicity::Message& message);
	};
}

#endif /* BOBCONTROLLER_H_ */
