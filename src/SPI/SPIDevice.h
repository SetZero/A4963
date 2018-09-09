//
// Created by sebastian on 31.08.18.
//

#pragma once

#include <memory>


class SPIDevice : public std::enable_shared_from_this<SPIDevice> {
public:
    bool operator<(const SPIDevice &rhs) const;

    bool operator>(const SPIDevice &rhs) const;

    bool operator<=(const SPIDevice &rhs) const;

    bool operator>=(const SPIDevice &rhs) const;

private:
    bool someValue;
};
