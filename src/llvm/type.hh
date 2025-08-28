#ifndef HILL__LLVM__TYPES_H_INCLUDED
#define HILL__LLVM__TYPES_H_INCLUDED

namespace hill::llvm {
	
	enum class type {
		/*
		e - little endian
		p:64:64:64 - 64-bit pointers with 64-bit alignment.
		p[n]:64:64:64 - Other address spaces are assumed to be the same as the default address space.
		S0 - natural stack alignment is unspecified
		i1:8:8 - i1 is 8-bit (byte) aligned
		i8:8:8 - i8 is 8-bit (byte) aligned as mandated
		i16:16:16 - i16 is 16-bit aligned
		i32:32:32 - i32 is 32-bit aligned
		i64:32:64 - i64 has ABI alignment of 32-bits but preferred alignment of 64-bits
		f16:16:16 - half is 16-bit aligned
		f32:32:32 - float is 32-bit aligned
		f64:64:64 - double is 64-bit aligned
		f128:128:128 - quad is 128-bit aligned
		v64:64:64 - 64-bit vector is 64-bit aligned
		v128:128:128 - 128-bit vector is 128-bit aligned
		a:0:64 - aggregates are 64-bit aligned*/
		VOID,
		I8, I16, I32, I64,
		U8, U16, U32, U64,
		F16, F32, F64, F128,
		//V64, V128,
	};
}

#endif /* HILL__LLVM__TYPES_H_INCLUDED */
