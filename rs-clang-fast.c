#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char** cc_params;
unsigned int cc_par_cnt = 1;

void find_obj() {
    if (access("compiler-rt.o", F_OK)) {
        perror("compiler-rt.o does not exist in current directory.");
        exit(1);
    }
}

void edit_params (int argc, char* argv[]) {
    char* name;

    cc_params = (char**)malloc((argc + 16) * sizeof(char*));
    memset(cc_params, 0, (argc + 16) * sizeof(char*));

    name = strrchr(argv[0], '/');
    if (!name) name = argv[0];
    else name++;

    if (!strcmp(name, "rs-clang++")) cc_params[0] = "clang++";
    else cc_params[0] = "clang";

    cc_params[cc_par_cnt++] = "-fsanitize-coverage=trace-pc-guard";

    while (--argc) {
        char* cur = *(++argv);
        cc_params[cc_par_cnt++] = cur;
    }

    cc_params[cc_par_cnt++] = "compiler-rt.o";

    cc_params[cc_par_cnt] = NULL;

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("It needs at least two arguments");
        return 1;
    }

    find_obj();
    edit_params(argc, argv);

    execvp(cc_params[0], cc_params);

    return 0;
}