// clang -DENABLE_ASSERT=1 -o test ./test.c && ./test

#include "../base.c"

#define BUFFER_COUNT 1024

int main(void)
{
    char buffer_in[BUFFER_COUNT] = {0};
    chars buffer = SLICE_FROM_ARRAY(chars, buffer_in);

    chars path = getAppDir(buffer);
    if (NULL == path.ptr)
    {
        fprintf(stderr, "error during getAppDir()\n");
        return 1;
    }

    SLICE_PRINT(path);
    printf("path = %.*s\n", SLICE_FORMAT(path));

    return 0;
}
