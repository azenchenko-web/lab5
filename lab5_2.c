#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <execinfo.h>

volatile sig_atomic_t got_sigquit = 0;

void handle_sigquit(int sig) {
    (void)sig;
    void *buffer[32];
    int size = backtrace(buffer, 32);

    const char msg[] = "\n--- Stack Trace (SIGQUIT) ---\n";
    ssize_t r = write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    (void)r;
    backtrace_symbols_fd(buffer, size, STDOUT_FILENO);

    got_sigquit = 1;
}

int main(void) {
    pid_t pid = getpid();
    printf("Програму запущено. PID процесу: %d\n", pid);
    printf("Команда для надсилання сигналу з іншого терміналу:\n  kill -3 %d\n\n", pid);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    // Перший виклик backtrace() завантажує libgcc, тому робимо його заздалегідь
    void *warmup[1];
    backtrace(warmup, 1);

    printf("Очікування сигналу... (Натисніть Ctrl+\\ у цьому терміналі для перевірки)\n");
    fflush(stdout);

    while (!got_sigquit) {
        pause();
    }

    printf("\nОтримано SIGQUIT. Програму завершено.\n");
    return 0;
}