#include "parser.h"

void Parser::gl() {

    curr_lex = scan.get_lex();

    curr_type = curr_lex.get_type();

    if (curr_type == LEX_REAL) {

        curr_real_value = curr_lex.get_real_value();
    }
    else {

        curr_value = curr_lex.get_value();
    }
    //std::cout << curr_lex << std::endl;
}

void Parser::push_const(){

    if (curr_type == LEX_INT_CONST) st.push(LEX_INT);

    else if (curr_type == LEX_REAL_CONST) st.push(LEX_REAL);

    else if (curr_type == LEX_STRING_CONST) st.push(LEX_STRING);

    else throw "isn't const";
}

void Parser::P() {

    if(curr_type == LEX_PROGRAM) {

        gl();
    }
    else throw curr_lex;

    if(curr_type == LEX_LBRACE) {

        gl();

        D1();

        B();
    }
    else throw curr_lex;

    if(curr_type == LEX_RBRACE) {

        gl();
    }
    else throw curr_lex;

    if(curr_type == LEX_FIN) {}

    else throw curr_lex;
}

void Parser::D1() {

    while (curr_type == LEX_INT || curr_type == LEX_REAL || curr_type == LEX_STRING) {

        ident_type = curr_type;

        gl();

        D();

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else throw curr_lex;
    }
}

void Parser::D() {

    int curr_ident;

    if (curr_type == LEX_ID) {

        curr_ident = curr_value;

        dec();

        st.push(ident_type);

        gl();
    }
    else throw curr_lex;

    if (curr_type == LEX_ASSIGN) {

        st.push(curr_type);

        gl();

        if (curr_type == LEX_INT_CONST || curr_type == LEX_REAL_CONST || curr_type == LEX_STRING_CONST) {

            poliz.push_back(Lex(POLIZ_ADDRESS, curr_ident));

            poliz.push_back(curr_lex);

            push_const();

            check_E();

            poliz.push_back(Lex(LEX_SEMICOLON));

            gl();
        }
        else throw curr_lex;
    }
    else {

        st.pop();
    }

    while(curr_type == LEX_COMMA) {

        gl();

        if (curr_type == LEX_ID) {

            curr_ident = curr_value;

            dec();

            st.push(ident_type);

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_ASSIGN) {

            st.push(curr_type);

            gl();

            if (curr_type == LEX_INT_CONST || curr_type == LEX_REAL_CONST || curr_type == LEX_STRING_CONST) {

                poliz.push_back(Lex(POLIZ_ADDRESS, curr_ident));

                poliz.push_back(curr_lex);

                push_const();

                check_E();

                poliz.push_back(Lex(LEX_SEMICOLON));

                gl();
            }
            else throw curr_lex;
        }
        else {

            st.pop();
        }
    }
}

void Parser::B() {

    while (curr_type != LEX_RBRACE && curr_type != LEX_FIN) {

        S();
    }
}

