#include "config.h"

#include <stdint.h>
#include <sanitizer/coverage_interface.h>

char* trace_bits;

void init_shm() {
    char* shm_str = getenv(SHM_ENV_VAR);

    if (shm_str) {
        int shm_id = atoi(shm_str);
        trace_bits = shmat(shm_id, NULL, 0);

        if (trace_bits == (void*)-1) {
            perror("shmat() failed");
            exit(1);
        }
    }
}

void init_forkserver() {
    int tmp;
    int do_fork;

    // if write failed, it indicates a normal run.
    if (write(FORKSRV_FD + 1, &tmp, sizeof(int)) != sizeof(int)) return;

    while (1) {
        pid_t child_pid;
        int status;

        if (read(FORKSRV_FD, &do_fork, sizeof(int)) != sizeof(int)) exit(1);

        child_pid = fork();

        if (child_pid < 0) {
            perror("forkserver failed");
            exit(1);
        } else if (child_pid == 0) {  // target instance
            close(FORKSRV_FD);
            close(FORKSRV_FD+1);

            return ;
        } else {    // forkserver

            if(write(FORKSRV_FD+1, &child_pid, sizeof(pid_t)) != sizeof(pid_t)) exit(1);

            waitpid(child_pid, &status, 0);
            printf("terminated child: %d (%d)\n", child_pid, WEXITSTATUS(status));
        }
    }
}

__attribute__((constructor)) void init_fuzz() {
    printf("initizlie\n");
    init_shm();
    init_forkserver();
}

void __sanitizer_cov_trace_pc_guard_init(
    uint32_t *start,
    uint32_t *stop
) {
    if (start == stop || *start) return;

    while (start < stop) {
        *start = 0;
        start++;
    }
}

void __sanitizer_cov_trace_pd_guard (
    uint32_t *guard
) {
    trace_bits[*guard]++;
}