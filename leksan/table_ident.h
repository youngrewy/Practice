#pragma once
#ifndef TABLE_IDENT_H
#define TABLE_IDENT_H
#include <iostream>
#include <cstring>
#include "ident.h"
//#include "lex.h"
//#include "type_of_lex.h"
using namespace std;

class table_ident
{
    Ident * p;
    int size;
    int top;
public: 
    table_ident(int max_size);
    ~table_ident();
    void reset();
    Ident& operator[] (int k);
    void print();
    int put(const char *buf);
};


#endif
