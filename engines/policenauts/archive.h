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

#ifndef POLICENAUTS_ARCHIVE_H
#define POLICENAUTS_ARCHIVE_H

#include "common/array.h"
#include "common/file.h"

namespace Policenauts {

/** Where a pack lies in PNAUTS.WLD. */
struct PackRef {
	uint16 id;
	uint32 sector;
	uint32 size;
};

/** A location's pack, from its LCDDAT.TBL record. */
struct LocationRef {
	PackRef pack;
	uint32 unpackedSize; ///< All the entries unpacked
	uint32 leadingSize;  ///< The bytes of the entries unpacked at load time
};

struct PackEntry {
	byte type;
	byte flags;
	uint16 id;
	uint32 offset;  ///< Within the pack
	uint32 packed;
	uint16 size;    ///< The unpacked size the directory gives
	bool stored;    ///< A 64 KB entry kept as it is, its packed size wrapped to 0
};

/** A pack (SCPK, LDPK or DTPK): a directory of entries and their data. */
class Pack {
public:
	bool load(Common::SeekableReadStream &stream, const PackRef &ref);

	uint32 getTag() const { return _tag; }
	uint16 getId() const { return _id; }
	/** The entries unpacked at load time, those of the leading part. */
	uint16 getLeadingCount() const { return _leadingCount; }
	const Common::Array<PackEntry> &getEntries() const { return _entries; }
	uint32 getSize() const { return _data.size(); }

	/** Whether the entry's data lies within the pack. */
	bool isValid(const PackEntry &entry) const;
	const byte *getEntryData(const PackEntry &entry) const;

	/** The entry's data, decompressed according to its flags. */
	bool unpack(const PackEntry &entry, Common::Array<byte> &out) const;

private:
	uint32 _tag = 0;
	uint16 _id = 0;
	uint16 _leadingCount = 0;
	Common::Array<PackEntry> _entries;
	Common::Array<byte> _data;
};

/** PNAUTS.WLD, the CD's data archive, and the index tables of its packs. */
class Archive {
public:
	bool open();

	bool loadPack(const PackRef &ref, Pack &pack);

	/** The script packs, from SCDDAT.TBL. */
	const Common::Array<PackRef> &getScriptPacks() const { return _scriptPacks; }
	/** The location packs, from LCDDAT.TBL; location n is at n - 1. */
	const Common::Array<LocationRef> &getLocations() const { return _locations; }

private:
	Common::File _wld;
	Common::Array<PackRef> _scriptPacks;
	Common::Array<LocationRef> _locations;
};

} // End of namespace Policenauts

#endif // POLICENAUTS_ARCHIVE_H
