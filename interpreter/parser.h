#pragma once

#include "lex.h"
#include "scanner.h"
#include "stack.h"
#include "poliz.h"

#include <iostream>

class Parser {

    Lex curr_lex;

    type_of_lex curr_type;

    type_of_lex ident_type;

    int curr_value; float curr_real_value;

    bool l_value_flag;

    Scanner scan;

    Stack<type_of_lex> st;

    Stack<int> continue_st;

    void gl();

    void push_const();

    void P();

    void D1();

    void D();

    void B();

    void S();

    void E();

    void O();

    void A();

    void L();

    void E1();

    void T();

    void N();

    void F();

    void check_id();

    void check_id_in_read();

    void dec();

    void check_E();

    void check_O();

    void check_A();

    void check_L();

    void check_E1();

    void check_T();

    void check_N();

public:

    Poliz poliz;

    Parser(const char*);

    void analyze();
};
