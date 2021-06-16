#include "SqlError.h"

SqlError::SqlError(string msg){
    this->msg = msg;
}


SyntaxError::SyntaxError(string msg): SqlError(msg){
    
}

DBError::DBError(string msg): SqlError(msg){
    
}
