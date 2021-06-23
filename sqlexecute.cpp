// #include <Python.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include "Interpreter.h"
#include "SqlError.h"
#include "Basicop.h"
// #include "Attribute.h"

using namespace std;


int main(int argc, char *argv[])
{
    string cmd;
    int i;
    if(argc == 2){
        cmd = argv[1];
    }else{
        printf("Wrong parameter number %d\n", argc);
        for(i=0; i<argc; i++){
            printf("%s\n", argv[i]);
        }
        return 0;
    }
    strip(cmd);
    if(cmd[cmd.length() - 1] != ';'){
        cout<<"[Error]: you must end your command by a \";\""<<endl;
    }
    cmd = cmd.substr(0, cmd.length() - 1);
    Interpreter I;
    I.Parse(cmd);
    delete &I;
    // system("pause");
    return 0;
}
