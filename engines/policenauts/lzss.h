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

#ifndef POLICENAUTS_LZSS_H
#define POLICENAUTS_LZSS_H

#include "common/array.h"
#include "common/scummsys.h"

namespace Policenauts {

/**
 * Decompresses the LZSS variant with a 2 KB ring starting at 0x7DE,
 * 11-bit offsets and 5-bit lengths. Stops once dstSize bytes are out,
 * or at the end of the input when dstSize is 0.
 */
void decompressLzss2k(const byte *src, uint32 srcSize, Common::Array<byte> &dst, uint32 dstSize = 0);

/**
 * Decompresses the LZSS variant with a 4 KB ring starting at 0xFEE,
 * 12-bit offsets and 4-bit lengths, used for backgrounds.
 */
void decompressLzss4k(const byte *src, uint32 srcSize, Common::Array<byte> &dst);

/**
 * Swaps bits 3 and 7 of every byte, undoing the scrambling of bytecode,
 * text and frame lists.
 */
void unscramble(Common::Array<byte> &data);

} // End of namespace Policenauts

#endif // POLICENAUTS_LZSS_H
