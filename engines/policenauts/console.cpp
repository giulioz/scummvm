/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "policenauts/console.h"
#include "policenauts/policenauts.h"

namespace Policenauts {

Console::Console() : GUI::Debugger() {
	registerCmd("location", WRAP_METHOD(Console, Cmd_location));
}

Console::~Console() {
}

bool Console::Cmd_location(int argc, const char **argv) {
	if (argc != 2) {
		debugPrintf("Usage: %s <number>\n", argv[0]);
		return true;
	}

	if (!g_engine->showLocation(atoi(argv[1]))) {
		debugPrintf("Location %s could not be loaded\n", argv[1]);
		return true;
	}
	// Leave the console so that the location shows
	return false;
}

} // End of namespace Policenauts
