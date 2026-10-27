#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Escribe un mensaje con fecha y hora en el log o, si no está disponible,
// en la salida de error estándar.
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

// Crea un proceso hijo, opcionalmente cambia su directorio y reemplaza el
// proceso hijo con el comando solicitado. Devuelve su código de salida.
int runCommand(FILE* log, const char* description, const char* program,
               char* const arguments[], const char* workingDirectory) {
    char message[160];
    snprintf(message, sizeof(message), "Iniciando %s.", description);
    logMessage(log, "INFO", message);

    pid_t child = fork();
    if (child == -1) {
        snprintf(message, sizeof(message), "No se pudo crear el proceso para %s: %s.",
                 description, strerror(errno));
        logMessage(log, "ERROR", message);
        return -1;
    }

    if (child == 0) {
        if (workingDirectory != NULL && chdir(workingDirectory) == -1) {
            perror("chdir");
            _exit(126);
        }

        if (strcmp(program, "apt") == 0) {
            setenv("DEBIAN_FRONTEND", "noninteractive", 1);
        }

        execvp(program, arguments);
        perror("execvp");
        _exit(127);
    }

    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited == -1 && errno == EINTR);

    if (waited == -1) {
        snprintf(message, sizeof(message), "Error esperando a %s: %s.",
                 description, strerror(errno));
        logMessage(log, "ERROR", message);
        return -1;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        snprintf(message, sizeof(message), "%s se completo correctamente.", description);
        logMessage(log, "OK", message);
        return 0;
    }

    if (WIFEXITED(status)) {
        snprintf(message, sizeof(message), "%s termino con codigo %d.",
                 description, WEXITSTATUS(status));
    } else {
        snprintf(message, sizeof(message), "%s termino de forma inesperada.", description);
    }
    logMessage(log, "ERROR", message);
    return -1;
}

int main() {
    // Abre el log en modo de anexado para conservar el historial.
    FILE* log = fopen("log.txt", "a");
    if (log == NULL) {
        fprintf(stderr, "No se pudo abrir log.txt; se usara stderr.\n");
    }

    int exitCode = 0;

    // Actualiza los índices y los paquetes del sistema antes de Immich.
    char aptName[] = "apt";
    char updateOperation[] = "update";
    char* aptUpdate[] = {aptName, updateOperation, NULL};
    char upgradeOperation[] = "upgrade";
    char yesOption[] = "-y";
    char* aptUpgrade[] = {aptName, upgradeOperation, yesOption, NULL};

    if (runCommand(log, "apt update", "apt", aptUpdate, NULL) != 0) {
        exitCode = 1;
        logMessage(log, "ERROR", "Se omite apt upgrade porque apt update fallo.");
    } else if (runCommand(log, "apt upgrade", "apt", aptUpgrade, NULL) != 0) {
        exitCode = 1;
    }

    // Docker Compose se ejecuta desde el directorio que contiene la
    // configuración del despliegue de Immich.
    char dockerName[] = "docker";
    char composeCommand[] = "compose";
    char pullCommand[] = "pull";
    char* composePull[] = {dockerName, composeCommand, pullCommand, NULL};
    char upCommand[] = "up";
    char detachedOption[] = "-d";
    char* composeUp[] = {dockerName, composeCommand, upCommand, detachedOption, NULL};

    if (runCommand(log, "docker compose pull", "docker", composePull, "/Immich") != 0) {
        exitCode = 1;
        logMessage(log, "ERROR", "Se omite docker compose up -d porque pull fallo.");
    } else if (runCommand(log, "docker compose up -d", "docker", composeUp, "/Immich") != 0) {
        exitCode = 1;
    }

    logMessage(log, exitCode == 0 ? "OK" : "ERROR",
               exitCode == 0 ? "Todas las operaciones finalizaron correctamente."
                             : "El proceso termino con errores.");

    if (log != NULL) {
        fclose(log);
    }

    return exitCode;
}
