#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "graphics.h"

int main(int argc, char **argv)
{
    // grid size stuff
    int h, w;
    if(argc == 1) {
        w = 4;
        h = 3;
    } else if(argc == 3) {
        w = atoi(argv[2]);
        h = atoi(argv[1]);
    } else {
        printf("%s [height width ]\n", argv[0]);
        return -1;
    }

    if(w <= 0 || h <= 0) {
        printf("negative width/height grid\n");
        return -1;
    }
    if(initGfx(h, w)) {
        return EXIT_FAILURE;
    }

    int ch;
    while((ch = getchar()) != 'q') {
        unselect();
        switch(ch) {
            case 'w': 
                mvCursorUp();
                break;
            case 'a': 
                mvCursorLeft();
                break;
            case 's': 
                mvCursorDown();
                break;
            case 'd': 
                mvCursorRight();
                break;
            case ' ':
                select();
                break;
        }
    }

    endGfx();
    return 0;
}
