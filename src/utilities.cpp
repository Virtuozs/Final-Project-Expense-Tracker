// utilities.cpp
#include "utilities.hpp"
#include <cstdlib>  
#include <iostream> 

void utilities::cleanScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
