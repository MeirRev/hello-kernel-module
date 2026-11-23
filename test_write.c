#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const char message[] = "hello";
    int fd = open("/dev/hello_device0", O_WRONLY);
    if (fd == -1) {
        perror("open for writing");
        return 1;
    }

    ssize_t written = write(fd, message, sizeof(message) - 1);
    if (written == -1)
        perror("write");

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    if (written == -1)
        return 1;

    if (written != (ssize_t)(sizeof(message) - 1)) {
        fprintf(stderr, "FAIL: expected 5 bytes, wrote %zd\n", written);
        return 1;
    }

    printf("PASS: wrote %zd bytes\n", written);
    return 0;
}