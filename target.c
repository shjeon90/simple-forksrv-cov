#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void buggy() {
    int (*fp)();

    fp = NULL;
    fp();
}

int main(int argc, char* argv[]) {
    int fd;
    char buf[1024];

    printf("I'm a fuzz target (pid: %d, ppid: %d)\n", getpid(), getppid());
    if (argc < 2) {
        perror("argc > 2");
        return 1;
    }

    printf("open a file\n");
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        perror("open() failed");
        return 1;
    }

    printf("read from the file\n");
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf("buf: %s\n", buf);

    if (buf[0] == 'h') {
        if (buf[1] == 'e') {
            if (buf[2] == 'l') {
                buggy();
            }
        }
    }

    close(fd);

    return 0;
}