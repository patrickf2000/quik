#include <string>
#include <fstream>
#include <iostream>

#include <ltac/ltac.hh>

void printLtac(LtacFile *file, std::string output) {
    std::ofstream writer(output);
    
    for (auto data : file->getData()) {
    
    }
    
    writer << std::endl;
    
    for (auto code : file->getCode()) {
    
    }
    
    writer.close();
}