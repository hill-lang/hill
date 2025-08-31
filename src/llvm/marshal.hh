#ifndef HILL__LLVM__MARSHAL_HH_INCLUDED
#define HILL__LLVM__MARSHAL_HH_INCLUDED

#include "translation_unit.hh"
#include "../exceptions.hh"
#include "../version.hh"

#include <string>
#include <fstream>
#include <memory>

namespace hill::llvm {

	inline void marshal_ir(
		const std::shared_ptr<translation_unit> &tu,
		std::ofstream &ofs)
	{
		ofs << "; ModuleID = '" << tu->source_filename << "'\n";
		ofs << "source_filename = \"" << tu->source_filename << "\"\n";
		ofs << "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128\"\n";
		ofs << "target triple = \"x86_64-pc-windows-msvc19.44.35215\"\n\n";

		ofs << "; Function Attrs: noinline nounwind optnone uwtable\n"
			"define dso_local i32 @main() #0 {\n"
			"  %1 = alloca i32, align 4\n"
			"  %2 = alloca i32, align 4\n"
			"  %3 = alloca i32, align 4\n"
			"  %4 = alloca i32, align 4\n"
			"  store i32 0, ptr %1, align 4\n"
			"  store i32 1, ptr %2, align 4\n"
			"  store i32 2, ptr %3, align 4\n"
			"  store i32 3, ptr %4, align 4\n"
			"  %5 = load i32, ptr %2, align 4\n"
			"  %6 = load i32, ptr %3, align 4\n"
			"  %7 = mul nsw i32 %5, %6\n"
			"  %8 = load i32, ptr %4, align 4\n"
			"  %9 = mul nsw i32 %7, %8\n"
			"  ret i32 %9\n"
			"}\n\n";

		ofs << "attributes #0 = { noinline nounwind optnone uwtable "
			"\"min-legal-vector-width\"=\"0\" "
			"\"no-trapping-math\"=\"true\" "
			"\"stack-protector-buffer-size\"=\"8\" "
			"\"target-cpu\"=\"x86-64\" "
			"\"target-features\"=\"+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87\" "
			"\"tune-cpu\"=\"generic\" }\n\n";

		ofs << "!llvm.module.flags = !{!0, !1, !2, !3}\n";
		ofs << "!llvm.ident = !{!4}\n\n";

		ofs << "!0 = !{i32 1, !\"wchar_size\", i32 2}\n";
		ofs << "!1 = !{i32 8, !\"PIC Level\", i32 2}\n";
		ofs << "!2 = !{i32 7, !\"uwtable\", i32 2}\n";
		ofs << "!3 = !{i32 1, !\"MaxTLSAlign\", i32 65536}\n";
		ofs << "!4 = !{!\"hill version "
			<< std::to_string(HILL_VERSION_MAJOR) << "."
			<< std::to_string(HILL_VERSION_MINOR) << "."
			<< std::to_string(HILL_VERSION_PATCH) << "\"}\n";
	}

	inline void marshal_bitcode(
		const std::shared_ptr<translation_unit> &tu,
		std::ofstream &ofs)
	{
		(void)tu; (void)ofs;
		throw not_implemented_exception();
	}

	/*inline std::shared_ptr<translation_unit> unmarshal_ir(std::ifstream &ifs)
	{
		throw not_implemented_exception();
	}*/
}

#endif /* HILL__LLVM__MARSHAL_HH_INCLUDED */
