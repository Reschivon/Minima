
#include "Print.h"

std::string status;
std::string getStatus(){
    return status;
}
void debug_add(std::basic_string<char> a) {
    status += a + " ";
}
void debug_add(int a) {
    status += std::to_string(a) + " ";
}
void debug_add(uint a) {
    status += std::to_string(a) + " ";
}

std::string newlines_have_indents(const std::string &in) {
    return std::regex_replace(in, std::regex("\n"), tab_string);
}

void print_single(int arg) {
    std::cout << arg;
}

void print_single(std::basic_string<char> arg) {
    std::cout << newlines_have_indents(arg);
}

void regen_tab() {
    tab_string = "\n";
    for(int i = 0; i < indents; i++)
        tab_string += "    ";
}

void unindent() {
    indents--;
    if(indents < 0)
        indents = 0;
    regen_tab();
    std::cout << tab_string;
}

void indent() {
    indents++;
    regen_tab();
}
