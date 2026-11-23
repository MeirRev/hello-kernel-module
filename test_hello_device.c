#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const char *path = "/dev/hello_device0";
    int status = 0;

    int fd1 = open(path, O_RDONLY);
    if (fd1 == -1) {
        perror("First open");
        return 1;
    }
    printf("First open succeeded: fd=%d\n", fd1);

    int fd2 = open(path, O_RDONLY);
    if (fd2 == -1) {
        perror("Second open");
        close(fd1);
        return 1;
    }
    printf("Second open succeeded: fd=%d\n", fd2);

    if (close(fd1) == -1) {
        perror("First close");
        status = 1;
    } else {
        puts("First close succeeded");
    }

    if (close(fd2) == -1) {
        perror("Second close");
        status = 1;
    } else {
        puts("Second close succeeded");
    }

    return status;
}