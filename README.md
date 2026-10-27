# Server Script

Colección de pequeños programas en C++ para automatizar tareas de mantenimiento en servidores Linux. Cada carpeta contiene una variante independiente, su código fuente y un `Makefile`.

## Proyectos

| Carpeta | Descripción |
|---|---|
| [`General/`](./General/) | Ejecuta `apt update` y, si tiene éxito, `apt upgrade -y`. |
| [`Immich/`](./Immich/) | Actualiza los paquetes del sistema y luego actualiza un despliegue de Immich con Docker Compose desde `/Immich`. |

Cada subproyecto tiene su propio README con instrucciones específicas de compilación, ejecución, cron y precauciones:

- [Documentación de General](./General/README.md)
- [Documentación de Immich](./Immich/README.md)

## Requisitos generales

- Linux. Los programas usan llamadas POSIX como `fork`, `exec` y `waitpid`.
- `g++` y `make` para compilar.
- Privilegios de administrador al ejecutar las tareas de APT.
- Para la variante Immich: Docker, el complemento `docker compose` y el proyecto Compose ubicado en `/Immich`.

## Compilación

Compila cada variante desde su carpeta:

```bash
cd General
make
```

o:

```bash
cd Immich
make
```

Los ejecutables se generan localmente en sus respectivas carpetas. Consulta el README de cada variante para conocer el directorio de trabajo requerido y cómo configurar una tarea programada.

## Registros y privilegios

Los programas añaden mensajes fechados a `log.txt` en su directorio de trabajo. APT y Docker también pueden imprimir salida propia en la terminal o en el registro configurado por cron.

Estos ejecutables realizan cambios privilegiados en el sistema. No concedas privilegios usando setuid (`chmod +s`): ejecútalos mediante un mecanismo administrativo explícito, como el crontab de `root`, y restringe quién puede modificar el código, los ejecutables y las configuraciones que se ejecutarán con esos permisos.

## Aviso

Las actualizaciones automáticas pueden instalar paquetes o desplegar nuevas versiones de servicios sin supervisión. Revisa el código, prueba manualmente, mantén copias de seguridad apropiadas y comprueba los servicios después de cada despliegue. Este proyecto no incluye una estrategia de rollback ni realiza una verificación de salud de Immich.

---

Documentación preparada con ayuda de Copilot.
