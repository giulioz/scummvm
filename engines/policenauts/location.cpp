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
#include "common/textconsole.h"

#include "policenauts/detection.h"
#include "policenauts/gfx.h"
#include "policenauts/location.h"

namespace Policenauts {

// A pack this big unpacked is only unpacked up to its leading part at load
// time, the rest when a record asks for it
const uint32 kWholePackLimit = 0x28000;

bool Location::load(Archive &archive, uint16 number, Graphics::Surface &picture) {
	const Common::Array<LocationRef> &locations = archive.getLocations();
	if (number == 0 || number > locations.size())
		return false;

	const LocationRef &ref = locations[number - 1];
	if (!archive.loadPack(ref.pack, _pack))
		return false;
	// 1735:0765 ldpk_header_parse
	if (_pack.getTag() != MKTAG('L', 'D', 'P', 'K') || _pack.getId() != ref.pack.id) {
		warning("Location %d: pack %d is not its LDPK", number, ref.pack.id);
		return false;
	}
	_number = number;

	const Common::Array<PackEntry> &entries = _pack.getEntries();
	uint last = ref.unpackedSize <= kWholePackLimit ? entries.size() : _pack.getLeadingCount();
	last = MIN<uint>(last, entries.size());

	// 1735:0868 ldpk_unpack_entries: the backgrounds go to the picture buffer
	for (uint i = 0; i < last; i++) {
		const PackEntry &e = entries[i];
		if (!(e.flags & 0x10))
			continue;
		Common::Array<byte> data;
		if (!_pack.unpack(e, data)) {
			warning("Location %d: background %d lies past the pack's end", number, e.id);
			continue;
		}
		decodeBackground(data, picture);
	}

	// 1735:03FA location_find_records: the last type-3 entry with the
	// location's number, among those before the first type-4 entry that is
	// not a background
	_records.clear();
	for (uint i = 0; i < entries.size(); i++) {
		const PackEntry &e = entries[i];
		if ((e.type & 15) == 4 && !(e.flags & 0x10))
			break;
		if ((e.type & 15) == 3 && e.id == number)
			unpackRecords(e);
	}

	debugC(1, kDebugResource, "Location %d: pack %d, %d entries, %d bytes of records",
		number, ref.pack.id, entries.size(), _records.size());
	return true;
}

void Location::unpackRecords(const PackEntry &entry) {
	_records.clear();
	const byte *d = _pack.getEntryData(entry);
	if (!d) {
		warning("Location %d: records %d lie past the pack's end", _number, entry.id);
		return;
	}

	if (entry.flags & 0x20) {
		// Packed, unless the entry is too small to be
		if (entry.packed >= 0x10)
			_pack.unpack(entry, _records);
	} else {
		// Kept as it is: the directory's size, up to the pack's end
		_records.assign(d, d + MIN<uint32>(entry.size, _pack.getSize() - entry.offset));
	}
}

const byte *Location::getRecord(uint32 ref, uint32 size) const {
	uint16 pack = ref & 0xFFFF;
	uint16 offset = ref >> 16;
	if (pack == 0xFFFF || (pack != 0 && pack != _number))
		return nullptr;
	if (offset > _records.size() || size > _records.size() - offset)
		return nullptr;
	return _records.data() + offset;
}

bool Location::getEntryPalette(byte *palette) const {
	// The root is the records' first; 17DD:02AF loc_group_find walks the
	// location-wide groups from its +18h through each group's +2
	const byte *root = getRecord(_number, 0x1C);
	if (!root)
		return false;

	const byte *group = getRecord(READ_LE_UINT32(root + 0x18), 0x16);
	for (uint steps = 0; group && READ_LE_UINT16(group + 0xA) != 8; steps++) {
		if (steps > _records.size())
			return false;
		group = getRecord(READ_LE_UINT32(group + 2), 0x16);
	}
	if (!group)
		return false;

	// 17B4:0F3E: the palette of the group's first item
	const byte *item = getRecord(READ_LE_UINT32(group + 0x12), 0x12 + kPaletteSize);
	if (!item)
		return false;
	memcpy(palette, item + 0x12, kPaletteSize);
	return true;
}

} // End of namespace Policenauts
