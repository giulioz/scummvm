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

#include "common/debug.h"
#include "common/endian.h"

#include "policenauts/archive.h"
#include "policenauts/detection.h"
#include "policenauts/lzss.h"

namespace Policenauts {

const uint32 kSectorSize = 2048;
const uint32 kPackHeaderSize = 16;
const uint32 kPackEntrySize = 16;

bool Pack::load(Common::SeekableReadStream &stream, const PackRef &ref) {
	_entries.clear();
	if (ref.size < kPackHeaderSize)
		return false;
	_data.resize(ref.size);
	if (!stream.seek((int64)ref.sector * kSectorSize) || stream.read(_data.data(), ref.size) != ref.size)
		return false;

	_tag = READ_BE_UINT32(&_data[0]);
	_id = READ_LE_UINT16(&_data[8]);
	uint16 count = READ_LE_UINT16(&_data[10]);
	_leadingCount = READ_LE_UINT16(&_data[12]);
	if (kPackHeaderSize + count * kPackEntrySize > ref.size)
		return false;

	for (uint i = 0; i < count; i++) {
		const byte *h = &_data[kPackHeaderSize + i * kPackEntrySize];
		PackEntry e;
		e.type = h[0];
		e.flags = h[1];
		e.id = READ_LE_UINT16(h + 2);
		e.offset = READ_LE_UINT32(h + 4);
		e.packed = READ_LE_UINT16(h + 8);
		e.size = READ_LE_UINT16(h + 10);
		e.stored = false;
		if (e.packed == 0) {
			// A 64 KB entry runs up to the next entry or the pack's end
			uint32 end = _data.size();
			for (uint k = 0; k < count; k++) {
				uint32 o = READ_LE_UINT32(&_data[kPackHeaderSize + k * kPackEntrySize + 4]);
				if (o > e.offset && o < end)
					end = o;
			}
			e.packed = end > e.offset ? end - e.offset : 0;
			e.stored = true;
		}
		_entries.push_back(e);
	}

	debugC(2, kDebugResource, "Pack %s %d: %d entries, %d leading", tag2str(_tag), _id, count, _leadingCount);
	return true;
}

bool Pack::isValid(const PackEntry &entry) const {
	return entry.offset <= _data.size() && entry.packed <= _data.size() - entry.offset;
}

const byte *Pack::getEntryData(const PackEntry &entry) const {
	return isValid(entry) ? _data.data() + entry.offset : nullptr;
}

bool Pack::unpack(const PackEntry &entry, Common::Array<byte> &out) const {
	const byte *d = getEntryData(entry);
	if (!d)
		return false;

	if (entry.stored) {
		out.assign(d, d + entry.packed);
	} else if (entry.flags & 0x10) {
		decompressLzss4k(d, entry.packed, out);
	} else if (entry.flags & 0x20) {
		if (entry.packed < 2)
			return false;
		decompressLzss2k(d + 2, entry.packed - 2, out, READ_LE_UINT16(d));
		unscramble(out);
	} else if (entry.flags & 0x40) {
		// A sprite: the size is given in 8-byte units
		if (entry.packed < 2)
			return false;
		decompressLzss2k(d + 2, entry.packed - 2, out, READ_LE_UINT16(d) * 8);
	} else {
		out.assign(d, d + entry.packed);
	}
	return true;
}

bool Archive::open() {
	if (!_wld.open("PAC/PNAUTS.WLD"))
		return false;

	Common::File table;
	if (!table.open("PAC/SCDDAT.TBL"))
		return false;
	while (table.size() - table.pos() >= 14) {
		PackRef ref;
		ref.sector = table.readUint32LE();
		table.skip(4);
		ref.id = table.readUint16LE();
		ref.size = table.readUint32LE();
		_scriptPacks.push_back(ref);
	}
	table.close();

	if (!table.open("PAC/LCDDAT.TBL"))
		return false;
	while (table.size() - table.pos() >= 22) {
		LocationRef ref;
		ref.pack.sector = table.readUint32LE();
		table.skip(4);
		ref.pack.id = table.readUint16LE();
		ref.pack.size = table.readUint32LE();
		ref.unpackedSize = table.readUint32LE();
		ref.leadingSize = table.readUint32LE();
		_locations.push_back(ref);
	}

	debugC(1, kDebugResource, "PNAUTS.WLD: %d script packs, %d locations", _scriptPacks.size(), _locations.size());
	return !_scriptPacks.empty() && !_locations.empty();
}

bool Archive::loadPack(const PackRef &ref, Pack &pack) {
	return pack.load(_wld, ref);
}

} // End of namespace Policenauts
