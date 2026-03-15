#include "lex.h"
#include <cctype>
#include <iostream>
#include <map>
#include <string>

using namespace std;

bool invalidChar = false;
bool newLineStr = false;
bool iCorrectExpT1 = false;
bool iCorrectExpT2 = false;
bool ifloatingPointC = false;
bool commentMissingClose = false;
bool commentNestErr = false;
string tokenNames[47] = {"IF", "ELSE", "WRITELN", "WRITE", "READLN", "INTEGER", "REAL",
	"BOOLEAN", "CHAR", "STRING", "BEGIN", "END", "VAR", "CONST", "THEN", "PROGRAM",
	"IDENT", "TRUE", "FALSE",
	"ICONST", "RCONST", "SCONST", "BCONST", "CCONST",
	"PLUS", "MINUS", "MULT", "DIV", "IDIV", "MOD", "ASSOP", "EQ", 
	"GTHAN", "LTHAN", "AND", "OR", "NOT", 
	"COMMA", "SEMICOL", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "DOT", "COLON",
	"ERR",
	"DONE"
};
ostream& operator<<(ostream& out, const LexItem& tok)
{

    if(invalidChar)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Invalid character for starting a token \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(newLineStr)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") New Line is not allowed within string literal \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(ifloatingPointC)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Invalid floating-point constant \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(iCorrectExpT1)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Invalid exponent for a floating-point constant \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(iCorrectExpT2)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Invalid exponent for floating-point constant \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(commentMissingClose)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Missing closing symbol(s) for a comment \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(commentNestErr)
    {
        out << "ERR: Error in line (" << tok.GetLinenum() +1<< ") Invalid nesting of comments \"" << tok.GetLexeme() << "\"" << endl;
    }
    else if(tok.GetToken() == SCONST)
    {
        out << tokenNames[tok.GetToken()] << ": " << tok.GetLexeme() << endl;
    }
    else
    {
        if(tok.GetToken() == IDENT)
        {
            string progChecker = tok.GetLexeme().substr(0, tok.GetLexeme().size() - 1);
            out << tokenNames[tok.GetToken()] << ": <" << tok.GetLexeme() << ">" << endl;
        }
        else
        {
            out << tokenNames[tok.GetToken()] << ": \"" << tok.GetLexeme() << "\"" << endl;
        }
    }
    invalidChar = false;
    newLineStr = false;
    iCorrectExpT1 = false;
    iCorrectExpT2 = false;
    ifloatingPointC = false;
    commentMissingClose = false;
    commentNestErr = false;

    return out;
}

LexItem id_or_kw(const string& lexeme, int linenum)
{
    if(lexeme == "if")
    {
        return LexItem(IF, lexeme, linenum);
    }
    else if(lexeme == "else")
    {
        return LexItem(ELSE, lexeme, linenum);
    }
    else if(lexeme == "writeln")
    {
        return LexItem(WRITELN, lexeme, linenum);
    }
    else if(lexeme == "write")
    {
        return LexItem(WRITE, lexeme, linenum);
    }
    else if(lexeme == "readln")
    {
        return LexItem(READLN, lexeme, linenum);
    }
    else if(lexeme == "integer")
    {
        return LexItem(INTEGER, lexeme, linenum);
    }
    else if(lexeme == "real")
    {
        return LexItem(REAL, lexeme, linenum);
    }
    else if(lexeme == "boolean")
    {
        return LexItem(BOOLEAN, lexeme, linenum);
    }
    else if(lexeme == "char")
    {
        return LexItem(CHAR, lexeme, linenum);
    }
    else if(lexeme == "string")
    {
        return LexItem(STRING, lexeme, linenum);
    }
    else if(lexeme == "begin")
    {
        return LexItem(BEGIN, lexeme, linenum);
    }
    else if(lexeme == "end" || lexeme == "END")
    {
        return LexItem(END, "end", linenum);
    }
    else if(lexeme == "var")
    {
        return LexItem(VAR, lexeme, linenum);
    }
    else if(lexeme == "const")
    {
        return LexItem(CONST, lexeme, linenum);
    }
    else if(lexeme == "then")
    {
        return LexItem(THEN, lexeme, linenum);
    }
    else if(lexeme == "program")
    {
        return LexItem(PROGRAM, lexeme, linenum);
    }
    else if(lexeme == "div")
    {
        return LexItem(IDIV, lexeme, linenum);
    }
    else if(lexeme == "mod")
    {
        return LexItem(MOD, lexeme, linenum);
    }
    else if(lexeme == "and")
    {
        return LexItem(AND, lexeme, linenum);
    }
    else if(lexeme == "or")
    {
        return LexItem(OR, lexeme, linenum);
    }
    else if(lexeme == "not")
    {
        return LexItem(NOT, lexeme, linenum);
    }
    else
    {
        return LexItem(IDENT, lexeme, linenum);
    }
}

