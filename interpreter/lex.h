#pragma once

#include <iostream>
#include <cstring>
#include <string>

#define REAL_OFFSET 13
#define STRING_OFFSET 26
#define UNARY 4

enum type_of_lex {
    LEX_NULL, /*0*/
    LEX_AND, LEX_CONTINUE, LEX_ELSE, LEX_FALSE, LEX_FOR, LEX_IF, LEX_INT, /*7*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_REAL, LEX_STRING, LEX_TRUE, LEX_WHILE, LEX_WRITE, /*16*/
    LEX_FIN, /*17*/
    LEX_SEMICOLON, LEX_COMMA, LEX_LPAREN, LEX_RPAREN, LEX_LBRACE, LEX_RBRACE, /*23*/
      LEX_ASSIGN,   LEX_EQ,   LEX_NEQ,   LEX_LSS,   LEX_GTR,   LEX_LEQ,   LEX_GEQ,   LEX_PLUS,   LEX_MINUS,   LEX_TIMES,   LEX_SLASH,   LEX_U_PLUS,   LEX_U_MINUS, /*36*/
    LEX_R_ASSIGN, LEX_R_EQ, LEX_R_NEQ, LEX_R_LSS, LEX_R_GTR, LEX_R_LEQ, LEX_R_GEQ, LEX_R_PLUS, LEX_R_MINUS, LEX_R_TIMES, LEX_R_SLASH, LEX_U_R_PLUS, LEX_U_R_MINUS, /*49*/
    LEX_S_ASSIGN, LEX_S_EQ, LEX_S_NEQ, LEX_S_LSS, LEX_S_GTR, LEX_S_LEQ, LEX_S_GEQ, LEX_S_PLUS, /*57*/
    LEX_INT_CONST, LEX_REAL_CONST, LEX_STRING_CONST, /*60*/
    LEX_ID, /*61*/
    POLIZ_LABEL, /*62*/
    POLIZ_ADDRESS, /*63*/
    POLIZ_GO, /*64*/
    POLIZ_FGO  /*65*/
};

class Lex {

    type_of_lex type;

    int value; float real_value;

public:

    Lex(type_of_lex t = LEX_NULL);

    Lex(type_of_lex t, int v);

    Lex(type_of_lex t, float v);

    type_of_lex get_type() const;

    int get_value() const;

    float get_real_value() const;

    void reset_type(type_of_lex);

    friend std::ostream& operator<<(std::ostream& out, Lex l);
};
