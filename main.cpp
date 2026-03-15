#include "lex.h"
#include <iostream>
#include <cctype>
#include <string>
#include <fstream>
#include <map>

using namespace std;

int main(int argc, char* argv[])
{
    ifstream inFile;
    string fileName, placeholder = "";
    int fileNameCounter = 0, linenum = 0, tokCount = 0, idAkwCount = 0, numCount = 0, boolCount = 0, stringCount = 0;
    map<string, int> keyWordMap;
    map<string, int> identMap;
    map<string, int> strMap;
    map<string, int> integerMap;
    map<string, int> realMap;

    bool allTF = false, numTF = false, strTF = false, idsTF = false;
    
    for(int i = 1; i < argc; i++)
    {
        placeholder = argv[i];
        if(placeholder == "-all")
        {
            allTF = true;
        }
        else if(placeholder == "-num")
        {
            numTF = true;
        }
        else if(placeholder == "-str")
        {
            strTF = true;
        }
        else if(placeholder == "-ids")
        {
            idsTF = true;
        }
        else if(placeholder.substr(0,1) == "-")
        {
            cout << "UNRECOGNIZED FLAG {" << placeholder << "}" << endl;
            return 0;
        }
        else
        {
            fileNameCounter++;
            fileName = argv[i];
        }
    }

    if(fileNameCounter > 1)
    {
        cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
        return 0;
    }

    if(fileName == "")
    {
        cout << "NO SPECIFIED INPUT FILE." << endl;
        return 0;
    }

    inFile.open(fileName);

    if(!inFile)
    {
        cout << "CANNOT OPEN THE FILE " << fileName << endl;
        return 0;
    }

    while(true)
    {
        LexItem tempLex = getNextToken(inFile, linenum);

        int countLex = tempLex.GetToken();

        if(countLex == DONE)
        {
            break;
        }

        tokCount++;

        if(countLex == ERR)
        {
            cout << tempLex;
            return 0;
        }

        if(allTF)
        {
            cout << tempLex;
        }

        if(countLex == ICONST)
        {
            integerMap[tempLex.GetLexeme()]++;
            numCount++;
        }
        else if(countLex == RCONST)
        {
            realMap[tempLex.GetLexeme()]++;
            numCount++;
        }
        else if(countLex == SCONST)
        {
            strMap[tempLex.GetLexeme()]++;
            stringCount++;
        }
        else if(countLex == IDENT)
        {
            identMap[tempLex.GetLexeme()]++;
            idAkwCount++;
        }
        else if(countLex == TRUE || countLex == FALSE)
        {
            boolCount++;
        }
        else if((countLex >= IF && countLex <= PROGRAM) || countLex == AND || countLex == IDIV || countLex == MOD || countLex == OR || countLex == NOT)
        {
            keyWordMap[tempLex.GetLexeme()]++;
            idAkwCount++;
        }
    }

    /*ENDING LINES*/
    
    int totLine = linenum;

    if(totLine == 0)
    {
        cout << "Empty File." << endl;
        return 0;
    }

    cout << endl;
    cout << "Lines: " << linenum << endl;
    cout << "Total Tokens: " << tokCount << endl;
    cout << "Identifiers & Keywords: " << idAkwCount << endl;
    cout << "Numbers: " << numCount << endl;
    cout << "Booleans: " << boolCount << endl;
    cout << "Strings: " << stringCount << endl; 

    if(idsTF)
    {
        if(identMap.size() > 0)
        {
            cout << "IDENTIFIERS:" << endl;
            for(auto i = identMap.begin(); i != identMap.end(); i++)
            {
                if(next(i) != identMap.end())
                {
                    cout << i->first << " (" << i->second << "), ";
                }
                else
                {
                    cout << i->first << " (" << i->second << ")" << endl;
                }
            }
        }

        if(keyWordMap.size() > 0)
        {
            if(identMap.size() == 0)
            {
                cout << endl;
            }
            cout << "KEYWORDS:" << endl;
            for(auto i = keyWordMap.begin(); i != keyWordMap.end(); i++)
            {
                if(next(i) != keyWordMap.end())
                {
                    cout << i->first << " (" << i->second << "), ";
                }
                else
                {
                    cout << i->first << " (" << i->second << ")" << endl;
                }
            }
        }
    }

    if(numTF)
    {
        if(integerMap.size() > 0)
        {
            cout << "INTEGERS:" << endl;
            for(auto i = integerMap.begin(); i != integerMap.end(); i++)
            {
                if(next(i) != integerMap.end())
                {
                    cout << i->first << " (" << i->second << "), ";
                }
                else
                {
                    cout << i->first << " (" << i->second << ")" << endl;
                }
            }
        }

        if(realMap.size() > 0)
        {
            cout << "REALS:" << endl;
            for(auto i = realMap.begin(); i != realMap.end(); i++)
            {
                if(next(i) != realMap.end())
                {
                    cout << i->first << " (" << i->second << "), ";
                }
                else
                {
                    cout << i->first << " (" << i->second << ")" << endl;
                }
            }
        }
    }

    if(strTF)
    {
        if(strMap.size() > 0)
        {
            cout << "STRINGS:" << endl;
            for(auto i = strMap.begin(); i != strMap.end(); i++)
            {
                if(next(i) != strMap.end())
                {
                    cout << (i->first) << " (" << i->second << "), ";
                }
                else
                {
                    cout << (i->first) << " (" << i->second << ")" << endl;
                }
            }
        }
    }
}
