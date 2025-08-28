#ifndef HILL__LLVM__MARSHAL_H_INCLUDED
#define HILL__LLVM__MARSHAL_H_INCLUDED

#include "translation_unit.hh"
#include "../exceptions.hh"

#include <fstream>
#include <memory>

namespace hill::llvm {

	inline void marshal(
		const std::shared_ptr<translation_unit> &tu,
		std::ofstream &ofs)
	{
		throw not_implemented_exception();
	}

	inline std::shared_ptr<translation_unit> unmarshal(std::ifstream &ifs)
	{
		throw not_implemented_exception();
	}
}

#endif /* HILL__LLVM__MARSHAL_H_INCLUDED */