void Parser::S() {

    if (curr_type == LEX_IF) {

        int else_lp, from_lp;

        gl();

        if (curr_type == LEX_LPAREN) {

            gl();

            E();

            st.pop();

            else_lp = poliz.size();

            poliz.push_back(Lex());

            poliz.push_back(Lex(POLIZ_FGO));
        }
        else throw curr_lex;

        if (curr_type == LEX_RPAREN) {

            gl();

            S();

            from_lp = poliz.size();

            poliz.push_back(Lex());

            poliz.push_back(Lex(POLIZ_GO));
        }
        else throw curr_lex;

        if (curr_type == LEX_ELSE) {

            gl();

            poliz[else_lp] = Lex(POLIZ_LABEL, poliz.size());

            S();

            poliz[from_lp] = Lex(POLIZ_LABEL, poliz.size());
        }
        else throw curr_lex;
    }
    else if (curr_type == LEX_WHILE) {

        int continue_l, break_lp;

        gl();

        if (curr_type == LEX_LPAREN) {

            gl();

            continue_l = poliz.size();

            continue_st.push(continue_l);

            E();

            st.pop();

            break_lp = poliz.size();

            poliz.push_back(Lex());

            poliz.push_back(Lex(POLIZ_FGO));
        }
        else throw curr_lex;

        if (curr_type == LEX_RPAREN) {

            gl();

            S();

            poliz.push_back(Lex(POLIZ_LABEL, continue_l));

            poliz.push_back(Lex(POLIZ_GO));

            poliz[break_lp] = Lex(POLIZ_LABEL, poliz.size());
        }
        else throw curr_lex;

        continue_st.pop();

    }
    else if (curr_type == LEX_FOR) {

        int continue_l, condition_l, iter_lp, break_lp;

        gl();

        if (curr_type == LEX_LPAREN) {

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else {

            E();

            st.pop();

            poliz.push_back(Lex(LEX_SEMICOLON));

            if (curr_type == LEX_SEMICOLON) {

                gl();
            }
            else throw curr_lex;
        }
        condition_l = poliz.size();

        if (curr_type == LEX_SEMICOLON) {

            poliz.push_back(Lex(LEX_INT_CONST, 1));

            gl();
        }
        else {

            E();

            st.pop();

            if (curr_type == LEX_SEMICOLON) {

                gl();
            }
            else throw curr_lex;
        }
        break_lp = poliz.size();

        poliz.push_back(Lex());

        poliz.push_back(Lex(POLIZ_FGO));

        iter_lp = poliz.size();

        poliz.push_back(Lex());

        poliz.push_back(Lex(POLIZ_GO));

        continue_l = poliz.size();

        continue_st.push(continue_l);

        if (curr_type == LEX_RPAREN) {

            gl();
        }
        else {

            E();

            st.pop();

            poliz.push_back(Lex(LEX_SEMICOLON));

            if (curr_type == LEX_RPAREN) {

                gl();
            }
            else throw curr_lex;
        }
        poliz.push_back(Lex(POLIZ_LABEL, condition_l));

        poliz.push_back(Lex(POLIZ_GO));

        poliz[iter_lp] = Lex(POLIZ_LABEL, poliz.size());

        S();

        poliz.push_back(Lex(POLIZ_LABEL, continue_l));

        poliz.push_back(Lex(POLIZ_GO));

        poliz[break_lp] = Lex(POLIZ_LABEL, poliz.size());

        continue_st.pop();
    }
    else if (curr_type == LEX_CONTINUE) {

        if(continue_st.is_empty()) throw "unexpected continue";

        int continue_l = continue_st.pop();

        continue_st.push(continue_l);

        poliz.push_back(Lex(POLIZ_LABEL, continue_l));

        poliz.push_back(Lex(POLIZ_GO));

        gl();

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else throw curr_lex;
    }
    else if (curr_type == LEX_READ) {

        gl();

        if (curr_type == LEX_LPAREN) {

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_ID) {

            check_id();

            poliz.push_back(Lex(POLIZ_ADDRESS, curr_value));

            poliz.push_back(Lex(LEX_READ));

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_RPAREN) {

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else throw curr_lex;
    }
    else if (curr_type == LEX_WRITE) {

        gl();

        if (curr_type == LEX_LPAREN) {

            gl();
        }
        else throw curr_lex;

        E();

        type_of_lex write_type = st.pop();

        if (write_type == LEX_INT) {

            poliz.push_back(Lex(LEX_WRITE));
        }
        else if (write_type == LEX_REAL) {

            poliz.push_back(Lex(LEX_R_WRITE));
        }
        else {

            poliz.push_back(Lex(LEX_S_WRITE));
        }
        while (curr_type == LEX_COMMA) {

            gl();

            E();

            st.pop();

            poliz.push_back(Lex(LEX_WRITE));
        }
        if (curr_type == LEX_RPAREN) {

            gl();
        }
        else throw curr_lex;

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else throw curr_lex;
    }
    else if (curr_type == LEX_LBRACE) {

        gl();

        B();

        if (curr_type == LEX_RBRACE) {

            gl();
        }
        else throw curr_lex;
    }
    else {

        E();

        st.pop();

        poliz.push_back(Lex(LEX_SEMICOLON));

        if (curr_type == LEX_SEMICOLON) {

            gl();
        }
        else throw curr_lex;
    }
}

void Parser::E() {

    l_value_flag = true;

    O();

    if (curr_type == LEX_ASSIGN) {

        if(!l_value_flag) throw "Not l_value expression";

        else poliz[poliz.size() - 1].reset_type(POLIZ_ADDRESS);

        st.push(curr_type);

        gl();

        E();

        check_E();
    }
}

void Parser::O() {

    A();

    while (curr_type == LEX_OR) {

        l_value_flag = false;

        st.push(curr_type);

        gl();

        A();

        check_O();
    }
}

void Parser::A() {

    L();

    while (curr_type == LEX_AND) {

        l_value_flag = false;

        st.push(curr_type);

        gl();

        L();

        check_A();
    }
}

void Parser::L() {

    E1();

    while (curr_type == LEX_EQ || curr_type == LEX_NEQ || curr_type == LEX_GTR || curr_type == LEX_GEQ || curr_type == LEX_LSS || curr_type == LEX_LEQ) {

        l_value_flag = false;

        st.push(curr_type);

        gl();

        E1();

        check_L();
    }
}

void Parser::E1() {

    T();

    while (curr_type == LEX_PLUS || curr_type == LEX_MINUS) {

        l_value_flag = false;

        st.push(curr_type);

        gl();

        T();

        check_E1();
    }
}

void Parser::T() {

    N();

    while (curr_type == LEX_TIMES || curr_type == LEX_SLASH) {

        l_value_flag = false;

        st.push(curr_type);

        gl();

        N();

        check_T();
    }
}

void Parser::N() {

    if (curr_type == LEX_NOT || curr_type == LEX_PLUS || curr_type == LEX_MINUS) { // NOT, unary plus and unary minus

        l_value_flag = false;

        st.push(curr_type);

        gl();

        N();

        check_N();
    }
    else {

        F();
    }
}

void Parser::F() {

    if (curr_type == LEX_LPAREN) {

        l_value_flag = false;

        gl();

        E();

        if (curr_type == LEX_RPAREN) {

            gl();
        }
        else throw curr_lex;
    }
    else if (curr_type == LEX_INT_CONST || curr_type == LEX_REAL_CONST || curr_type == LEX_STRING_CONST) {

        l_value_flag = false;

        push_const();

        poliz.push_back(curr_lex);

        gl();
    }
    else if(curr_type == LEX_ID) {

        check_id();

        st.push(Scanner::TID[curr_value].get_type());

        poliz.push_back(Lex(LEX_ID, curr_value));

        gl();
    }
    else throw curr_lex;
}

void Parser::check_id() {

    if(!Scanner::TID[curr_value].get_declare()) throw "ID is not declared";
}

void Parser::dec() {

    if(Scanner::TID[curr_value].get_declare()) throw "ID has been already declared!";

    else {

        Scanner::TID[curr_value].put_declare();

        Scanner::TID[curr_value].set_type(ident_type);
    }
}

void Parser::check_E() {

    type_of_lex r_value = st.pop(), operation = st.pop(), l_value = st.pop();

    if (l_value == LEX_INT) {

        if(r_value == LEX_STRING) throw "Different types in = !";

        st.push(l_value);

        poliz.push_back(Lex(operation));
    }
    else if (l_value == LEX_REAL) {

        if(r_value == LEX_STRING) throw "Different types in = !";

        st.push(l_value);

        poliz.push_back(Lex((type_of_lex)(operation + REAL_OFFSET)));
    }
    else if (l_value == LEX_STRING && r_value == LEX_STRING) {

      st.push(l_value);

      poliz.push_back(Lex((type_of_lex)(operation + STRING_OFFSET)));
    }
    else throw "Different types in = !";
}

void Parser::check_O() {

    type_of_lex operand1 = st.pop(), operation = st.pop(), operand2 = st.pop();

    if (operand1 == LEX_INT && operand2 == LEX_INT) {

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else throw "Wrong types in operation";
}

void Parser::check_A() {

    type_of_lex operand1 = st.pop(), operation = st.pop(), operand2 = st.pop();

    if (operand1 == LEX_INT && operand2 == LEX_INT) {

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else throw "Wrong types in operation";
}

void Parser::check_L() {

    type_of_lex operand1 = st.pop(), operation = st.pop(), operand2 = st.pop();

    if (operand1 == LEX_INT && operand2 == LEX_INT) {

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else if (operand1 == LEX_STRING && operand2 == LEX_STRING) {

        st.push(LEX_INT);

        poliz.push_back(Lex((type_of_lex)(operation + STRING_OFFSET)));
    }
    else if (operand1 == LEX_STRING || operand2 == LEX_STRING) throw "Wrong types in operation";

    else {

        st.push(LEX_INT);

        poliz.push_back(Lex((type_of_lex)(operation + REAL_OFFSET)));
    }
}

void Parser::check_E1() {

    type_of_lex operand1 = st.pop(), operation = st.pop(), operand2 = st.pop();

    if (operand1 == LEX_INT && operand2 == LEX_INT) {

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else if (operand1 == LEX_STRING && operand2 == LEX_STRING) {

        if(operation != LEX_PLUS) throw "Wrong types in operation";

        st.push(LEX_STRING);

        poliz.push_back(Lex((type_of_lex)(operation + STRING_OFFSET)));
    }
    else if (operand1 == LEX_STRING || operand2 == LEX_STRING) throw "Wrong types in operation";

    else {

        st.push(LEX_REAL);

        poliz.push_back(Lex((type_of_lex)(operation + REAL_OFFSET)));
    }
}

void Parser::check_T() {

    type_of_lex operand1 = st.pop(), operation = st.pop(), operand2 = st.pop();

    if (operand1 == LEX_INT && operand2 == LEX_INT) {

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else if (operand1 == LEX_STRING || operand2 == LEX_STRING) throw "Wrong types in operation";

    else {

        st.push(LEX_REAL);

        poliz.push_back(Lex((type_of_lex)(operation + REAL_OFFSET)));
    }
}

void Parser::check_N() {

    type_of_lex operand = st.pop(), operation = st.pop();

    if(operand == LEX_STRING) throw "Wrong types in operation";

    else if(operation == LEX_NOT) {

        if(operand != LEX_INT) throw "Wrong types in operation";

        st.push(LEX_INT);

        poliz.push_back(Lex(operation));
    }
    else {

        if(operand == LEX_INT) {

          st.push(LEX_INT);

          poliz.push_back(Lex((type_of_lex)(operation + UNARY)));
        }

        else {

            st.push(LEX_REAL);

            poliz.push_back(Lex((type_of_lex)(operation + REAL_OFFSET + UNARY)));
        }
    }
}

Parser::Parser(const char* buf) : scan(buf) {}

void Parser::analyze() {

    gl();

    P();

    //std::cout << "SUCCESS" <<std::endl;
}
