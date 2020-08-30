#ifndef XV6_x86_64_USER_H
#define XV6_x86_64_USER_H

#define NULL 0

// system calls
int exec(char *path, char **argv);
int getpid(void);
int fork(void);

int hello(void);
int print(char *msg);

#endif /* ifndef XV6_x86_64_USER_H */
