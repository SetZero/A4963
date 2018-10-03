//
// Created by sebastian on 03.10.18.
//
#include "A4963RegisterInfo.h"

using regName = NS_A4963::A4963RegisterNames;
template<regName name>
using reg = NS_A4963::RegisterValues<name>;

const reg<regName::BlankTime>::type reg<regName::BlankTime>::value{400ns, 6us, 0us};
const reg<regName::DeadTime>::type  reg<regName::DeadTime>::value{50ns, 3.15us, 100ns};
const reg<regName::CurrentSenseThresholdVoltage>::type reg<regName::CurrentSenseThresholdVoltage>::value{12.5_mV, 200.0_mV, 12.5_mV};