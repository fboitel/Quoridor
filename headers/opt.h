#ifndef _QUOR_OPT_H_
#define _QUOR_OPT_H_

enum shape_t { SQUARE, TORIC, H, SNAKE, INVALID_SHAPE = -1 };

void parse_args(int argc, char** argv);

#endif // _QUOR_OPT_H_