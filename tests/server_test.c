#include "tests.h"
#include "player.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include <stdio.h>
#include <string.h>

bool is_valid_displacement(struct graph_t* board, size_t destination, enum color_t player);

static void setup(void){}
static void teardown(void){}

void test_is_valid_displacement(){
    printf("%s", __func__);
    if (0)
        FAIL("Where is the test brooo");
}


void test_server_main(void){
    TEST(test_is_valid_displacement);

    SUMMARY();
}