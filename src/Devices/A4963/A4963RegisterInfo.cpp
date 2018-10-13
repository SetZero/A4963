//
// Created by sebastian on 03.10.18.
//
#include "A4963RegisterInfo.h"
const std::function<uint16_t(auto)> NS_A4963::RegisterValues < NS_A4963::A4963RegisterNames::BlankTime >::normalizer = [](auto input, auto precision) -> uint16_t { return static_cast<type::value_type>(input / precision); };

template<typename T>
const T NS_A4963::RegisterValues < NS_A4963::A4963RegisterNames::DeadTime >::normalizer = [](auto input, auto precision) { return static_cast<type::value_type>(input / precision); };

template<typename T>
const T NS_A4963::RegisterValues < NS_A4963::A4963RegisterNames::CurrentSenseThresholdVoltage >::normalizer = [](auto input, auto precision) { return static_cast<type::value_type>(input / precision) - 1; };

template<typename T>
const T NS_A4963::RegisterValues < NS_A4963::A4963RegisterNames::VDSThreshold >::normalizer = [](auto input, auto precision) { return static_cast<type::value_type>(input / precision); };