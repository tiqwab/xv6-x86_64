#ifndef XV6_x86_64_USER_H
#define XV6_x86_64_USER_H

// system calls
int hello(void);
int print(char *msg);
int exec(char *path, char **argv);
int getpid(void);

#endif /* ifndef XV6_x86_64_USER_H */
