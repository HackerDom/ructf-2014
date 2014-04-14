#pragma once

#ifdef DEBUG
#define D(...) fprintf(stderr,__VA_ARGS__);
#else
#define D(...) {}
#endif
