#ifndef HILL__LLVM__TRANSLATION_UNIT_HH_INCLUDED
#define HILL__LLVM__TRANSLATION_UNIT_HH_INCLUDED

#include "builtin_type.hh"

#include <string>

namespace hill::llvm {

	/*
	define dso_local i32 @main() #0 {
	  %1 = alloca i32, align 4
	  store i32 0, ptr %1, align 4
	  call void @test()
	  ret i32 10
	}*/
	struct function {
		builtin_type return_type;

	};

	/* Might be called a "module" in llvm land? */
	struct translation_unit {
		/* Header */
		std::string source_filename;

		/*
		source_filename = "llvm_example.c"
		target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
		target triple = "x86_64-pc-windows-msvc19.44.35215"*/
	};
}

#endif /* HILL__LLVM__TRANSLATION_UNIT_HH_INCLUDED */
