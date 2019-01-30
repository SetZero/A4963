# Unofficial A4963 Flashing Software

This is an unofficial free A4963 flashing Software for Linux. 

Used Libaries: 
  - [nlohmann JSON](https://github.com/nlohmann/json)
  - [MCP2210 Linux Libary](https://www.microchip.com/wwwproducts/en/MCP2210)
  - [cxxopts](https://github.com/jarro2783/cxxopts/blob/master/include/cxxopts.hpp)
  - [spdlog](https://github.com/gabime/spdlog)

### Build
```sh
$ cd A4963
$ cmake .
$ make .
```

### Commands

| Plugin | README |
| ------ | ------ |
| -h, --help | Shows all command line arguments |
| -d, --debug | Enables debug output |
| -j, --json [file] | Selects the json file to read from |
| -g, --generate | Generates a default json file on startup, this will take -j location, if existing |
| -f, --force | This will overwrite any exiting settings file. Only possible if -g is specified |
| -c, --client | This will enable interactive client mode |
| -i, --interface [interface] | Select the interface to flash from. Possible values: "mcp" and "atmega" |
| -s, --settings-check | Only check if the settings file syntax is correct. This will **not** flash any data to the chip, even if an interface is  specified! |

#### Examples:
```sh
# Read from config.json and flash with mcp2210
$ ./hid --json config.json --interface mcp 
# same as above
$ ./hid -j config.json -i mcp
# additionally force generate config.json and show debug output
$ ./hid -gfd -j config.json -i mcp
```

### Development

- This Software is written in C++17
- Currently only tested on gcc
- A Windows build is available in the "Windows" branch. This branch is **not up to date**! If you want to use this software on Windows you can either open a ticket and I'll port it to the newest version if there is enough demand or help me port it by forking the repository and doing a pull-request.  
