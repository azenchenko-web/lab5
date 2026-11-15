#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#define PROGRAM_NAME "pwd_80_12"
#define VERSION "1.0.0"
#define AUTHOR "Андрій Зенченко"

static void print_help(void) {
    printf("Використання: %s [-L | -P] [ОПЦІЇ]\n", PROGRAM_NAME);
    printf("Виводить ім'я поточної робочої директорії.\n\n");
    printf("  -L         Вивести логічний шлях (з $PWD, враховуючи символічні лінки)\n");
    printf("  -P         Вивести фізичний шлях (розкриваючи всі символічні лінки)\n");
    printf("  --help     Показати цю довідку та завершити роботу\n");
    printf("  --version  Показати версію утиліти\n");
    printf("  --author   Показати ім'я та прізвище розробника\n");
}

int main(int argc, char *argv[]) {
    int physical = 0;
    int opt;

    static struct option long_options[] = {
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"author",  no_argument, NULL, 'a'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "LP", long_options, NULL)) != -1) {
        switch (opt) {
            case 'L':
                physical = 0;
                break;
            case 'P':
                physical = 1;
                break;
            case 'h':
                print_help();
                return 0;
            case 'v':
                printf("%s версія %s\n", PROGRAM_NAME, VERSION);
                return 0;
            case 'a':
                printf("Розробник: %s\n", AUTHOR);
                return 0;
            default:
                fprintf(stderr, "Спробуйте '%s --help' для отримання довідки.\n", PROGRAM_NAME);
                return 1;
        }
    }

    if (physical) {
        char *path = realpath(".", NULL);
        if (path == NULL) {
            perror("realpath");
            return 1;
        }
        printf("%s\n", path);
        free(path);
    } else {
        const char *path = getenv("PWD");
        if (path != NULL) {
            printf("%s\n", path);
        } else {
            char buf[PATH_MAX];
            if (getcwd(buf, sizeof(buf)) == NULL) {
                perror("getcwd");
                return 1;
            }
            printf("%s\n", buf);
        }
    }

    return 0;
}