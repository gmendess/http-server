#ifndef __HTTP_UTIL_H
#define __HTTP_UTIL_H

void net_panic(const char* func_name, const int err);
void errno_panic(const char* func_name);

#endif // __HTTP_UTIL_H