LexItem getNextToken(istream& in, int& linenum)
{
    char c;
    string builder;
    LexItem tempLex;
    bool leftQuoteFound = false;
    bool rightQuoteFound = false;
    bool leftParenFoundT1 = false;
    bool leftParenFoundT2 = false;
    string symbols = "*/:=<>+-.;,)";

    while(in.get(c))
    {
        leftQuoteFound = false;
        rightQuoteFound = false;
        leftParenFoundT1 = false;
        leftParenFoundT2 = false;

        bool isSymbol = (symbols.find(c) != string::npos);

        if(c == '.' && builder.size() > 0 && isdigit(builder[0]))
        {
            isSymbol = false;
        }

        if((c == '+' || c == '-') && builder.size() > 0 && isdigit(builder[0]) && (builder.find('e') != string::npos || builder.find('E') != string::npos))
        {
            isSymbol = false;
        }

        if (!isSymbol && c != ' ' && c != '\t' && c != '\r' && c != '\n' && c != '\'' && c != '{' && c != '}' && c != '(' && c != ')') 
        {
            builder += c;
        }

        // if space or symbol check for number first
        // then if neither continue to next word.
        if(c == ' ' || c == '\t' || c == '\r' || isSymbol || c == '\n')
        {

            /* --------------------------------------------------------
            * START OF NUMBER FINDER
            */
            string holder = "";
            bool periodFound = false;
            bool eFound = false;
            bool plusMinusFound = false;
            if(builder.size() > 0 && isdigit(builder[0]))
            {
                for(int i = 0; i < builder.size(); i++)
                {
                    if(isdigit(builder[i]))
                    {
                        holder += builder[i];
                    }
                    else if(builder[i] == '.' && periodFound == false)
                    {
                        holder += builder[i];
                        periodFound = true;
                    }
                    else if((builder[i] == 'e' || builder[i] == 'E') && eFound == false)
                    {
                        if(periodFound)
                        {
                            holder += builder[i];
                            eFound = true;
                        }
                        else
                        {
                            in.putback(c);

                            for(int j = builder.size() - 1; j >= i; j--)
                            {
                                in.putback(builder[j]);
                            }

                            tempLex = LexItem(ICONST, holder, linenum);
                            return tempLex;
                        }
                    }
                    else if ((builder[i] == '+' || builder[i] == '-') && (plusMinusFound == false) && (eFound == true))
                    {
                        holder += builder[i];
                        plusMinusFound = true;
                    }
                    else
                    {
                        if(eFound == true || plusMinusFound == true)
                        {
                            if(plusMinusFound)
                            {
                                iCorrectExpT2 = true;
                                holder += builder[i];
                                tempLex = LexItem(ERR, holder, linenum);
                                return tempLex;
                            }
                            iCorrectExpT1 = true;
                            holder += builder[i];
                            tempLex = LexItem(ERR, holder, linenum);
                            return tempLex;
                        }
                        else if(periodFound == true)
                        {
                            ifloatingPointC = true;
                            holder += builder[i];
                            holder += builder[i+1];
                            tempLex = LexItem(ERR, holder, linenum);
                            return tempLex;
                        }
                        periodFound = false;
                        eFound = false;
                        plusMinusFound = false;
                    }
                }
                in.putback(c);
                if(holder[holder.length() - 1] == 'e' || holder[holder.length() - 1] == 'E' || holder[holder.length() - 1] == '+' || holder[holder.length() - 1] == '-' || holder[holder.length() - 1] == '.')
                {
                    iCorrectExpT1 = true;
                    tempLex = LexItem(ERR, holder, linenum);
                    return tempLex;
                }
                else if(holder.find('.') != string::npos || (holder.find('e') != string::npos || holder.find('E') != string::npos))
                {
                    tempLex = LexItem(RCONST, holder, linenum);
                    return tempLex;
                }
                else
                {
                    tempLex = LexItem(ICONST, holder, linenum);
                    return tempLex;
                }
            }
            // Check if next char is symbol or empty space because if it is and current word is not symbol or a space
            // then current word might be ident.
            else if(builder.size() > 0)
            { 
                if(builder.size() > 0 && !isalpha(builder[0]))
                {
                    if(builder[0] == '"')
                    {
                        invalidChar = true;
                        tempLex = LexItem(ERR, "\"", linenum);
                        return tempLex;
                    }
                    invalidChar = true;
                    tempLex = LexItem(ERR, builder, linenum);
                    return tempLex;
                }
                else if(builder.size() > 0 && isalpha(builder[0]))
                {
                    tempLex = id_or_kw(builder, linenum);
                    in.putback(c);
                    return tempLex;
                }
            }
            if(c == ' ' || c == '\t' || c == '\r')
            {
                continue;
            }
            /* ------------------------------------------------------
             * END OF NUMBER FINDER
             */
        }

        // if new line add to new line
        if(c == '\n')
        {
            linenum++;
            continue;
        }
        
        // Check EOF
        int eofCheck = in.peek();
        if(eofCheck == EOF)
        {
            if(builder.size() > 0)
            {
                return id_or_kw(builder, linenum);
            }
            tempLex = LexItem(DONE, "", linenum);
            return tempLex;
        }

        if(builder == "true")
        {
            tempLex = LexItem(TRUE, builder, linenum);
            return tempLex;
        }
        else if(builder == "false")
        {
            tempLex = LexItem(FALSE, builder, linenum);
            return tempLex;
        }

        // Check for conditions like 6.
        int spaceCheck = in.peek();
        if(c == '.' && (spaceCheck == ' ' || spaceCheck == '\n' || spaceCheck == EOF) && isdigit(builder[0]))
        {
            in.putback(c);
            string removeDot = builder.substr(0, builder.size()-1);
            tempLex = LexItem(ICONST, removeDot, linenum);
            return tempLex;
        }
        else if(isSymbol)
        {
            if(c == '.')
            {
                tempLex = LexItem(DOT, ".", linenum);
                return tempLex;
            }
            else if(c == '+')
            {
                tempLex = LexItem(PLUS, "+", linenum);
                return tempLex;
            }
            else if(c == '-')
            {
                tempLex = LexItem(MINUS, "-", linenum);
                return tempLex;
            }
            else if(c == '*')
            {
                tempLex = LexItem(MULT, "*", linenum);
                return tempLex;
            }
            else if(c == '/')
            {
                tempLex = LexItem(DIV, "/", linenum);
                return tempLex;
            }
            else if(c == ':' && spaceCheck == '=')
            {
                in.get(c);
                tempLex = LexItem(ASSOP, ":=", linenum);
                return tempLex;
            }
            else if(c == ':')
            {
                tempLex = LexItem(COLON, ":", linenum);
                return tempLex;
            }
            else if(c == '=')
            {
                tempLex = LexItem(EQ, "=", linenum);
                return tempLex;
            }
            else if(c == '<')
            {
                tempLex = LexItem(LTHAN, "<", linenum);
                return tempLex;
            }
            else if(c == '>')
            {
                tempLex = LexItem(GTHAN, ">", linenum);
                return tempLex;
            }
            else if(c == ';')
            {
                tempLex = LexItem(SEMICOL, ";", linenum);
                return tempLex;
            }
            else if(c == ',')
            {
                tempLex = LexItem(COMMA, ",", linenum);
                return tempLex;
            }
            else if(c == ')')
            {
                tempLex = LexItem(RPAREN, ")", linenum);
                return tempLex;
            }
            else if(builder == "e" || builder == "E")
            {
                tempLex = id_or_kw(builder, linenum);
                return tempLex;
            }
        }

        //checks if kw and if it is it returns that
        tempLex = id_or_kw(builder, linenum);
        if(tempLex.GetToken() != 16)
        {
            if(tempLex.GetToken() == WRITE)
            {
                if(spaceCheck == 'l')
                {
                    builder += 'l';
                    in.get(c);
                    spaceCheck = in.peek();
                    if(spaceCheck == 'n')
                    {
                        builder += 'n';
                        tempLex = id_or_kw(builder, linenum);
                    }
                }
            }
            return tempLex;
        }

        /*--------------------------------------------------------------
        * START OF COMMENT AND SCONST CHECKER
        */
        // checks if either ' or { or (* comments
        if(c == '\'' || c == '{' || c == '(')
        {
            if(c == '\'')
            {
                leftQuoteFound = true;
            }
            string commentBuilder = "";
            commentBuilder += c;
            string stringTrack = "";
            stringTrack += c;

            char findMult = in.peek();
            
            if(c == '(' && findMult != '*')
            {
                tempLex = LexItem(LPAREN, "(", linenum);
                return tempLex;
            }
            else if(c == '{')
            {
                leftParenFoundT2 = true;
            }
            else if(c == '(' && findMult == '*')
            {
                leftParenFoundT1 = true;
            }


            while(in.get(c))
            {
                stringTrack += c;

                // checks if eof and if it is then you didn't
                // find second paren
                eofCheck = in.peek();
                if(eofCheck == EOF)
                {
                    if(commentBuilder == "{")
                    {
                        commentMissingClose = true;
                        tempLex = LexItem(ERR, "{", linenum + 1);
                        return tempLex;
                    }
                    else
                    {
                        commentMissingClose = true;
                        tempLex = LexItem(ERR, "(*", linenum + 1);
                        return tempLex;
                    }
                }

                // if it's a new line and you have found a leftQuote
                // and you haven't found a right quote then this is an error
                if(c == '\n' && (leftQuoteFound && rightQuoteFound == false))
                {
                    string removeNewLn = stringTrack.substr(0, stringTrack.size() - 1);
                    newLineStr = true;
                    tempLex = LexItem(ERR, removeNewLn, linenum);
                    return tempLex;
                }
                else if(c == '\n')
                {
                    linenum++;
                }
                findMult = in.peek();
               
                // check if another left paren found
                if((c == '(' && findMult == '*') || c == '{')
                {
                    // if you found a left paren
                    // that means you found a nested comment
                    if(leftParenFoundT2 )
                    {
                        if(c == '(' && findMult == '*')
                        {
                            continue;
                        }
                        commentNestErr = true;
                        commentBuilder = "{{";
                        tempLex = LexItem(ERR, commentBuilder, linenum);
                        return tempLex;
                    }
                    else if(leftParenFoundT1)
                    {
                        if(c == '{')
                        {
                            continue;
                        }
                        commentNestErr = true;
                        commentBuilder = "(*(*";
                        tempLex = LexItem(ERR, commentBuilder, linenum);
                        return tempLex;
                    }
                }
                // finds right paren
                else if((leftParenFoundT1 || leftParenFoundT2) && ((c == '*' && findMult == ')') || c == '}'))
                {
                    if(findMult == ')')
                    {
                        in.get(c);
                    }
                    builder = "";
                    break;
                }
                // finds second right quote
                else if((!leftParenFoundT1 && !leftParenFoundT2) && c == '\'')
                {
                    rightQuoteFound = true;
                    break;
                }
            }

            // if a right quote is found then and left quote was already found
            // then it's an SCONST 
            if(leftQuoteFound && rightQuoteFound)
            {
                tempLex = LexItem(SCONST, stringTrack, linenum);
                return tempLex;
            }
        }

        /* --------------------------------------------------------------
         * END OF COMMENT AND SCONST CHECKER
         */
    }
    tempLex = LexItem(DONE, "", linenum);
    return tempLex;
}