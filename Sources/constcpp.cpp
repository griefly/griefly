#include "constheader.h"
#include <Typelist.h>
#include <fstream>

unsigned long long MAIN_TICK = 0;

int DELAY_MAIN = 32;

int FPS_MAX = 256;

int TITLE_SIZE = 32;

int DRAW_MAX = 128;

bool NODRAW = false;

std::fstream loc("errorfile.txt", std::ios::trunc | std::ios::out);
std::ostream& SYSTEM_STREAM = std::cout;