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

#include "policenauts/gfx.h"

namespace Policenauts {

void decodeBackground(const Common::Array<byte> &data, Graphics::Surface &dst) {
	uint n = MIN<uint>(data.size(), kPictureWidth * kPictureHeight / 2);
	for (uint i = 0; i < n; i++) {
		byte b = data[i];
		byte *p = (byte *)dst.getBasePtr((i * 2) % kPictureWidth, (i * 2) / kPictureWidth);
		p[0] = ((b >> 7) & 1) << 3 | ((b >> 5) & 1) << 2 | ((b >> 3) & 1) << 1 | ((b >> 1) & 1);
		p[1] = ((b >> 6) & 1) << 3 | ((b >> 4) & 1) << 2 | ((b >> 2) & 1) << 1 | (b & 1);
	}
}

void convertPalette(const byte *palette, byte *rgb) {
	for (int i = 0; i < 16; i++) {
		rgb[i * 3 + 0] = (palette[i * 4 + 2] >> 1) * 17;
		rgb[i * 3 + 1] = (palette[i * 4 + 1] >> 1) * 17;
		rgb[i * 3 + 2] = (palette[i * 4 + 0] >> 1) * 17;
	}
}

} // End of namespace Policenauts
