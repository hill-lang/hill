#ifndef HILL__TEST__LLVM_HH_INCLUDED
#define HILL__TEST__LLVM_HH_INCLUDED

#include "../llvm/marshal.hh"

#include "./support.hh"

#include <memory>

namespace hill::test {

	inline bool llvm(utils::junit_session &test_session)
	{
		auto suite = test_session.add_suite("Test.LLVM");
		(void)suite;

		std::ofstream ofstr("tmp/llvm_test.ll", std::ios::out | std::ios::binary);

		auto tu = std::make_shared<llvm::translation_unit>();
		tu->source_filename = "llvm_test.hill";

		llvm::marshal_ir(tu, ofstr);

		return true;
	}
}

#endif /* HILL__TEST__LLVM_HH_INCLUDED */
