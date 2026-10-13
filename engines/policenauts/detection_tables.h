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

namespace Policenauts {

const PlainGameDescriptor policenautsGames[] = {
	{ "policenauts", "Policenauts" },
	{ nullptr, nullptr }
};

const ADGameDescription gameDescriptions[] = {
	{
		// PC-9821 CD-ROM release. PAC and PCM come from the CD's data
		// track, ITP.EXE from the system disk.
		"policenauts",
		nullptr,
		AD_ENTRY3s("PAC/PNAUTS.WLD", "8abc93fb345994b6860ff6cd2eef24bd", 14602240,
				   "PCM/PNAUTS.PCM", "89a4aa0243320ad8f7368070693c461a", 366342144,
				   "ITP.EXE", "46e7176890b01b7b42417bb2e849389a", 670384),
		Common::JA_JPN,
		Common::kPlatformPC98,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NOMIDI)
	},

	AD_TABLE_END_MARKER
};

} // End of namespace Policenauts
