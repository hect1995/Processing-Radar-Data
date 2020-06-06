#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include "data.hpp"

int main(int argc, char* argv[]){
    try{
        Data fileData(argv[1],argv[2]);
    }
    catch(...){
        std::cerr << "You have to provide a valid address for the data as argument and output address of csv\n";
    }
}
