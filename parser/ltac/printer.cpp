#include <string>
#include <fstream>
#include <iostream>

#include <ltac/ltac.hh>

void printLtac(LtacFile *file, std::string output) {
    std::ofstream writer(output);
    
    for (auto data : file->getData()) {
        writer << data->printInstr() << std::endl;
    }
    
    writer << std::endl;
    
    for (auto code : file->getCode()) {
        writer << code->printInstr() << std::endl;
    }
    
    writer.close();
}

// Prints a label
std::string LtacLabel::printInstr() {
    return name + ":";
}

// Prints a function
std::string LtacFunc::printInstr() {
    if (is_extern)
        return "extern " + name;
        
    std::string content = "\n";
    if (is_global) content += "global ";

    content += "func " + name + ":\n";
    content += "  setup " + std::to_string(stack_size) + "\n";
    return content;
}

// Prints a string data element
std::string LtacString::printInstr() {
    return name + " .string \"" + val + "\"";
}