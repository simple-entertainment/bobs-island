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
#include "BobsCodec.h"

using namespace simplicity;
using namespace std;

namespace bobsisland
{
	BobsCodec::BobsCodec() :
		decodedBobs()
	{
	}

	Message BobsCodec::decode(const byte* data)
	{
		Message message;
		unsigned int index = 0;

		memcpy(&message.subject, &data[index], sizeof(unsigned short));
		index += sizeof(unsigned short);

		unsigned int bobCount = 0;
		memcpy(&bobCount, &data[index], sizeof(unsigned int));
		index += sizeof(unsigned int);

		decodedBobs.resize(bobCount);
		message.body = &decodedBobs;

		for (BobState& decodedBob : decodedBobs)
		{
			memcpy(&decodedBob, &data[index], sizeof(BobState));
			index += sizeof(BobState);
		}

		return message;
	}

	vector<byte> BobsCodec::encode(const Message& message)
	{
		const vector<BobState>& bobs = *static_cast<const vector<BobState>*>(message.body);

		unsigned int bobCount = bobs.size();
		vector<byte> data(sizeof(unsigned short) + sizeof(unsigned int) + sizeof(BobState) * bobCount);
		unsigned int index = 0;

		memcpy(&data[index], &message.subject, sizeof(unsigned short));
		index += sizeof(unsigned short);

		memcpy(&data[index], &bobCount, sizeof(unsigned int));
		index += sizeof(unsigned int);

		for (const BobState& bob : bobs)
		{
			memcpy(&data[index], &bob, sizeof(BobState));
			index += sizeof(BobState);
		}

		return data;
	}
}
