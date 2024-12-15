
#include <string>
#include <iostream>
#include "out_input_stream.h"

int main() {
    CommandsAnalyzer al;
    std::string cmd;
    while(std::getline(std::cin, cmd)) {
        al.AnalyzeEvent(cmd);
    }
    return 0;
}