#include "config.h"

int shm_id;
char* trace_bits;
pid_t child_pid;
pid_t forksrv_pid;
int exec_tmout = EXEC_TIMEOUT;
int fsrv_read;
int fsrv_write;

void simle_checksum() {
    unsigned int checksum = 0;

    for (int i = 0;i < MAP_SIZE;++i) {
        checksum += trace_bits[i];
    }

    printf("checksum: %u\n", checksum);
}

void remove_shm() {
    printf("remove shared memory\n");
    shmctl(shm_id, IPC_RMID, NULL);
}

void init_shm() {
    char shm_str[16];

    // create shared memory
    shm_id = shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id < 0) {
        perror("shmget() failed");
        exit(1);
    }

    atexit(remove_shm);

    memset(shm_str, 0, sizeof(shm_str));
    sprintf(shm_str, "%d", shm_id);

    printf("shm str: %s\n", shm_str);

    // set env to tell fuzz target is in fuzzer.
    setenv(SHM_ENV_VAR, shm_str, 1);

    trace_bits = shmat(shm_id, NULL, 0);
    if (trace_bits == (void*)-1) {
        perror("shmat() failed");
        exit(1);
    }
}

void init_forkserver(char* argv[]) {
    int status;
    int st_pipe[2], ctl_pipe[2];
    static struct itimerval it;

    if (pipe(st_pipe) || pipe(ctl_pipe)) {
        perror("pipe() failed");
        exit(1);
    }

    forksrv_pid = fork();
    if (forksrv_pid < 0) {
        perror("fork() failed");

        exit(1);
    } else if (forksrv_pid == 0) {    // forkserver
        int dev_null_fd = open("/dev/null", O_RDWR);

        dup2(dev_null_fd, STDOUT_FILENO);
        dup2(dev_null_fd, STDERR_FILENO);

        dup2(ctl_pipe[0], FORKSRV_FD);
        dup2(st_pipe[1], FORKSRV_FD + 1);

        close(ctl_pipe[0]);
        close(st_pipe[0]);

        execvp(argv[0], argv);
    }

    fsrv_read = st_pipe[0];
    fsrv_write = ctl_pipe[1];

    // check forkserver was up
    if (read(fsrv_read, &status, sizeof(int)) != sizeof(int)) exit(1);
}

static void handle_stop_sig(int sig) {
  if (child_pid > 0) kill(child_pid, SIGKILL);
  if (forksrv_pid > 0) kill(forksrv_pid, SIGKILL);

  exit(0);
}

void setup_signal_handlers() {
    signal(SIGINT, handle_stop_sig);
    signal(SIGTERM, handle_stop_sig);
    signal(SIGHUP, handle_stop_sig);
    signal(SIGALRM, handle_stop_sig);
}

void run_target() {
    int do_fork;
    pid_t child_pid;
    int status;

    memset(trace_bits, 0, MAP_SIZE);

    // make fuzz target
    printf("tell forksrv to spawn a fuzz target\n");
    if (write(fsrv_write, &do_fork, sizeof(int)) != sizeof(int)) exit(1);

    // read child_pid
    if (read(fsrv_read, &child_pid, sizeof(pid_t)) != sizeof(pid_t)) exit(1);
    printf("pid fuzz target: %d\n", child_pid);

    // do fuzz stuff?

    //read exit status of fuzz target
    if (read(fsrv_read, &status, sizeof(int)) != sizeof(int)) exit(1);
    printf("exit fuzz target: %d\n", status);
        
}

int main(int argc, char* argv[]) {
    char* pargv[argc];

    if (argc < 2) {
        perror("/path/to/bin /path/to/child");
        return 1;
    }

    memset(pargv, 0, sizeof(pargv));
    for (int i = 0;i < argc-1;++i) {
        pargv[i] = argv[i+1];
    }

    setup_signal_handlers();

    init_shm();
    init_forkserver(pargv);

    while (1) {
        run_target();
        simle_checksum();
    }

    return 0;
}