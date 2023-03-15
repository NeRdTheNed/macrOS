#include <macrOS/FindVersion.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    macrOS_IFR_RUNTIME_LESS_MAC_OS_MM(10, 0) {
        return EXIT_FAILURE;
    }
    macrOS_IFR_RUNTIME_HAS_MAC_OS_MM(999, 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
