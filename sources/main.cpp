#include <core/print.hpp>
#include <core/span.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/gpu.hpp>
#include "application.hpp"
#include <core/os/directory.hpp>
#include <core/env/os.hpp>

int main(){
    GPU::ForceInit();
#ifdef SX_OS_WINDOWS
    Directory::Change("../../../");
#endif
    Application().Run();
    return 0;
}
