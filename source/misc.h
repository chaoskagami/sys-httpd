#pragma once

#define ESC(x)
#define RESET
#define BLACK
#define RED
#define GREEN
#define YELLOW
#define BLUE
#define MAGENTA
#define CYAN
#define WHITE

#include <switch.h>

extern int should_log;


void log_init(void);
void fatalLater(Result err);

