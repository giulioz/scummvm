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

#ifndef POLICENAUTS_H
#define POLICENAUTS_H

#include "common/scummsys.h"
#include "engines/engine.h"
#include "graphics/screen.h"

#include "policenauts/archive.h"
#include "policenauts/detection.h"
#include "policenauts/gfx.h"

namespace Policenauts {

class PolicenautsEngine : public Engine {
private:
	const ADGameDescription *_gameDescription;
	uint32 _vsyncBaseTime = 0;
	uint32 _vsyncCount = 0;

	Archive _archive;
	/** The fixed palette (DS:20CEh), for locations without their own. */
	byte _fixedPalette[kPaletteSize];
	/** The picture buffer location backgrounds are decoded into. */
	Graphics::Surface _picture;

	bool loadFixedPalette();
	/** Stores and shows a palette as 17B4:0A18 screen_palette does. */
	void setScreenPalette(const byte *palette);

protected:
	// Engine APIs
	Common::Error run() override;

public:
	Graphics::Screen *_screen = nullptr;

public:
	PolicenautsEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~PolicenautsEngine() override;

	bool hasFeature(EngineFeature f) const override;

	uint32 getFeatures() const {
		return _gameDescription->flags;
	}

	/**
	 * Waits for the next vertical retrace, the point where the original
	 * game waits. Processes events and updates the screen.
	 */
	void waitVsync();

	/**
	 * Loads a location and shows its picture with its palette, as the
	 * location statement (17FE:03DF) leaves them.
	 */
	bool showLocation(uint16 number);
};

extern PolicenautsEngine *g_engine;

} // End of namespace Policenauts

#endif // POLICENAUTS_H
