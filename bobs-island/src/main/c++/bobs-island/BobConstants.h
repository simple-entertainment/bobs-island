/*
 * Copyright � 2014 Simple Entertainment Limited
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
#ifndef BOBCONSTANTS_H_
#define BOBCONSTANTS_H_

#include <simplicity/API.h>

namespace bobsisland
{
	enum class Direction
	{
		BACKWARD,
		FORWARD,
		LEFT,
		RIGHT
	};

	namespace Subject
	{
		static unsigned short BOBS = simplicity::Subject::USER_ID_0;

		static unsigned short JUMP = simplicity::Subject::USER_ID_0 + 1;

		static unsigned short LOOK = simplicity::Subject::USER_ID_0 + 2;

		static unsigned short MOVE = simplicity::Subject::USER_ID_0 + 3;

		static unsigned short SHOOT = simplicity::Subject::USER_ID_0 + 4;
	}
}

#endif /* BOBCONSTANTS_H_ */