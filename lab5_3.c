#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_FILE "config.txt"

char logLevel[16] = "info";
int logInterval = 5;
volatile sig_atomic_t reload_requested = 0;

void loadConfig(void) {
    FILE *f = fopen(CONFIG_FILE, "r");
    if (f == NULL) {
        perror("Не вдалося відкрити config.txt");
        return;
    }

    char level[16];
    int interval;
    if (fscanf(f, "%15s %d", level, &interval) == 2 && interval > 0) {
        snprintf(logLevel, sizeof(logLevel), "%s", level);
        logInterval = interval;
        printf("[CONFIG LOADED] logLevel: \"%s\", logInterval: %d\n", logLevel, logInterval);
    } else {
        printf("Помилка формату config.txt: очікується \"рівень інтервал\"\n");
    }
    fclose(f);
}

void handle_sigusr1(int sig) {
    (void)sig;
    reload_requested = 1;
}

int main(void) {
    pid_t pid = getpid();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    loadConfig();

    printf("\nPID процесу: %d\n", pid);
    printf("Команда для перезавантаження конфігурації:\n  kill -USR1 %d\n\n", pid);
    fflush(stdout);

    while (1) {
        if (reload_requested) {
            reload_requested = 0;
            printf("\n");
            loadConfig();
            printf("\n");
        }
        printf("[%s] Повідомлення системи (інтервал %d сек)...\n", logLevel, logInterval);
        fflush(stdout);
        sleep(logInterval);
    }

    return 0;
}