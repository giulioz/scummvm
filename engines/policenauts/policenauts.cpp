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

#include "common/config-manager.h"
#include "common/events.h"
#include "common/file.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/paletteman.h"

#include "policenauts/console.h"
#include "policenauts/location.h"
#include "policenauts/policenauts.h"

namespace Policenauts {

// The PC-98's 400-line mode refreshes at 56.42 Hz
const uint32 kVsyncRateCentiHz = 5642;

// Where ITP.EXE holds the fixed palette, DS:20CEh
const uint32 kFixedPaletteOffset = 0x1DB2E;

PolicenautsEngine *g_engine;

PolicenautsEngine::PolicenautsEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc) {
	g_engine = this;
	memset(_fixedPalette, 0, sizeof(_fixedPalette));
}

PolicenautsEngine::~PolicenautsEngine() {
	_picture.free();
	delete _screen;
}

bool PolicenautsEngine::hasFeature(EngineFeature f) const {
	return f == kSupportsReturnToLauncher;
}

Common::Error PolicenautsEngine::run() {
	initGraphics(kScreenWidth, kScreenHeight);
	_screen = new Graphics::Screen();

	setDebugger(new Console());

	if (!_archive.open())
		return Common::Error(Common::kReadingFailed, "PAC/PNAUTS.WLD");
	if (!loadFixedPalette())
		return Common::Error(Common::kReadingFailed, "ITP.EXE");

	_picture.create(kPictureWidth, kPictureHeight, Graphics::PixelFormat::createFormatCLUT8());
	setScreenPalette(_fixedPalette);

	// Until the script runs, --boot-param shows a location
	int bootParam = ConfMan.getInt("boot_param");
	if (bootParam && !showLocation(bootParam))
		warning("Location %d could not be loaded", bootParam);

	_vsyncBaseTime = g_system->getMillis();
	while (!shouldQuit())
		waitVsync();

	return Common::kNoError;
}

bool PolicenautsEngine::loadFixedPalette() {
	Common::File exe;
	if (!exe.open("ITP.EXE") || !exe.seek(kFixedPaletteOffset))
		return false;
	return exe.read(_fixedPalette, kPaletteSize) == kPaletteSize;
}

void PolicenautsEngine::setScreenPalette(const byte *palette) {
	// 17B4:09B7 screen_palette_store: colour 0 is always white
	byte stored[kPaletteSize];
	memcpy(stored, palette, kPaletteSize);
	stored[0] = stored[1] = stored[2] = 0x1F;

	byte rgb[16 * 3];
	convertPalette(stored, rgb);
	g_system->getPaletteManager()->setPalette(rgb, 0, 16);
}

bool PolicenautsEngine::showLocation(uint16 number) {
	Location location;
	if (!location.load(_archive, number, _picture))
		return false;

	byte palette[kPaletteSize];
	setScreenPalette(location.getEntryPalette(palette) ? palette : _fixedPalette);
	_screen->blitFrom(_picture, Common::Point(kPictureX, kPictureY));
	return true;
}

void PolicenautsEngine::waitVsync() {
	Common::Event event;
	while (g_system->getEventManager()->pollEvent(event)) {
	}

	_screen->update();

	// Deadlines count from a base time so that rounding does not drift
	++_vsyncCount;
	uint32 deadline = _vsyncBaseTime + (uint32)((uint64)_vsyncCount * 100000 / kVsyncRateCentiHz);
	uint32 now = g_system->getMillis();
	if (deadline > now) {
		g_system->delayMillis(deadline - now);
	} else {
		// Running late (a slow frame, or the engine was paused): count
		// from now instead of rushing to catch up
		_vsyncBaseTime = now;
		_vsyncCount = 0;
	}
}

} // End of namespace Policenauts
