#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    const char expected[] = "hello";
    const size_t expected_len = sizeof(expected) - 1;
    char chunk[2];
    size_t total = 0;
    int status = 0;

    int fd = open("/dev/hello_device0", O_RDONLY);
    if (fd == -1) {
        perror("open for reading");
        return 1;
    }

    for (;;) {
        ssize_t n = read(fd, chunk, sizeof(chunk));

        if (n == -1) {
            perror("read");
            status = 1;
            break;
        }

        if (n == 0)
            break;

        if ((size_t)n > expected_len - total ||
            memcmp(chunk, expected + total, (size_t)n) != 0) {
            fprintf(stderr, "FAIL: unexpected data at byte %zu\n", total);
            status = 1;
            break;
        }

        total += (size_t)n;
    }

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    if (status != 0)
        return status;

    if (total != expected_len) {
        fprintf(stderr, "FAIL: expected %zu bytes, read %zu\n",
                expected_len, total);
        return 1;
    }

    printf("PASS: read \"%s\" and reached EOF\n", expected);
    return 0;
}