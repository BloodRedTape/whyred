#include <core/print.hpp>
#include <core/span.hpp>
#include <graphics/api/buffer.hpp>
#include "application.hpp"

#define concat_impl(f, s) f##s
#define concat(f, s) concat_impl(f, s)

int StraitXMain(Span<const char*> args){
    Application().Run();
    return 0;
}
