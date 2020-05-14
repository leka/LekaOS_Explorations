#include "mbed.h"
#include "board.h"

Board board;

int main(void) {
    printf("--- Starting new run ---\r\n");
    board.check_id();
}
