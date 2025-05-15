#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#include "block.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <vector>

namespace hill {

	enum class file_version {
		NONE = 0,
		V0_1,
	};

	inline const char *file_version_str(file_version v)
	{
		switch (v) { // MAX 9 characters!
		case file_version::NONE: return "NONE";
		case file_version::V0_1: return "0.1";
		default: return "UNKNOWN";
		}
	}

	enum class serializer_mode {
		NONE,
		BIN,
		ASCII
	};

	struct serializer {

		serializer_mode mode;

		serializer(serializer_mode mode) : mode(mode) {}

		bool serialize(
			const std::filesystem::path &fpath,
			const block &b)
		{
			std::ofstream ofs(fpath, std::ios::binary);
			if (!ofs.is_open()) {
				return false;
			}

			bool success = false;

			switch (mode) {
			case serializer_mode::BIN: success=bin(ofs, b); break;
			case serializer_mode::ASCII: success=ascii(ofs, b); break;
			default: success=false; break;
			}

			ofs.flush();
			ofs.close();

			return success;
		}

	private:
		enum class dt_flag {
			MUT = 1 << 0,
		};

		enum class offset_table {
			LITERALS,
			INSTRUCTIONS,
		};

		template<typename T> static void write_bin(T v, std::ofstream &ofs)
		{
			ofs.write((const char *)&v, sizeof v);
		}

		static void data_type_ascii(const type_spec &ts, std::ofstream &ofs)
		{
/*			ofs << ' ' << basic_type_str(dt.bt);
			ofs << ":mut=" << (dt.mut ? '1' : '0');*/
		}

		static void data_type_bin(const type_spec &ts, std::ofstream &ofs)
		{
/*			uint16_t flags = (dt.mut ? (uint16_t)dt_flag::MUT : 0);

			write_bin(static_cast<uint16_t>(dt.bt), ofs);
			write_bin(flags, ofs);*/
		}

		static void instr_ascii(const instr &ins, std::ofstream &ofs)
		{
			if (ins.op == op_code::ID) {
				return;
			}

			ofs << op_code_str(ins.op);
			data_type_ascii(ins.res_ts, ofs);

			switch (ins.op) {
			case op_code::END:
				break;
			case op_code::LOAD:
				ofs << ' ' << ins.ix;
				break;
			case op_code::COPY:
				ofs << ' ' << ins.ix;
				data_type_ascii(ins.arg2_ts, ofs);
				break;
			default:
				data_type_ascii(ins.arg1_ts, ofs);
				data_type_ascii(ins.arg2_ts, ofs);
				break;
			}
			ofs << '\n';
		}

		static void instr_bin(const instr &ins, std::ofstream &ofs)
		{
			write_bin(static_cast<uint16_t>(ins.op), ofs);

			data_type_bin(ins.res_ts, ofs);

			switch (ins.op) {
			case op_code::LOAD:
				write_bin(static_cast<uint64_t>(ins.ix), ofs);
				write_bin<uint32_t>(0u, ofs);
				write_bin<uint32_t>(0u, ofs);
				break;
			case op_code::COPY:
				write_bin(static_cast<uint64_t>(ins.ix), ofs);
				data_type_bin(ins.arg2_ts, ofs);
				break;
			default:
				data_type_bin(ins.arg1_ts, ofs);
				data_type_bin(ins.arg2_ts, ofs);
				break;
			}
		}

		// File extention .hill_c.txt?
		static bool ascii(
			std::ofstream &ofs,
			const block &b)
		{
			auto now = std::chrono::system_clock::now();

			ofs << "HILL "
				<< 'V' << file_version_str(file_version::V0_1)
				<< ' ' << std::format("{:%FT%TZ}", now) // ISO 8601
				<< '\n';

			ofs << "#LITERALS";
			ofs << std::hex << std::setfill('0') << std::uppercase;
			for (size_t i=0; i < b.values.mem.size(); ++i) {
				ofs << ((i%32 == 0) ? '\n' : ' ');
				ofs << std::setw(2) << static_cast<size_t>(b.values.mem[i]);
			}
			ofs << std::dec << std::setfill(' ') << std::nouppercase;
			ofs << "\n";

			ofs << "#INSTRS\n";
			for (const auto &ins : b.instrs) {
				instr_ascii(ins, ofs);
			}

			return true;
		}

		// File extention .hill_c?
		static bool bin(
			std::ofstream &ofs,
			const block &b)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			ofs << std::setfill('\0');

			// Header (16 bytes)
			ofs << std::left << "HILL";
			ofs << '-'
				<< std::setw(9)
				<< std::left
				<< file_version_str(file_version::V0_1)
				<< std::right // 10 bytes (Version str)
				<< '\n';

			write_bin((uint16_t)file_version::V0_1, ofs); // 2 bytes (File version)
			for (size_t i=0; i<38; ++i) { // x bytes (Reserved)
				write_bin<uint8_t>(0u, ofs);
			}
			write_bin(static_cast<uint64_t>(now), ofs); // 8 bytes (Timestamp)

			// TODO: Offset table (Offsets are relative to after this table?)
			/**
			 * Offset table format:
			 *   Table entry count (1 byte)
			 * 
			 *   Entry identifier (1 byte)
			 *   Offset (8 byte)
			 *   ...
			 */
			write_bin<uint8_t>(2, ofs);
			write_bin((uint8_t)offset_table::LITERALS, ofs);
			write_bin<uint64_t>(0u, ofs); // TODO
			write_bin((uint8_t)offset_table::INSTRUCTIONS, ofs);
			write_bin<uint64_t>(0u, ofs); // TODO

			// Literals
			for (auto v : b.values.mem) {
				write_bin(v, ofs);
			}

			// Instructions
			for (const auto &ins : b.instrs) {
				instr_bin(ins, ofs);
			}

			return true;
		}
	};
}

#endif /* SERIALIZER_H_INCLUDED */
