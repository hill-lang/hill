#ifndef HILL__SERIALIZER_H_INCLUDED
#define HILL__SERIALIZER_H_INCLUDED

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
		V0_1,
	};

	constexpr const char *file_version_str(file_version v)
	{
		switch (v) { // MAX 9 characters excluding the null terminator
		case file_version::V0_1: return "0.1";
		default: throw internal_exception();
		}
	}

	enum class serializer_mode {
		BIN,
		ASCII
	};

	struct serializer {
		serializer(serializer_mode mode): mode(mode) {}

		serializer_mode mode;

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
		enum class offset_table {
			LITERALS,
			INSTRUCTIONS,
		};

		template<typename T> void write_bin(T v, std::ofstream &ofs)
		{
			ofs.write((const char *)&v, sizeof v);
		}

		void type_spec_ascii(const type_spec &ts, std::ofstream &ofs)
		{
			size_t sz = ts.types.size();
			ofs << ' ';

			if (sz>1 || sz==0) {
				ofs << '(';
			}

			auto prev_type = basic_type::TUPLE;
			for (auto &type : ts.types) {
				if (prev_type != basic_type::TUPLE && type != basic_type::END) ofs << ',';
				ofs << basic_type_str(type);

				prev_type = type;
			}

			if (sz>1 || sz==0) {
				ofs << ')';
			}
		}

		void type_spec_bin(const type_spec &ts, std::ofstream &ofs)
		{
			write_bin(static_cast<uint16_t>(ts.types.size()), ofs);
			for (auto &t : ts.types) {
				write_bin(static_cast<uint16_t>(t), ofs);
			}
		}

		void instr_ascii(const instr &ins, std::ofstream &ofs)
		{
			ofs << op_code_str(ins.op);
			type_spec_ascii(ins.res_ts, ofs);

			switch (ins.op) {
			case op_code::END:
			case op_code::TUPLE:
				break;
			case op_code::LOAD:
			case op_code::LOADL:
				ofs << ' ' << ins.val.ix;
				break;
			case op_code::LOADI:
				ofs << ' ';
				switch (ins.res_ts.types[0]) {
				case basic_type::I8: ofs << ins.val.imm_i8; break;
				case basic_type::I16: ofs << ins.val.imm_i16; break;
				case basic_type::I32: ofs << ins.val.imm_i32; break;
				case basic_type::I64:
				case basic_type::I: ofs << ins.val.imm_i64; break;
				case basic_type::U8: ofs << ins.val.imm_u8; break;
				case basic_type::U16: ofs << ins.val.imm_u16; break;
				case basic_type::U32: ofs << ins.val.imm_u32; break;
				case basic_type::U64:
				case basic_type::U: ofs << ins.val.imm_u64; break;
				case basic_type::F32: ofs << ins.val.imm_f32; break;
				case basic_type::F64:
				case basic_type::F: ofs << ins.val.imm_f64; break;
				default: ofs << "ERR"; break;
				}
				break;
			case op_code::COPY:
				ofs << ' ' << ins.val.ix;
				type_spec_ascii(ins.arg2_ts, ofs);
				break;
			case op_code::ADD:
			case op_code::SUB:
			case op_code::MUL:
				type_spec_ascii(ins.arg1_ts, ofs);
				type_spec_ascii(ins.arg2_ts, ofs);
				break;
			default:
				throw internal_exception();
			}
			ofs << '\n';
		}

		void instr_bin(const instr &ins, std::ofstream &ofs)
		{
			write_bin(static_cast<uint16_t>(ins.op), ofs);

			type_spec_bin(ins.res_ts, ofs);

			switch (ins.op) {
			case op_code::END:
			case op_code::TUPLE:
				break;
			case op_code::LOAD:
			case op_code::LOADL:
				write_bin(static_cast<uint64_t>(ins.val.ix), ofs);
				write_bin<uint32_t>(0u, ofs);
				write_bin<uint32_t>(0u, ofs);
				break;
			case op_code::LOADI:
				switch (ins.res_ts.types[0]) {
				case basic_type::I8: write_bin(static_cast<uint64_t>(ins.val.imm_i8), ofs); break;
				case basic_type::I16: write_bin(static_cast<uint16_t>(ins.val.imm_i16), ofs); break;
				case basic_type::I32: write_bin(static_cast<uint32_t>(ins.val.imm_i32), ofs); break;
				case basic_type::I64:
				case basic_type::I: write_bin(static_cast<uint64_t>(ins.val.imm_i64), ofs); break;
				case basic_type::U8: write_bin(static_cast<uint8_t>(ins.val.imm_u8), ofs); break;
				case basic_type::U16: write_bin(static_cast<uint16_t>(ins.val.imm_u16), ofs); break;
				case basic_type::U32: write_bin(static_cast<uint32_t>(ins.val.imm_u32), ofs); break;
				case basic_type::U64:
				case basic_type::U: write_bin(static_cast<uint64_t>(ins.val.imm_u64), ofs); break;
				case basic_type::F32: write_bin(static_cast<float>(ins.val.imm_f32), ofs); break;
				case basic_type::F64:
				case basic_type::F: write_bin(static_cast<double>(ins.val.imm_f64), ofs); break;
				default: break; /* TODO: Thorw? */
				}
				break;
			case op_code::COPY:
				write_bin(static_cast<uint64_t>(ins.val.ix), ofs);
				type_spec_bin(ins.arg2_ts, ofs);
				break;
			case op_code::ADD:
			case op_code::SUB:
			case op_code::MUL:
				type_spec_bin(ins.arg1_ts, ofs);
				type_spec_bin(ins.arg2_ts, ofs);
				break;
			default:
				throw internal_exception();
			}
		}

		// File extention .hill_c.txt?
		bool ascii(
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
		bool bin(
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

#endif /* HILL__SERIALIZER_H_INCLUDED */
