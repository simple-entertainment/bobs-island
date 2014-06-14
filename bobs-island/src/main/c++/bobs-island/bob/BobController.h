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
	namespace Action
	{
		static unsigned short JUMP = simplicity::Subject::USER_ID_0;

		static unsigned short LOOK = simplicity::Subject::USER_ID_0 + 1;

		static unsigned short MOVE = simplicity::Subject::USER_ID_0 + 2;

		static unsigned short SHOOT = simplicity::Subject::USER_ID_0 + 3;
	}

	class BobController : public simplicity::Script
	{
		public:
			enum class Direction
			{
				BACKWARD,
				FORWARD,
				LEFT,
				RIGHT
			};

			BobController();

			void execute(simplicity::Entity& entity);

			void onCloseScene(simplicity::Scene& scene, simplicity::Entity& entity) override;

			void onOpenScene(simplicity::Scene& scene, simplicity::Entity& entity) override;

		private:
			std::map<simplicity::Keyboard::Button, simplicity::Button::State> keyboardButtonStates;

			simplicity::Vector<int, 2> mousePosition;

			simplicity::Vector<int, 2> newMousePosition;

			bool shooting;

			void onKeyboardButton(const void* message);

			void onMouseButton(const void* message);

			void onMouseMove(const void* message);
	};
}

#endif /* BOBCONTROLLER_H_ */
