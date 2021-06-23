#include<iostream>
using namespace std;

int main(int argc, char* argv[]){
    if(argc == 2){
        printf("%s\n", argv[1]);
    }else{
        printf("not enough\n");
    }
    return 0;
}
