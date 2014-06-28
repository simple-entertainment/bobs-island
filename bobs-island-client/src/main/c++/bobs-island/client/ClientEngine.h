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
#ifndef CLIENTENGINE_H_
#define CLIENTENGINE_H_

#include <simplicity/API.h>

#include <bobs-island/bob/BobState.h>

namespace bobsisland
{
	namespace client
	{
		class ClientEngine : public simplicity::Engine
		{
			public:
				ClientEngine();

				void advance() override;
				
				void onPlay() override;

				void onStop() override;

			private:
				std::map<unsigned long, simplicity::Entity*> bobs;

				std::vector<BobState> bobStates;

				bool onBobs(const simplicity::Message& message);
		};
	}
}

#endif /* CLIENTENGINE_H_ */
