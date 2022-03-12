#pragma once

#define concat_impl(f, s) f##s 
#define concat(f, s) concat_impl(f, s)

#define defer(statements) \
struct { \
	struct Impl{ \
		~Impl(){ \
			statements; \
		} \
	}dtor; \
}concat(__defer, __COUNTER__)
