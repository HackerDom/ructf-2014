#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

void info(const char *format, ...);
void error(const char *format, ...);

#ifdef DEBUG
#define debug(format, ...) info(format, ##__VA_ARGS__)
#else
#define debug(format, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif