#include "scanner.h"

const char *
Scanner::TW    [] = {"", "and", "boolean", "do", "else", "if", "false", "int", "not", "or", "program", "read", "true", "while", "write", "string", "goto", NULL};
 
const char *
Scanner::TD    [] = {";", ",", "=", "(", ")", "{", "}", "==", "<", ">", "+", "-", "*", "/", "%", "\\*", "*\\", "<=", "!=", ">=", "+=", "-=", "\"", ":", NULL};

const type_of_lex 
Scanner::words[] = {LEX_NULL, LEX_AND, LEX_BOOLEAN, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_WHILE, LEX_WRITE, LEX_STR, LEX_GOTO, LEX_NULL};

const type_of_lex
Scanner::dlms[] = {LEX_SEMICOLON, LEX_COMMA, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_LBRACE, LEX_RBRACE, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_MULT, LEX_DIV, LEX_MOD, LEX_FRONTSLASH, LEX_BACKSLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_PLUSEQ, LEX_MINUSEQ, LEX_QUOTE, LEX_COLON, LEX_NULL}; 

int words_size = 18;
char Scanner::gc () // считывание символа из файла
{
    return (c = fgetc (fp));
}

void Scanner::clear() // каждый раз чистим буфер для нового слова
{
    buf_top = 0;
    for (int i = 0; i < 100; i++)
        buf[i] = '\0';
}

void Scanner::add() // добавление буквы
{
    buf[buf_top++] = c;
}

Scanner::Scanner(const char * program) // открываем файл на чтение
{
    fp = fopen(program, "r");
    CS = H; // тут лежит служебное слово
    clear();
}

int Scanner::look(const char * buf, const char **list) // перебираем массив TW, ищем нужное слово
{
    int i = 0;
    while (list[i])
    {
        if (!strcmp(buf, list[i]))
            return i;
        i++;
    }
    return 0;
}

char * Scanner::g_label() // вернем номер в таблице TL, которому будет соответствовать метка после goto
{
    //clear();
    c = gc();
    while (c == ' ' || c == '\n' || c == '\r' || c == '\t') 
        { 
            if(c == '\n')
            {
                count_str++;
                count_col = 1;
            }
            if(c == ' ' || c == '\t')
            {
                count_col++;
            }
            c = gc();
        }
    do
    {
        //cout << "C:         " << c << endl;
        if (isalpha(c) || isdigit(c))
        {
            add();
        }
        else if (c != ';')
        {
            if(c == '\n')
            {
                count_str++;
                count_col = 1;
                throw " \\n ";
            }
            else
            {
                throw c;
            }
        }
        else
            break;
        c = gc();
    }
    while (1);
    //clear();
    ungetc(c, fp);
    return buf;
    //TL[j].put_position(j);
}

