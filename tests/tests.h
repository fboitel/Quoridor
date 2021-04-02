#ifndef _QUOR_TESTS_H_
#define _QUOR_TESTS_H_

// Test macros
static int pass;
static int passed = 0;
static int total = 0;
#define TEST(f) pass = 1, setup(), f(), teardown(), printf("\t%s\n", pass ? "OK" : "FAIL"), passed += pass, total++
#define FAIL(msg) fprintf(stderr, "ERROR: %s\n", msg), pass = 0

void test_player_main(void);

#endif // _QUOR_TESTS_H_