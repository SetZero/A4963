#include <iostream>
#include "src/SPI/mcp2210_hal.h"
#include "Percentage.h"


bool reconnect( std::unique_ptr<MCP2210>& ptr);
int userInput();
void clearInput();

int main(int argc, char **argv) {

    for(int i = 1; i < argc ; i++){
        std::cout << "Arg number " << std::to_string(i) << ": " <<argv[i] << std::endl;
    }

    using namespace CustomDataTypes::literals;
    std::cout << "this is sparta!" << std::endl;
    std::unique_ptr<MCP2210> ptr;
    ptr = std::make_unique<MCP2210>();
    spi::Data* ttest = new spi::SPIData<4, spi::big_endian>(static_cast<uint16_t >(43690),static_cast<uint16_t >(43690));
    std::cout << std::to_string((*ttest)[0]) << std::endl;
    //std::cout << " 1: " << std::to_string(ttest[0]) << " 2 " << std::to_string(ttest[1]) << " 3 " << std::to_string(ttest[2]) << " 4 " << std::to_string(ttest[3]) << std::endl;
    auto eingaben = {"spi test", "eingabe test", "exit"};
        while(true) {

            if (*ptr) {
                std::cout << "mögliche eingaben: " << std::endl;
                size_t i = 0;
                for (auto val : eingaben) {
                    std::cout << i << ".: " << val << std::endl;
                    i++;
                }
                switch (userInput()) {
                    case 0: {
                        for (int j = 0; j < 10; j++) {
                            using namespace spi::literals;
                            try {
                                if (*ptr)
                                    ptr->transfer({42_spi8,21_spi8});
                                else {
                                    std::cout << " no connection ";
                                    reconnect(ptr);
                                }
                            } catch (MCPIOException &e) {
                                std::cout << e.what();
                                if (!reconnect(ptr)) break;
                            }
                        }
                        break;
                    }
                    case 1: {
                        std::cout << "give your data (no whitespace): ";
                        std::string input{};
                        std::cin >> input;
                        clearInput();
                        for (auto ch: input) {
                            try {
                                if (*ptr)
                                    ptr->transfer(spi::SPIData(ch));
                                else {
                                    std::cout << " no connection ";
                                    reconnect(ptr);
                                }
                            } catch (MCPIOException &e) {
                                std::cout << e.what();
                                reconnect(ptr);
                            }
                        }
                        break;
                    }
                    case 2 :
                        return 0;
                    default:
                        break;
                }
            } else {
                std::cout << "device currently not connected: type 0 to attempt connections or anything else to exit the application" << std::endl;
                if(userInput()) exit(1);
                else reconnect(ptr);
            }
        }
    }


bool reconnect( std::unique_ptr<MCP2210>& ptr){
    do {
        std::cout << " type sth to attempt a reconnect ('break' for stop trying) " << std::endl;
        std::string z;
        std::cin >> z;
        if(z == "break") break;
        ptr->connect();
    } while(!(*ptr));
    return *ptr;
}

int userInput(){
    int z = -1;
    do {
        try {
            std::string str = std::to_string(0);
            std::cin >> str;
            z = std::stoi(str);
        } catch(std::exception& e){
            std::cerr << e.what();
            std::cout << " please use a valid number! " << std::endl;
        }
    } while(z == -1);
    clearInput();
    return z;
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

