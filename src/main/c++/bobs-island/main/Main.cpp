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
#include <simplicity/Simplicity.h>

#include "../common/Setup.h"
#include "simplicity-generated.h"

using namespace simplicity;

int main()
{
	simplicity_generated_setupEngine();
	simplicity_setupEngine();

	simplicity_generated_setupScene();
	simplicity_setupScene();

	Simplicity::play();

	return 0;
}
