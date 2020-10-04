#include "user.h"

int main(int argc, char *argv[]) {
  fstest();

  int fd1, fd2;
  ssize_t n;
  char *dir_name = "foo";
  char *file_name = "bar.txt";
  char *message = "hello from write";
  char buf[128];

  if (mkdir(dir_name) < 0) {
    printf("failed to mkdir %s\n", dir_name);
    return 1;
  }

  if ((fd1 = open("/foo/bar.txt", O_CREATE | O_RDWR)) < 0) {
    printf("failed to open %s\n", "/foo/bar.txt");
    return 1;
  }

  if ((n = write(fd1, message, strlen(message))) < 0 ||
      (size_t)n != strlen(message)) {
    printf("failed to write to %s\n", file_name);
    return 1;
  }

  if (link("/foo/bar.txt", "/foo/baz.txt") < 0) {
    printf("failed to link. old: %s, new: %s\n", "/foo/bar.txt",
           "/foo/baz.txt");
    return 1;
  }

  if (chdir("foo") < 0) {
    printf("failed to chdir to %s\n", "foo");
    return 1;
  }

  if ((fd2 = open("baz.txt", O_RDONLY)) < 0) {
    printf("failed to open %s\n", "/foo/baz.txt");
    return 1;
  }

  if (read(fd2, buf, 128) < 0) {
    printf("failed to read from %s\n", file_name);
    return 1;
  }
  printf("%s\n", message);

  close(fd2);
  close(fd1);

  // check stdin, stdout, and stderr
  memset(buf, 0, sizeof(buf));
  printf("please enter something: ");
  if ((n = read(0, buf, 128)) < 0) {
    printf("failed to read from stdin\n");
    return 1;
  }
  if (write(1, buf, n) < 0) {
    printf("failed to write to stdout\n");
    return 1;
  }
  if (write(2, buf, n) < 0) {
    printf("failed to write to stderr\n");
    return 1;
  }

  // check pipe
  pid_t pid1;
  int pipefd[2];

  if (pipe(pipefd) < 0) {
    printf("failed to pipe\n");
    return 1;
  }

  if ((pid1 = fork()) < 0) {
    printf("failed to fork\n");
    return 1;
  } else if (pid1 > 0) {
    // parent: write to pipe
    close(pipefd[0]);
    write(pipefd[1], "pipe test", 10);
    close(pipefd[1]);
    wait();
  } else {
    // child: reads from pipe
    close(pipefd[1]);
    while (read(pipefd[0], buf, 1) > 0) {
      write(1, buf, 1);
    }
    write(1, "\n", 1);
    close(pipefd[0]);
    exit();
  }

  // check kill
  if ((pid1 = fork()) < 0) {
    printf("failed to fork\n");
    return 1;
  } else if (pid1 == 0) {
    // child
    for (;;) {
    }
  }

  // parent
  sleep(3);
  kill(pid1);
  wait();
  printf("kill and sleep test\n");

  return 0;
}
