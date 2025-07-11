#ifndef HILL__INSTR_HH_INCLUDED
#define HILL__INSTR_HH_INCLUDED

#include "type.hh"
#include "val_ref.hh"

#include <string>
#include <stdint.h>

namespace hill {

	enum class op_code {
		END, // End of program
		LOAD, // Load value from stack
		LOADL, // Load literal value (literal or calculated by analyzer/optimizer)
		LOADI, // Load immediate value (literal or calculated by analyzer/optimizer)
		COPY, // Copy from the stack, to the stack || Bind identifier to memory

		ADD, // Arithmetic addition
		SUB, // Arithmetic subtraction
		MUL, // Arithmetic multiplication
		NEG, // Arithmetic negation

		TUPLE, // Build tuple
		CALL, // Regular function call

		TUPLE_ELM, // Access tuple element

		ID, // Placeholder for identifiers
	};

	inline const char *op_code_str(op_code op)
	{
		switch (op) {
		case op_code::END: return "END";
		case op_code::LOAD: return "LOAD";
		case op_code::LOADL: return "LOADL";
		case op_code::LOADI: return "LOADI";
		case op_code::COPY: return "COPY";
		case op_code::ADD: return "ADD";
		case op_code::SUB: return "SUB";
		case op_code::MUL: return "MUL";
		case op_code::NEG: return "NEG";
		case op_code::TUPLE: return "TUPLE";
		case op_code::CALL: return "CALL";
		case op_code::TUPLE_ELM: return "TUPLE_ELM";
		case op_code::ID: return "ID";
		default: throw internal_exception();
		}
	}

	struct instr {
		op_code op;
		type res_type;
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

			void *imm_p;
		} val;
		type arg1_type;
		type arg2_type;
		std::string id = "";
		int offset = 0;

		std::string to_str() const
		{
			std::stringstream ss;

			ss << op_code_str(op);

			ss << " res_type:" << res_type.to_str();
			switch (op) {
			case op_code::END:
			case op_code::TUPLE:
			case op_code::LOAD:
			case op_code::LOADL:
			case op_code::TUPLE_ELM:
				ss << " ix:" << this->val.ix;
				break;
			case op_code::LOADI:
				ss << " imm:";
				switch (this->res_type.first()) {
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
				default: throw internal_exception();
				}
				break;
			case op_code::COPY:
				ss << " ix:" << this->val.ix;
				ss << " arg2_type:" << this->arg2_type.to_str();
				break;
			case op_code::ADD:
			case op_code::SUB:
			case op_code::MUL:
				ss << " arg1_type:" << this->arg1_type.to_str();
				ss << " arg2_type:" << this->arg2_type.to_str();
				break;
			default:
				throw internal_exception();
			}

			return ss.str();
		}
	};

	template<typename VT> instr make_instr(op_code o, type ts, VT v, int offset)
	{
		instr i;

		i.op = o;
		i.res_type = ts;
		i.val.imm_u32 = v;
		i.offset = offset;

		return i;
	}

	template<> instr make_instr<void *>(op_code o, type ts, void *v, int offset)
	{
		instr i;

		i.op = o;
		i.res_type = ts;
		i.val.imm_p = v;
		i.offset = offset;

		return i;
	}

	inline instr make_placeholder_instr(const std::string &id, int offset)
	{
		instr i;

		i.op = op_code::ID;
		i.id = id;
		i.offset = offset;

		return i;
	}
}

#endif /* HILL__INSTR_HH_INCLUDED */
