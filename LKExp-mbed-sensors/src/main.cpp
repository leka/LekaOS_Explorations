#include "mbed.h"
#include "board.h"

Board *board = new Board();

int main(void) {
    printf("--- Starting new run ---\r\n");
    board->check_id();
}
