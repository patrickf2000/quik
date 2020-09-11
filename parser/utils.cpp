// This is a temporary part of the parser
// These functions have the same name as the scanner will, so the goal is to
// eventually drop the scanner in their place
//
// The purpose of this method is so we don't break the entire thing at once
// (or at least so I hope).

#include "parse.hh"
#include "types.hh"

TokenType QkParser::getNext() {
    if (currentIndex >= currentLn.tokens.size())
        return TokenType::NL;

    auto t = currentLn.tokens.at(currentIndex);
    ++currentIndex;
    return t.type;
}

std::string QkParser::getSVal() {
    auto t = currentLn.tokens.at(currentIndex-1);
    return t.id;
}

int QkParser::getIVal() {
    auto t = currentLn.tokens.at(currentIndex-1);
    return std::stoi(t.id);
}

double QkParser::getFloatL() {
    auto t = currentLn.tokens.at(currentIndex-1);
    return std::stod(t.id);
}

int QkParser::getLnNo() {
    return currentLn.no;
}

std::string QkParser::getCurrentLn() {
    return currentLn.original;
}
