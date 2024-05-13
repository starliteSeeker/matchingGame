#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include "graphics.h"

void redrawCard(int row, int col);
int calcY(int row);
int calcX(int col);

int gridW, gridH;
int cardW, cardH;
int padLeft, padUp;

WINDOW *cursor;
int cursorY = 0, cursorX = 0;

WINDOW **cards;
char randomVals[256];
char *randomCount;
char *cardVals;
char *cardFlags;
#define CARD_HIDE 0b10000000
#define CARD_FLIP 0b01000000
#define CARD_HASVAL 0b00100000
int first = -1, second = -1; // first and second card selection

WINDOW *right;
int guesses = 0;
int pairs = 0;

int initGfx(int h, int w) {
    if(w % 2 != 0 && h % 2 != 0) {
        printf("grid size must be even\n");
        return -1;
    }
    if(w * h > 256 * 2) {
        printf("grid size must not be greater than 512\n");
        return -1;
    }

    // ncurses setup
    initscr();
    noecho();
    raw();
    curs_set(0);

    // calculate card size and spacing
    int cols = COLS - 12;
    gridW = w;
    gridH = h;
    cardW = (cols - 1) / gridW - 1;
    cardH = (LINES - 1) / gridH - 1;
    if(cardW < CARDW_MIN || cardH < CARDH_MIN || LINES <= 12) {
        endwin();
        printf("terminal too small\n");
        return -1;
    }
    padLeft = (cols - (cardW + 1) * gridW + 1) / 2;
    padUp = (LINES - (cardH + 1) * gridH + 1) / 2;

    // draw right side
    right = newwin(LINES, 12, 0, cols);
    mvwvline(right, 0, 0, '*', LINES);
    mvwaddstr(right, 1, 2, "guesses:");
    mvwprintw(right, 2, 2, "%9d", guesses);
    mvwaddstr(right, LINES - 9, 2, "space");
    mvwaddstr(right, LINES - 8, 2, "to select");
    mvwaddstr(right, LINES - 6, 2, "w/a/s/d");
    mvwaddstr(right, LINES - 5, 2, "to move");
    mvwaddstr(right, LINES - 3, 2, "q");
    mvwaddstr(right, LINES - 2, 2, "to quit");
    wrefresh(right);

    // create cards
    cards = malloc(sizeof(WINDOW*) * gridW * gridH);
    for(int i = 0; i < gridH; i++) {
        for(int j = 0; j < gridW; j++) {
            WINDOW *card = newwin(cardH, cardW, calcY(i), calcX(j));
            box(card, 0, 0);
            wrefresh(card);
            cards[i * gridW + j] = card;
        }
    }
    cardFlags = calloc(gridW * gridH, sizeof(char));

    // create card values
    srand(time(NULL));
    cardVals = malloc(sizeof(char) * gridW * gridH);
    randomCount = calloc(gridW * gridH / 2, sizeof(char));
    for(int i = 0; i < 256; i++)
        randomVals[i] = i;
    for(int i = 0; i < gridW * gridH / 2; i++) {
        int r = rand() % (256 - i);
        int t = randomVals[255-r];
        randomVals[255-r] = randomVals[i];
        randomVals[i] = t;
    }

    // create cursor
    cursor = newwin(cardH + 2, cardW + 2, calcY(cursorY) - 1, calcX(cursorX) - 1);
    wborder(cursor, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
    return 0;
}

void mvCursorDown() {
    if(cursorY >= gridH - 1) {
        return;
    }

    // remove old cursor
    wborder(cursor, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
    // move cursor
    cursorY++;
    mvwin(cursor, calcY(cursorY) - 1, calcX(cursorX) - 1);
    wborder(cursor, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
}
void mvCursorUp() {
    if(cursorY <= 0) {
        return;
    }

    // remove old cursor
    wborder(cursor, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
    // move cursor
    cursorY--;
    mvwin(cursor, calcY(cursorY) - 1, calcX(cursorX) - 1);
    wborder(cursor, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
}
void mvCursorRight() {
    if(cursorX >= gridW - 1) {
        return;
    }

    // remove old cursor
    wborder(cursor, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
    // move cursor
    cursorX++;
    mvwin(cursor, calcY(cursorY) - 1, calcX(cursorX) - 1);
    wborder(cursor, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
}
void mvCursorLeft() {
    if(cursorX <= 0) {
        return;
    }

    // remove old cursor
    wborder(cursor, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
    // move cursor
    cursorX--;
    mvwin(cursor, calcY(cursorY) - 1, calcX(cursorX) - 1);
    wborder(cursor, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
    wrefresh(cursor);
    redrawCard(cursorY, cursorX);
}

void redrawCard(int row, int col) {
    WINDOW* card = cards[row * gridW + col];
    werase(card);

    if(cardFlags[row * gridW + col] & CARD_HIDE) {
        wrefresh(card);
        return;
    }

    if(cardFlags[row * gridW + col] & CARD_FLIP) {
        box(card, 0, 0);
        mvwprintw(card, 1, 1, "%02X", cardVals[row * gridW + col] & 0xff);
    } else {
        box(card, 0, 0);
    }
    wrefresh(card);
}

void select() {
    int temp = cursorY * gridW + cursorX;
    if(cardFlags[temp] & CARD_HIDE) 
        return;

    if(first == -1) {
        first = temp;
        // pick unseen value
        if((cardFlags[temp] & CARD_HASVAL) == 0) {
            for(int i = 0; i < gridW * gridH / 2; i++) {
                if(randomCount[i] == 0) {
                    cardVals[temp] = randomVals[i];
                    cardFlags[temp] |= CARD_HASVAL;
                    randomCount[i]++;
                    break;
                }
            }
        }
        // or pick any value
        if((cardFlags[temp] & CARD_HASVAL) == 0) {
            for(int i = 0; i < gridW * gridH / 2; i++) {
                if(randomCount[i] < 2) {
                    cardVals[temp] = randomVals[i];
                    cardFlags[temp] |= CARD_HASVAL;
                    randomCount[i]++;
                    break;
                }
            }
        }
    } else {
        if(temp == first) 
            return;

        second = temp;
        // pick seen value and not equal to first
        if((cardFlags[temp] & CARD_HASVAL) == 0) {
            for(int i = 0; i < gridW * gridH / 2; i++) {
                if(randomCount[i] == 1 && randomVals[i] != cardVals[first]) {
                    cardVals[temp] = randomVals[i];
                    cardFlags[temp] |= CARD_HASVAL;
                    randomCount[i]++;
                    break;
                }
            }
        }
        // or pick any value not equal to first
        if((cardFlags[temp] & CARD_HASVAL) == 0) {
            for(int i = 0; i < gridW * gridH / 2; i++) {
                if(randomCount[i] < 2 && randomVals[i] != cardVals[first]) {
                    cardVals[temp] = randomVals[i];
                    cardFlags[temp] |= CARD_HASVAL;
                    randomCount[i]++;
                    break;
                }
            }
        }
        // or pick any value
        if((cardFlags[temp] & CARD_HASVAL) == 0) {
            for(int i = 0; i < gridW * gridH / 2; i++) {
                if(randomCount[i] < 2) {
                    cardVals[temp] = randomVals[i];
                    cardFlags[temp] |= CARD_HASVAL;
                    randomCount[i]++;
                    break;
                }
            }
        }
    }
    cardFlags[temp] |= CARD_FLIP;
    redrawCard(cursorY, cursorX);
}

void unselect() {
    if(second != -1) {
        // chech for match
        if(cardVals[first] == cardVals[second]) {
            pairs++;
            cardFlags[first] |= CARD_HIDE;
            cardFlags[second] |= CARD_HIDE;
        } else {
            // flip back both card
            cardFlags[first] &= ~CARD_FLIP;
            cardFlags[second] &= ~CARD_FLIP;
        }
        redrawCard(first / gridW, first % gridW);
        redrawCard(second / gridW, second % gridW);
        first = -1;
        second = -1;

        // add guess counter
        guesses++;
        mvwprintw(right, 2, 2, "%9d", guesses);
        wrefresh(right);
    }
}

int calcY(int row) {
    return padUp + row * (cardH + 1);
}
int calcX(int col) {
    return padLeft + col * (cardW + 1);
}

void endGfx() {
    for(int i = 0; i < gridH; i++) {
        for(int j = 0; j < gridW; j++) {
            delwin(cards[i * gridW + j]);
        }
    }
    delwin(right);
    delwin(cursor);
    free(cards);
    free(cardVals);
    free(cardFlags);
    free(randomCount);
    endwin();
}
