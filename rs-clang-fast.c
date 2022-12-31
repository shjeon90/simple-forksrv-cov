#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char** cc_params;
unsigned int cc_par_cnt = 1;
char* path_compiler_rt;

void find_obj(char* arg0) {
    char* slash;

    slash = strrchr(arg0, '/');
    if (slash) {
        char* dir;

        *slash = '\0';
        dir = (char*)malloc(strlen(arg0) + 16);
        memset(dir, 0, strlen(arg0) + 16);
        strcpy(dir, arg0);
        *slash = '/';

        strcat(dir, "/compiler-rt.o");

        if (access(dir, F_OK)) {
            free(dir);
            perror("compiler-rt.o does not exist.");
            exit(1);
        }

        path_compiler_rt = dir;
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

    cc_params[cc_par_cnt++] = path_compiler_rt;

    cc_params[cc_par_cnt] = NULL;

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("It needs at least two arguments");
        return 1;
    }

    find_obj(argv[0]);
    edit_params(argc, argv);

    execvp(cc_params[0], cc_params);

    return 0;
}