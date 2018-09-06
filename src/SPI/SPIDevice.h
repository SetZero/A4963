//
// Created by sebastian on 31.08.18.
//

#pragma once


class SPIDevice {
public:
    bool operator<(const SPIDevice &rhs) const;

    bool operator>(const SPIDevice &rhs) const;

    bool operator<=(const SPIDevice &rhs) const;

    bool operator>=(const SPIDevice &rhs) const;

private:
    bool someValue;
};