Lex Scanner::get_lex() // считывание слова
{
    int d = 0, j;
    CS = H; // служебное слово в начале
    clear();
    do
    {
        c = gc();
        if(c == EOF)
        {
            return Lex(LEX_EOF, LEX_EOF);
        }
        switch(CS)
        {
            case H:
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
                {
                    if(c == '\n')
                    {
                        //cout << "UUUUUU" << endl;
                        count_str++;
                        count_col = 1;
                    }
                    if(c == ' ' || c == '\t')
                    {
                        count_col++;
                    }
                }
                else if (isalpha(c))
                {
                    add();
                    CS = IDENT;
                }
                else if (isdigit(c))
                {
                    d = c - '0';
                    CS = NUMB;
                }
                else if (c == '<' || c == '>' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
                {
                    add();
                    CS = ALE;
                }
                else if(c == ';')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_SEMICOLON, j);
                }
                else if(c == ',')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_COMMA, j);
                }
                else if (c == '"')
                {
                    CS = QUOTE;
                }
                else if(c == ':')
                {
                    CS = IDENT;
                    ungetc(c, fp);
                }
                else if(c == '(')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    //count_col++;
                    return Lex(LEX_LPAREN, j);
                }
                else if(c == ')')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    //count_col++;
                    return Lex(LEX_RPAREN, j);
                }
                else if(c == '{')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    //count_col++;
                    return Lex(LEX_LBRACE, j);
                }
                else if(c == '}')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    //count_col++;
                    return Lex(LEX_RBRACE, j);
                }
                else if (c == '\\')
                {
                    try
                    {
                        c = gc();
                        if (c == '*')
                            cout << "Entering comment " << endl;
                        else 
                            throw c;
                        char c0 = gc();
                        c = gc();
                        //cout << c;
                        do 
                        {
                            if ((c0 == '*') && (c == '\\'))
                                break;
                            cout << c0;
                            c0 = c;
                            c = gc();
                            
                        }
                        while (c != EOF);
                        
                        if ( c == EOF)
                        {
                            throw "Comment is't cloused";
                        }
                        cout << endl;
                        //c = gc();
                        cout << "Leaving comment" << endl;
                        CS = H;
                        //c = gc();
                        break;
                    }
                    catch(char c)
                    {
                        cout << "\x1b[37;41m" << "Line: " << count_str << " Column: " << count_col << " Unexpected symbol" << c << "Wrong rule for comment" << "\x1b[0m" << endl;
                        err_scaner = true;
                    }
                    catch( const char* c)
                    {
                        cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                        err_scaner = true;
                    }

                }
                else if (c == '!')
                {
                    add();
                    CS = NEQ;
                }
                else if (c == '=')
                {
                    add();
                    CS = EQEQ;
                }
                else
                {
                    try
                    {
                        add();
                        throw c;
                    }
                    catch(char c)
                    {
                        //count_col++;
                        cout << "\x1b[37;41m" << "Line: " << count_str << " Column: " << count_col << " Unexpected symbol: " << c << "\x1b[0m" << endl;
                        err_scaner = true;
                        //count_col--;
                    }
                    catch( const char *c)
                    {
                        cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                        err_scaner = true;
                    }
                }
                break;
            case IDENT:
                if (isalpha(c) || isdigit(c))
                    add();
                else
                {
                    //cout << "I'm hear_int";
                    if ((j = look(buf, TW)) > 0)
                    {
                        ungetc(c, fp);
                        clear();
                        count_col++;
                        return Lex(words[j], j);
                    }
                    else if (c == ':')
                    {
                        //cout << buf_top << endl;
                        count_col++;
                        if(buf_top == 0)
                        {
                            throw " Expected name of COLON ";
                        }
                        return Lex(LEX_COLON, buf);
                        //TL[j].put_position(j);
                    }
                    else
                    {
                        //cout << "aaaaa"<< endl;
                        try
                        {
                            if(!isalpha(c) && !isdigit(c) && (j = look(&c, TW) < 0))
                            {
                                add();
                                throw c;
                            }
                        }
                        catch(char c)
                        {
                            cout << "\x1b[37;41m" << "Line: " << count_str << " Column: " << count_col << " Unexpected symbol: " << c << "\x1b[0m" << endl;
                            err_scaner = true;
                        }
                        catch( const char *c)
                        {
                            cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                            err_scaner = true;
                        }
                        j = TID.put(buf);
                        //cout << "UUUU: " << j << endl;
                        //TID[j].put_name(buf);
                        //TID[j].put_type(LEX_ID);
                        TID[j].put_value(j);
                        ungetc(c, fp);
                        clear();
                        count_col++;
                        return Lex(LEX_ID, j);
                    }
                }
                break;
            case NUMB:
                if (isdigit(c))
                    d = d * 10 + (c - '0');
                else
                {
                    //gc();
                    ungetc(c, fp); // нужно, чтобы не пропустить запятую
                    clear();
                    count_col++;
                    return Lex(LEX_NUM, d);
                }
                break;
            case ALE:
                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(dlms[j], j);
                }
                else 
                {
                    j = look(buf, TD);
                    ungetc(c, fp);
                    clear();
                    //cout << dlms[j];
                    count_col++;
                    return Lex(dlms[j], j);
                }
            /*case SLASH:
                if (c == '*')
                {
                    cout << "Entering comment" << endl;
                    CS = COMM;
                }
            case COMM:
            {
                cout << "I am in a comment" << endl;
                while (c != '*')
                {
                    gc();
                }
                gc();
                if (c == '\\')
                {
                    cout << "Leaving comment" << endl;
                    CS = H;
                }
                else
                    cout << "Sorry, it is not a comment" << endl;
                break;
            }*/
            case NEQ:
                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_NEQ, j);
                }
                else
                    cout << "Error with NOT EQUEAL"; // написать исключение
                break;
            case EQEQ:
            {

                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_EQ, j);
                }
                else
                {
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_ASSIGN, j);
                }
                break;
            }
            case QUOTE:
            {
                //char * buf_tmp = new char[1000];
                /*try
                {*/
                    while (c != '"' && c != ';' && c != ')')
                    {   
                        add();
                        c = gc();
                    }
					//ungetc(c, fp);
                    c = gc();
                    if(c != ' ' && c != ',' && c != ')' && !isalpha(c) && !isdigit(c))
                    {
						//cout << c << endl;
                     	ungetc(c, fp);
						//CS = H;
						throw c;
                    }
                    else
                    {
                        j = TSTR.put(buf);
                        //buf_tmp = strcpy(buf_tmp, buf);
                        ungetc(c, fp);
                        clear();
                        count_col++;
                        return Lex(LEX_STR, j);
                    }
                /*}
                catch(char c)
                {
                    cout << "\x1b[37;41m" << "Line: " << count_str << " Column: " << count_col << " Unexpected symbol: " << c << "\x1b[0m" << endl;
                    err_scaner = true;
                    j = TSTR.put(buf);
                    //buf_tmp = strcpy(buf_tmp, buf);
                    //ungetc(c, fp);
                    clear();
                    count_col++;
                    return Lex(LEX_STR, j);
                }
                catch( const char *c)
                {
                    cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                    err_scaner = true;
                    j = TSTR.put(buf);
                    //buf_tmp = strcpy(buf_tmp, buf);
                    //ungetc(c, fp);
                    clear();
                    count_col++;
                    return Lex(LEX_STR, j);
                }*/

            }
            
        }
    }
    while(1);
};
