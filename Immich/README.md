# Immich Server Updater

Pequeño actualizador para servidores Linux con **APT**, **Docker** y **Docker Compose**. Automatiza la actualización del sistema y la renovación de los contenedores de Immich, dejando un registro con fecha y hora.

> **Importante:** este programa modifica paquetes del sistema y despliega contenedores. Revísalo y pruébalo manualmente antes de programarlo en un servidor.

## Qué hace

En cada ejecución, sigue este orden:

1. Ejecuta `apt update`.
2. Si el paso anterior funciona, ejecuta `apt upgrade -y`.
3. Ejecuta `docker compose pull` desde `/Immich`.
4. Si `pull` funciona, ejecuta `docker compose up -d`.
5. Escribe el resultado de cada operación en `log.txt`, con fecha, hora y nivel (`INFO`, `OK` o `ERROR`).

Si una operación falla, registra el error y devuelve un código de salida distinto de cero. Un fallo de APT no impide que se intente actualizar Immich; `up -d` sí se omite si falla `pull`.

El programa registra sus propios mensajes en `log.txt`, en el directorio de trabajo desde el que se inicia. La salida directa de APT y Docker va a la salida estándar o de error del proceso.

## Requisitos

- Linux con `apt`.
- `g++` y `make` para compilar.
- Docker y el complemento `docker compose`.
- El archivo de configuración Compose del despliegue en `/Immich`.

## Compilar

Desde esta carpeta:

```bash
make
```

Se genera el ejecutable `immich-updater`. Para borrarlo:

```bash
make clean
```

## Probar manualmente

Ejecuta el programa desde `/Immich`, donde está el archivo Compose:

```bash
cd /Immich
sudo "/ruta/a/Immich/immich-updater"
```

Sustituye `/ruta/a/Immich` por la ruta de esta carpeta del repositorio. Revisa `/Immich/log.txt` y la salida de la terminal para confirmar el resultado.

## Programar con cron

Como APT necesita privilegios de administrador y el acceso a Docker suele ser privilegiado, una opción sencilla es instalar la tarea en el crontab de `root`:

```bash
sudo crontab -e
```

Por ejemplo, para ejecutarla cada domingo a las 03:00, añade una línea como esta, sustituyendo la ruta del ejecutable:

```cron
0 3 * * 0 cd /Immich && /ruta/a/Immich/immich-updater >> /Immich/cron.log 2>&1
```

Los cinco campos de cron son minuto, hora, día del mes, mes y día de la semana. `>>` añade la salida de APT/Docker a `cron.log` sin borrar ejecuciones anteriores, y `2>&1` incluye también los errores. Los mensajes resumidos del programa se guardan en `/Immich/log.txt`.

No hace falta cambiar los permisos del ejecutable para convertirlo en administrador. **No uses `chmod +s` ni setuid**: dar privilegios de administrador de esa forma a un programa propio es inseguro. Usa permisos normales y restringe quién puede modificar el ejecutable, el código y la configuración Compose, ya que la tarea de cron de `root` ejecutará esos archivos con privilegios elevados.

## Limitaciones y precauciones

- `apt upgrade -y` acepta automáticamente las actualizaciones disponibles. El modo no interactivo evita preguntas durante cron, pero conviene revisar previamente cómo se comporta en tu distribución y ante cambios de configuración de paquetes.
- El programa no crea copias de seguridad ni verifica que Immich funcione después del despliegue.
- Cron puede ejecutarse aunque el servidor esté apagado a la hora programada; cron tradicional no recupera automáticamente las ejecuciones perdidas.
- Protege los logs y rota o limpia periódicamente los registros si el servidor tiene espacio limitado.

---

Documentación preparada con ayuda de Copilot.
