

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;



// METADATA

static Metadata M = Metadata()
    .title("Li-demo2")
    .package("lithium.demo2", "0.1")
    .icon(Icon)
    .cubeRange(1, CUBE_ALLOCATION);

static AssetSlot MainSlot = AssetSlot::allocate()
    // .bootstrap(BootAssets)
    ;

#include "demo.cpp"


void main()
{
    static Demo demo;
    for (;;) {
        demo.init();
        demo.run();
        demo.cleanup();
    }
}
