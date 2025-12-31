#ifndef _BASE_H
#define _BASE_H

#if !defined(ENABLE_ASSERT)
#define ENABLE_ASSERT 0
#endif
// #if !defined(ENABLE_SANITIZER)
// #define ENABLE_SANITIZER 0
// #endif
// #if !defined(ENABLE_MANUAL_PROFILE)
// #define ENABLE_MANUAL_PROFILE 0
// #endif
// #if !defined(ENABLE_AUTO_PROFILE)
// #define ENABLE_AUTO_PROFILE 0
// #endif

// #if defined(ENABLE_ANY_PROFILE)
// #error user should not configure ENABLE_ANY_PROFILE
// #endif

// #if ENABLE_MANUAL_PROFILE || ENABLE_AUTO_PROFILE
// #define ENABLE_ANY_PROFILE 1
// #else
// #define ENABLE_ANY_PROFILE 0
// #endif

#if defined(__clang__)
#define COMPILER_CLANG 1

#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#error missing OS detection
#endif

#if defined(__amd64__)
#define ARCH_X64 1
#elif defined(__i386__)
#define ARCH_X86 1
#elif defined(__arm__)
#define ARCH_ARM 1
#elif defined(__aarch64__)
#define ARCH_ARM64 1
#else
#error missing ARCH detection
#endif

#elif defined(_MSC_VER)
#define COMPILER_CL 1

#if defined(_WIN32)
#define OS_WINDOWS 1
#else
#error missing OS detection
#endif

#if defined(_M_AMD64)
#define ARCH_X64 1
#elif defined(_M_I86)
#define ARCH_X86 1
#elif defined(_M_ARM)
#define ARCH_ARM 1
// TODO(allen): ARM64?
#else
#error missing ARCH detection
#endif

#elif defined(__GNUC__)
#define COMPILER_GCC 1

#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#error missing OS detection
#endif

#if defined(__amd64__)
#define ARCH_X64 1
#elif defined(__i386__)
#define ARCH_X86 1
#elif defined(__arm__)
#define ARCH_ARM 1
#elif defined(__aarch64__)
#define ARCH_ARM64 1
#else
#error missing ARCH detection
#endif

#else
#error no context cracking for this compiler
#endif

#if !defined(COMPILER_CL)
#define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
#define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif
#if !defined(OS_WINDOWS)
#define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
#define OS_LINUX 0
#endif
#if !defined(OS_MAC)
#define OS_MAC 0
#endif
#if !defined(ARCH_X64)
#define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
#define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
#define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
#define ARCH_ARM64 0
#endif

#if defined(__cplusplus)
#define LANG_CXX 1
#else
#define LANG_C 1
#endif

#if !defined(LANG_CXX)
#define LANG_CXX 0
#endif
#if !defined(LANG_C)
#define LANG_C 0
#endif

// #if !defined(PROFILER_SPALL)
// #define PROFILER_SPALL 0
// #endif

// #if OS_WINDOWS
// #if COMPILER_CL || COMPILER_CLANG
// #define INTRINSICS_MICROSOFT 1
// #endif
// #endif

// #if !defined(INTRINSICS_MICROSOFT)
// #define INTRINSICS_MICROSOFT 0
// #endif

#if ARCH_X64 || ARCH_ARM64
#define ARCH_ADDRSIZE 64
#else
#define ARCH_ADDRSIZE 32
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef ssize_t isize;

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

#if COMPILER_GCC || COMPILER_CLANG
#define ASSERT_BREAK() __builtin_trap()
#else
#define ASSERT_BREAK() (*(int *) 0 = 0)
#endif

#if ENABLE_ASSERT
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

#if !defined(ENABLE_OPENGL)
#define ENABLE_OPENGL 0
#endif

#if ENABLE_OPENGL

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

#endif // ENABLE_OPENGL

/// Returns a view into the buffer containing the path.
///
/// Arguments:
/// - `buffer`: Slice that will hold the result.
///
/// Returns: Slice that points inside the `buffer`.
///
/// On error: Returns an empty slice pointing to null with count = 0.
chars getAppDir(chars buffer);

#endif // _BASE_H
