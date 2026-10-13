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

#ifndef POLICENAUTS_GFX_H
#define POLICENAUTS_GFX_H

#include "common/array.h"
#include "graphics/surface.h"

namespace Policenauts {

const int kScreenWidth = 640;
const int kScreenHeight = 400;

// The location picture and where the original shows it (VRAM offset 0A08h)
const int kPictureWidth = 512;
const int kPictureHeight = 256;
const int kPictureX = 64;
const int kPictureY = 32;

/** A palette as the game stores it: 16 x (B, G, R, 0), 0-31 a channel. */
const int kPaletteSize = 16 * 4;

/**
 * Decodes a 512x256 background into 4-bit indices: two pixels a byte,
 * with the plane bits interleaved as I0 I1 G0 G1 R0 R1 B0 B1.
 */
void decodeBackground(const Common::Array<byte> &data, Graphics::Surface &dst);

/**
 * Converts a game palette to the RGB the PC-98 shows: the game drops the
 * lowest bit of each channel for the 16 levels of the analog palette.
 */
void convertPalette(const byte *palette, byte *rgb);

} // End of namespace Policenauts

#endif // POLICENAUTS_GFX_H
