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

#include "policenauts/lzss.h"

namespace Policenauts {

template<uint RING_SIZE, uint RING_START, byte OFFSET_HIGH_MASK, int OFFSET_HIGH_SHIFT, byte LENGTH_MASK>
void decompressLzss(const byte *src, uint32 srcSize, Common::Array<byte> &dst, uint32 dstSize) {
	byte ring[RING_SIZE];
	memset(ring, 0, RING_SIZE);
	uint r = RING_START;
	uint32 i = 0;
	uint flags = 0;
	int bitsLeft = 0;

	dst.clear();
	if (dstSize)
		dst.reserve(dstSize);

	// A match is copied whole, so the output can run past dstSize by up to
	// one match, as it does in the original
	while (i < srcSize && (!dstSize || dst.size() < dstSize)) {
		if (bitsLeft == 0) {
			flags = src[i++];
			bitsLeft = 8;
		}

		if (flags & 1) {
			if (i >= srcSize)
				break;
			byte c = src[i++];
			dst.push_back(c);
			ring[r] = c;
			r = (r + 1) & (RING_SIZE - 1);
		} else {
			if (i + 1 >= srcSize)
				break;
			byte lo = src[i];
			byte hi = src[i + 1];
			i += 2;
			uint offset = lo | ((hi & OFFSET_HIGH_MASK) << OFFSET_HIGH_SHIFT);
			uint count = (hi & LENGTH_MASK) + 3;
			for (uint k = 0; k < count; k++) {
				byte c = ring[(offset + k) & (RING_SIZE - 1)];
				dst.push_back(c);
				ring[r] = c;
				r = (r + 1) & (RING_SIZE - 1);
			}
		}

		flags >>= 1;
		bitsLeft--;
	}
}

void decompressLzss2k(const byte *src, uint32 srcSize, Common::Array<byte> &dst, uint32 dstSize) {
	decompressLzss<2048, 0x7DE, 0xE0, 3, 0x1F>(src, srcSize, dst, dstSize);
}

void decompressLzss4k(const byte *src, uint32 srcSize, Common::Array<byte> &dst) {
	decompressLzss<4096, 0xFEE, 0xF0, 4, 0x0F>(src, srcSize, dst, 0);
}

void unscramble(Common::Array<byte> &data) {
	for (uint i = 0; i < data.size(); i++) {
		byte b = data[i];
		data[i] = (b & 0x77) | ((b & 0x80) >> 4) | ((b & 0x08) << 4);
	}
}

} // End of namespace Policenauts
