#include "base.h"

#if OS_MAC
#import <mach-o/dyld.h>

chars getAppDir(chars buffer)
{
    usize path_count = buffer.count;
    i32 read_count = 0;

    if (0 != _NSGetExecutablePath(buffer.ptr, (u32 *) &path_count))
    {
        return (chars) {.ptr = NULL, .count = 0};
    }

    return (chars) {
        .ptr = buffer.ptr,
        .count = (usize) path_count,
    };
}
#else
#error getAppDir(): unsupported OS
#endif
