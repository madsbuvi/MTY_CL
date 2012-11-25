#ifndef LOG_H__
#define LOG_H__

extern int log_open(char const *name);
extern void log_print(int full, char const *hash, uint8_t const *key);

#endif /* LOG_H__ */
