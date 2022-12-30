#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SHM_ENV_VAR         "__FUZZ_ID"
#define MAP_SIZE_POW2       16
// size 65536
#define MAP_SIZE            (1 << MAP_SIZE_POW2)

// FORKSRV_FD -> read
// FORKSRV_FD + 1-> write
int FORKSRV_FD = 200;

#endif // CONFIG_H