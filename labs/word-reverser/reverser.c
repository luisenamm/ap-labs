#include <stdio.h>

int main(){
    // Place your magic here

    int c;
    int words[100];
    int pos = 0;
    while ((c = getchar()) != EOF){
        if (c == '\n'){
            if (pos != 0){
                for (int i = pos - 1; i >= 0; i--){
                    printf("%c", words[i]);
                }
                printf("\n");
                pos = 0;
            }
        }
        else{
            words[pos] = c;
            pos++;
        }
    }
    return 0;
}
