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
#ifndef BOBFACTORY_H_
#define BOBFACTORY_H_

#include <simplicity/API.h>

namespace bobsisland
{
	namespace BobFactory
	{
		std::unique_ptr<simplicity::Entity> createBob(unsigned long senderSystemId);
	}
}

#endif /* BOBFACTORY_H_ */
