#include <stdio.h>

int digit_cnt(char* input){
    int count = 0;
    for(int i = 0;input[i] != '\0';i++){
        if(input[i] >='0' && input[i] <= '9'){
            count++;
        }
    }
    return count;
}

int main(int argc, char **argv){
    printf("%d\n", digit_cnt(argv[1]));
    return 0;
}