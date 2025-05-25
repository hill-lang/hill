#ifndef HILL__TOKEN_HPP_INCLUDED
#define HILL__TOKEN_HPP_INCLUDED

#include "lang_spec.hpp"
#include "exceptions.hpp"

#include <string>
#include <sstream>
#include <ranges>

namespace hill {

	struct token {
		token():
			type(tt::END),
			type_spec(&lang_spec::get().get_tt_spec(tt::END)),
			op_type_spec(nullptr),
			text(""),
			lix(-1),
			cix(-1)
		{}
		token(tt token_type, const std::string &text, int lix, int cix):
			type(token_type),
			type_spec(&lang_spec::get().get_tt_spec(token_type)),
			op_type_spec(nullptr),
			text(text),
			lix(lix),
			cix(cix)
		{}
		token(token &&other) noexcept:
			type(other.type),
			type_spec(other.type_spec),
			op_type_spec(other.op_type_spec),
			text(std::move(other.text)),
			lix(other.lix),
			cix(other.cix)
		{}

		token(token &) = delete;
		token(const token &) = delete;
		token &operator=(token &) = delete;
		token &operator=(const token &) = delete;

		token &operator=(token &&other) noexcept
		{
			if (this != &other) {
				this->type = other.type;
				this->type_spec = other.type_spec;
				this->op_type_spec = other.op_type_spec;
				this->text = std::move(other.text);
				this->lix = other.lix;
				this->cix = other.cix;
			}
			return *this;
		}

//	private:
		tt type;
		const tt_spec *type_spec;
		const ot_spec *op_type_spec;
		std::string text;
		int lix, cix;

		template<typename FN> bool op_check(FN fn) const
		{
			if (this->op_type_spec) {
				return fn(*this->op_type_spec);
			} else {
				return !std::views::filter(this->type_spec->op_specs, fn).empty();
			}
		}

	public:
		token clone() const
		{
			token t;
			t.type = this->type;
			t.type_spec = this->type_spec;
			t.op_type_spec = this->op_type_spec;
			t.text = this->text;
			t.lix = this->lix;
			t.cix = this->cix;
			return t;
		}

		tt get_type() const {return this->type;}
		const std::string &get_text() const {return this->text;}
		std::string str() const {
			return this->type_spec->name + "(" + this->text + ")";
		}
		std::string to_str(bool include_pos=true) const
		{
			std::stringstream pos_ss;
			if (include_pos) pos_ss << ((lix>=0) ? lix+1 : 0) << ':' << ((cix>=0) ? cix+1 : 0) << ':';
			if (this->op_type_spec) {
				std::string arity =
					this->op_type_spec->arity==tt_arity::NULLARY ? "Nullary"
					: this->op_type_spec->arity==tt_arity::LUNARY ? "Left unary"
					: this->op_type_spec->arity==tt_arity::RUNARY ? "Right unary"
					: this->op_type_spec->arity==tt_arity::BINARY ? "Binary"
					: "<UNKNOWN>";
				return pos_ss.str() + this->str() + ":" + arity;
			} else {
				return pos_ss.str() + this->str();
			}
		}

		bool end() const {return this->type==tt::END || this->type==tt::ERROR;}
		bool error() const {return this->type==tt::ERROR;}

		bool has_arity(tt_arity arity) const {return op_check([&arity](auto &op) {return op.arity==arity;});}

		bool ws() const {return this->type_spec->kind==tt_kind::WS;}
		bool vend() const
		{
			return this->type_spec->kind==tt_kind::RGROUP || this->type_spec->kind==tt_kind::VAL;
			/* TODO: Same as vbegin() but inverted */
		}
		bool vbegin() const
		{
			return this->type_spec->kind==tt_kind::LGROUP
				|| this->type_spec->kind==tt_kind::VAL
				/*|| (this->type_spec->kind==tt_kind::OP && this->type_spec->arity==tt_arity::LUNARY)*/;
		}
		bool op() const {return this->type_spec->kind==tt_kind::OP;}
		bool val() const {return this->type_spec->kind==tt_kind::VAL;}
		bool lgroup() const {return this->type_spec->kind==tt_kind::LGROUP;}
		bool rgroup() const {return this->type_spec->kind==tt_kind::RGROUP;}

		/* Must be ran after select_op_spec() */
		bool lassoc() const{return this->op_type_spec && this->op_type_spec->assoc==tt_assoc::LEFT;}
		bool rassoc() const {return this->op_type_spec && this->op_type_spec->assoc==tt_assoc::RIGHT;}
		bool nassoc() const {return this->op_type_spec && this->op_type_spec->assoc==tt_assoc::NONE;}
		int prec() const {return this->op_type_spec ? this->op_type_spec->prec : -1;}

		const tt_spec *get_type_spec() const {return this->type_spec;}

		tt_arity get_arity() const
		{
			return this->op_type_spec ? this->op_type_spec->arity : tt_arity::NULLARY;
		}

		void select_op_spec(tt_arity arity)
		{
			if (this->op_type_spec) {
				throw internal_exception();
			}
			for (auto &ots : this->type_spec->op_specs) {
				if (ots.arity == arity) {
					if (this->op_type_spec) {throw internal_exception();}
					this->op_type_spec = &ots;
				}
			}
		}
	};
}

#endif /* HILL__TOKEN_HPP_INCLUDED */
