#ifndef HILL__INSTR_HPP_INCLUDED
#define HILL__INSTR_HPP_INCLUDED

#include "type.hpp"
#include "val_ref.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class op_code {
		END, // End of program
		ID, // Unbound identifier
		LOAD, // Load value from stack
		LOADL, // Load literal value (literal or calculated by analyzer/optimizer)
		LOADI, // Load immediate value (literal or calculated by analyzer/optimizer)
		COPY, // Copy from the stack, to the stack || Bind identifier to memory
		ADD, // Arithmetic addition
		SUB, // Arithmetic subtraction
		MUL, // Arithmetic multiplication
		TUPLE, // Build tuple
	};

	inline const char *op_code_str(op_code op)
	{
		switch (op) {
		case op_code::END: return "END";
		case op_code::ID: return "ID";
		case op_code::LOAD: return "LOAD";
		case op_code::LOADL: return "LOADL";
		case op_code::LOADI: return "LOADI";
		case op_code::COPY: return "COPY";
		case op_code::ADD: return "ADD";
		case op_code::SUB: return "SUB";
		case op_code::MUL: return "MUL";
		case op_code::TUPLE: return "TUPLE";
		default: return "<UNKNOWN>";
		}
	}

	struct instr {
		op_code op;
		type_spec res_ts;
		union {
			size_t ix;

			// Immediate values
			uint8_t imm_u8;
			uint16_t imm_u16;
			uint32_t imm_u32;
			uint64_t imm_u64;

			int8_t imm_i8;
			int16_t imm_i16;
			int32_t imm_i32;
			int64_t imm_i64;

			float imm_f32;
			double imm_f64;
		} val;
		type_spec arg1_ts;
		type_spec arg2_ts;

		std::string to_str() const
		{
			std::stringstream ss;

			ss << op_code_str(op);

			ss << " res_ts:" << res_ts.to_str();
			switch (op) {
			case op_code::END:
			case op_code::ID:
			case op_code::TUPLE:
			case op_code::LOAD:
			case op_code::LOADL:
				ss << " ix:" << this->val.ix;
				break;
			case op_code::LOADI:
				ss << " imm:";
				switch (this->res_ts.types[0]) {
				case basic_type::I8: ss << this->val.imm_i8; break;
				case basic_type::I16: ss << this->val.imm_i16; break;
				case basic_type::I32: ss << this->val.imm_i32; break;
				case basic_type::I64:
				case basic_type::I: ss << this->val.imm_i64; break;
				case basic_type::U8: ss << this->val.imm_u8; break;
				case basic_type::U16: ss << this->val.imm_u16; break;
				case basic_type::U32: ss << this->val.imm_u32; break;
				case basic_type::U64:
				case basic_type::U: ss << this->val.imm_u64; break;
				case basic_type::F32: ss << this->val.imm_f32; break;
				case basic_type::F64:
				case basic_type::F: ss << this->val.imm_f64; break;
				default: ss << "ERR"; break;
				}
				break;
			case op_code::COPY:
				ss << " ix:" << this->val.ix;
				ss << " arg2_ts:" << this->arg2_ts.to_str();
				break;
			case op_code::ADD:
			case op_code::SUB:
			case op_code::MUL:
				ss << " arg1_ts:" << this->arg1_ts.to_str();
				ss << " arg2_ts:" << this->arg2_ts.to_str();
				break;
			default:
				throw internal_exception();
			}

			return ss.str();
		}
	};
}

#endif /* HILL__INSTR_HPP_INCLUDED */
