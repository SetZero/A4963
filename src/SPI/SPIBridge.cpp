#include "SPIBridge.h"

namespace spi {
    inline namespace literals {
        spi::SPIData operator ""_spi(unsigned long long element) {
            return spi::SPIData(static_cast<int>(element));
        }
    }
}