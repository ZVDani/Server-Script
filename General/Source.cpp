#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Escribe un mensaje con fecha y hora en el archivo de log o, si no está
// disponible, en la salida de error estándar.
void logMessage(FILE* log, const char* level, const char* message) {
    time_t now = time(NULL);
    struct tm localTime;
    char timestamp[20] = "fecha no disponible";

    if (now != (time_t)-1 && localtime_r(&now, &localTime) != NULL) {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &localTime);
    }

    FILE* output = log != NULL ? log : stderr;
    fprintf(output, "[%s] %-5s | %s\n", timestamp, level, message);
    fflush(output);
}

// Ejecuta "sudo apt <operación>" en un proceso hijo y espera su resultado.
// Devuelve el código de salida del comando, o -1 si hubo un error al crearlo
// o esperarlo.
int runAptCommand(const char* operation) {
    pid_t child = fork();

    if (child == -1) {
        perror("fork");
        return -1;
    }

    if (child == 0) {
        execlp("sudo", "sudo", "apt", operation, (char*)NULL);
        perror("exec sudo");
        _exit(127);
    }

    int status;
    if (waitpid(child, &status, 0) == -1) {
        perror("waitpid");
        return -1;
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

int main() {
    // Abre el registro en modo de anexado para conservar ejecuciones anteriores.
    FILE* log = fopen("log.txt", "a");

    if (log == NULL) {
        printf("El archivo no se pudo abrir, trabajando sin logs\n");
    }

    // Ejecuta upgrade únicamente si la actualización de los índices tuvo éxito.
    int exitCode = 0;
    logMessage(log, "INFO", "Iniciando apt update.");
    if (runAptCommand("update") != 0) {
        logMessage(log, "ERROR", "apt update ha fallado; no se ejecutara upgrade.");
        exitCode = 1;
    } else {
        logMessage(log, "OK", "apt update se completo correctamente.");

        logMessage(log, "INFO", "Iniciando apt upgrade.");
        if (runAptCommand("upgrade") != 0) {
            logMessage(log, "ERROR", "apt upgrade ha fallado.");
            exitCode = 1;
        } else {
            logMessage(log, "OK", "apt upgrade se completo correctamente.");
        }
    }

    // Registra el resultado general, cierra el archivo y comunica el estado
    // mediante el código de salida del programa.
    logMessage(log, exitCode == 0 ? "OK" : "ERROR",
               exitCode == 0 ? "Todas las operaciones finalizaron correctamente."
                             : "El proceso termino con errores.");

    if (log != NULL) {
        fclose(log);
    }

    return exitCode;
}