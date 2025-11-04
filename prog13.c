#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    for(int i = 0; i < 15; i++){
        fprintf(stderr, "%d\n", i);
        sleep(1);
    }
    return EXIT_SUCCESS;
}

//when we use printf the text isnt always sent immediately to the screen or file, its stored in a temporary memory area (a buffer)
// system writes it out when: the buffer fils up or we print newline, the program ends
//stderr is never buffered