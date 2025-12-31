#ifndef _BASE_H
#define _BASE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;
typedef ssize_t isize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#define MEMBER_OF(T, x) (((T *) 0)->x)
#define INT_FROM_PTR(x) (unsigned long long) ((char *) x)
#define OFFSET_OF(T, x) INT_FROM_PTR(&MEMBER_OF(T, x))
#define EVAL_PRINT(x) printf("%s = %d\n", #x, (int) (x))

#define STATEMENT(x) \
    do               \
    {                \
        x            \
    } while (false)

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(*(x)))

// #define ASSERT_BREAK() (*(int *) 0 = 0)
#define ASSERT_BREAK() __builtin_trap()
#ifdef BASE_ENABLE_ASSERT
#define ASSERT(x)                                                                       \
    if (!(x))                                                                           \
    {                                                                                   \
        fprintf(stderr, "assertion %s failed at %s line %d\n", #x, __FILE__, __LINE__); \
        ASSERT_BREAK();                                                                 \
    }
#else
#define ASSERT(x)
#endif

#define BYTE 1
#define KB 1024 * BYTE
#define MB 1024 * KB

#define UNREACHABLE ASSERT(false)

/// @brief Unit datatype with no values.
typedef struct _Unit
{
    char _nothing;
} Unit;

#define UNIT_DEFAULT (Unit){._nothing = '\0'}
static Unit unit = UNIT_DEFAULT;

#define DECLARE_SLICE(type_name, inner_type) \
    typedef struct                           \
    {                                        \
        inner_type *ptr;                     \
        usize count;                         \
    } type_name;

DECLARE_SLICE(chars, char)

#define SLICE_FROM_ARRAY(slice_type, array)       \
    (slice_type)                                  \
    {                                             \
        .ptr = array, .count = ARRAY_COUNT(array) \
    }

#define SLICE_FORMAT(slice) (i32) slice.count, slice.ptr
#define SLICE_PRINT(slice)                                                      \
    printf("%s = { ptr = %p, count = %lu }\n", #slice, slice.ptr, slice.count);

#define DECLARE_VECTOR2(type_name, inner_type) \
    typedef union {                            \
        struct                                 \
        {                                      \
            inner_type x;                      \
            inner_type y;                      \
        };                                     \
        inner_type v[2];                       \
    } type_name;

#define DECLARE_VECTOR3(type_name, inner_type) \
    typedef union {                            \
        struct                                 \
        {                                      \
            inner_type x;                      \
            inner_type y;                      \
            inner_type z;                      \
        };                                     \
        inner_type v[3];                       \
    } type_name;

#define DECLARE_VECTOR4(type_name, inner_type) \
    typedef union {                            \
        struct                                 \
        {                                      \
            inner_type x;                      \
            inner_type y;                      \
            inner_type z;                      \
            inner_type w;                      \
        };                                     \
        inner_type v[4];                       \
    } type_name;

DECLARE_VECTOR2(v2f32, f32)
DECLARE_VECTOR3(v3f32, f32)
DECLARE_VECTOR4(v4f32, f32)

/// When function may fail use this macro to create `Result` type and return it.
/// Do not use `NULL`s or `int`s as error indicator.
#define DECLARE_RESULT(type_name, V, E)                      \
    typedef struct                                           \
    {                                                        \
        union {                                              \
            V value;                                         \
            E error;                                         \
        };                                                   \
        bool is_ok;                                          \
    } type_name;                                             \
                                                             \
    static inline type_name type_name##_ok(V value)          \
    {                                                        \
        return (type_name) {.value = value, .is_ok = true};  \
    }                                                        \
                                                             \
    static inline type_name type_name##_err(E error)         \
    {                                                        \
        return (type_name) {.error = error, .is_ok = false}; \
    }

DECLARE_RESULT(u32r, u32, Unit)

#ifdef BASE_ENABLE_GL

#define GL_CLEAR_ERRORS() STATEMENT(while (GL_FALSE != glGetError());)

#ifdef ENABLE_GL_LOG
#define GL_CALL(x) STATEMENT(GL_CLEAR_ERRORS(); x; ASSERT(glLogCall(#x, __FILE__, __LINE__));)
static bool glLogCall(char const *function_name, char const *file_name, int line)
{
    GLenum error;
    while (GL_FALSE != (error = glGetError()))
    {
        fprintf(stderr, "[OpenGL ERROR] %s\n  at %s line %d\n", function_name, file_name, line);
        return false;
    }

    return true;
}
#else
#define GL_CALL(x) STATEMENT(x;)
#endif

#endif // BASE_ENABLE_GL

/// Returns a view into the buffer containing the path.
///
/// Arguments:
/// - `buffer`: Slice that will hold the result.
///
/// Returns: Slice that points inside the `buffer`.
///
/// On error: Returns an empty slice pointing to null with count = 0.
chars getAppDir(chars buffer);

#ifdef BASE_IMPLEMENTATION

#include <mach-o/dyld.h>

// MAXPATHLEN is defined here. We can maybe use it somehow.
// #include <sys/param.h>

chars getAppDir(chars buffer)
{
    usize path_count = buffer.count;
    i32 read_count = 0;

    // From
    // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html:
    //
    // Note that _NSGetExecutablePath() will return "a path" to
    // the executable not a "real path" to the executable.  That is, the path
    // may be a symbolic link and not the real file. With deep directories the
    // total bufsize needed could be more than MAXPATHLEN.
    if (0 != _NSGetExecutablePath(buffer.ptr, (u32 *) &path_count))
    {
        return (chars) {.ptr = NULL, .count = 0};
    }

    return (chars) {
        .ptr = buffer.ptr,
        .count = (usize) path_count,
    };
}

#endif // BASE_IMPLEMENTATION

#endif // _BASE_H
