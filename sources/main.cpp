#include <core/print.hpp>
#include <core/span.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/gpu.hpp>
#include "application.hpp"

int main(){
    GPU::ForceInit();

    Application().Run();
    return 0;
}
