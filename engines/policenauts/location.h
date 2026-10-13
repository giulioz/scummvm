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

#ifndef POLICENAUTS_LOCATION_H
#define POLICENAUTS_LOCATION_H

#include "graphics/surface.h"

#include "policenauts/archive.h"

namespace Policenauts {

/**
 * A location as the location statement loads it (OVL1735): its pack, its
 * background decoded into the picture buffer and its records.
 */
class Location {
public:
	/**
	 * Loads location number (from 1). The background is decoded into the
	 * picture, which keeps what it had when the location has none.
	 */
	bool load(Archive &archive, uint16 number, Graphics::Surface &picture);

	/**
	 * The palette of the location-wide group 8, which the location is shown
	 * with; false when it has none and the fixed palette is used instead.
	 */
	bool getEntryPalette(byte *palette) const;

private:
	/** Unpacks the records entry as 1735:0868 ldpk_unpack_entries does. */
	void unpackRecords(const PackEntry &entry);

	/**
	 * The record a reference {pack, offset} points at, when size bytes of
	 * it are there; the pack is 0 or the location's number.
	 */
	const byte *getRecord(uint32 ref, uint32 size) const;

	uint16 _number = 0;
	Pack _pack;
	Common::Array<byte> _records;
};

} // End of namespace Policenauts

#endif // POLICENAUTS_LOCATION_H
