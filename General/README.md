# General System Updater

Actualizador general para sistemas Linux basados en Debian. Ejecuta las operaciones de APT en procesos hijos y guarda un resumen con fecha y hora en `log.txt`.

> **Atención:** este programa cambia paquetes del sistema. Revísalo y pruébalo manualmente antes de automatizarlo en un servidor.

## Qué hace

1. Ejecuta `apt update`.
2. Si `apt update` termina correctamente, ejecuta `apt upgrade -y`.
3. Registra el inicio y el resultado de cada operación con fecha, hora y nivel (`INFO`, `OK` o `ERROR`).
4. Devuelve un código de salida distinto de cero si falla una operación.

El archivo `log.txt` se crea o se amplía en el directorio de trabajo desde el que se inicia el programa. Si no puede abrirse, los mensajes del programa se envían a `stderr`.

## Requisitos

- Linux con `apt`.
- `g++` y `make`.
- Privilegios de administrador para ejecutar APT.

## Compilar

Desde esta carpeta:

```bash
make
```

Se genera el ejecutable `updater`. Para borrarlo:

```bash
make clean
```

## Ejecutar

Ejecuta el programa como administrador desde el directorio donde quieras guardar `log.txt`:

```bash
sudo ./updater
```

El programa espera a que termine cada proceso hijo; primero actualiza los índices y luego, si todo va bien, instala las actualizaciones disponibles. `apt upgrade -y` acepta automáticamente la confirmación de actualización.

## Programar con cron

Para automatizarlo como `root`, edita el crontab del administrador:

```bash
sudo crontab -e
```

Por ejemplo, para ejecutarlo cada domingo a las 03:00 (sustituye la ruta por la ubicación real del ejecutable):

```cron
0 3 * * 0 cd "/ruta/a/Server Script/General" && ./updater >> ./cron.log 2>&1
```

La tarea se ejecutará con los permisos de `root`. `>>` añade la salida del comando a `cron.log` y `2>&1` incluye también los errores. El resumen de la aplicación se guarda además en `log.txt`.

No uses `chmod +s` ni setuid para conceder privilegios. Mantén el ejecutable y su carpeta protegidos contra modificaciones por usuarios no autorizados: cron ejecutará como `root` lo que allí se encuentre.

## Limitaciones

- La ejecución automática de `apt upgrade -y` puede instalar actualizaciones sin intervención; comprueba previamente que ese comportamiento te convenga.
- Este programa no crea copias de seguridad ni verifica servicios después de actualizar.
- Cron tradicional no recupera una ejecución que se haya perdido porque el servidor estaba apagado.
- Considera la rotación de `log.txt` y `cron.log` para evitar que crezcan indefinidamente.

---

Documentación preparada con ayuda de Copilot.
