#include "SPIBridge.h"

spi::SPIData operator ""_spi(unsigned long long element) {
    return spi::SPIData(static_cast<int>(element));
}