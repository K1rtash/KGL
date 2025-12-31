#ifndef KIRTASH_H_STL
#define KIRTASH_H_STL

#include <iostream>
#include <string>

namespace kirtash {

    bool stringIsInt(std::string val)
    {
        if(val.empty()) return false; // si esta vacio no puede ser un numero

        bool negative = false;
        int start, end = (val.length() - 1);

        if(val[0] == '-') negative = true; // si empieza por - es negativo
        if(negative && val.length() <= 1) return false; // si empieza por - y mide 1 o menos es solo un -
        negative ? start = 1 : start = 0;
        for(int i = start; i < end; i++) {
            if(!std::isdigit(val[i])) return false;
        }
        return true;
    }
}
#endif