#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define POSICION_MAXIMA 52
#define Kp 0.1
#define Ki 0.01
#define Kd 0.05

// Función para convertir la posición en un ángulo
double calcular_angulo(int posicion) {
    return (posicion - 1) * (360.0 / POSICION_MAXIMA);
}

// Busca evitar que la margarita de la vuelta más larga para llegar al ángulo deseado, ya que es 360 grados
double calculo_error_minimo(double angulo_deseado, double angulo_actual) {
    double error = angulo_deseado - angulo_actual;
    if (error > 180) {
        error -= 360;
    } else if (error < -180) {
        error += 360;
    }
    return error;
}

// Función para el controlador PID, ajustando el tiempo en el cálculo
double pid_control(double angulo_deseado, double angulo_actual, double *integral, double *error_previo, double tiempo_transcurrido) {
    double error = calculo_error_minimo(angulo_deseado, angulo_actual);
    *integral += error * tiempo_transcurrido; // Ajuste para el término integral
    double derivativo = (error - *error_previo) / tiempo_transcurrido; // Ajuste para el término derivativo
    *error_previo = error;
    return Kp * error + Ki * (*integral) + Kd * derivativo;
}

// Función para mapear una letra a una posición numérica
int letra_a_posicion(char letra) {
    if (isalpha(letra)) {
        if (islower(letra)) {
            return letra - 'a' + 1; // Para minúsculas
        } else {
            return letra - 'A' + 27; // Para mayúsculas
        }
    }
    return -1; // Retorna -1 si no es una letra válida
}

int main() {
    char letra_deseada;
    double perturbacion;
    int posicion_deseada;
    double angulo_deseado, angulo_actual = 0.0;
    double integral = 0.0;
    double error_previo = 0.0;
    double senal_de_control;
    FILE* archivo;

    // Inicializa el generador de números aleatorios
    srand(time(NULL));

    printf("Ingrese una letra (a-z, A-Z) para la posicion deseada de la margarita: ");
    scanf(" %c", &letra_deseada);

    // Mapea la letra a una posición en número
    posicion_deseada = letra_a_posicion(letra_deseada);

    // Se valida la entrada del usuario
    if (posicion_deseada < 1 || posicion_deseada > POSICION_MAXIMA) {
        printf("Error: La letra debe estar entre 'a' y 'z' o 'A' y 'Z'.\n");
        return 1;
    }

    // Se calcula el ángulo deseado correspondiente
    angulo_deseado = calcular_angulo(posicion_deseada);

    archivo = fopen("data.dat", "a");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    clock_t tiempo_inicial, tiempo_actual;
    double tiempo_transcurrido;

    int iteracion = 0;
    tiempo_inicial = clock();

    while (1) {
        printf("Iteracion %d - Angulo actual: %.2f grados\n", iteracion, angulo_actual);

        if (fabs(calculo_error_minimo(angulo_deseado, angulo_actual)) < 0.01) {
            break;
        }

        tiempo_actual = clock();
        tiempo_transcurrido = (double)(tiempo_actual - tiempo_inicial) / CLOCKS_PER_SEC;

        senal_de_control = pid_control(angulo_deseado, angulo_actual, &integral, &error_previo, tiempo_transcurrido);
        angulo_actual += senal_de_control;

        if (angulo_actual >= 360.0) {
            angulo_actual -= 360.0;
        } else if (angulo_actual < 0.0) {
            angulo_actual += 360.0;
        }

        fprintf(archivo, "%.2lf\n", angulo_actual);
        iteracion++;

        tiempo_inicial = clock(); // Actualiza el tiempo inicial para la próxima iteración
    }

    fclose(archivo);

    printf("La posicion %d (letra %c) ha sido alcanzada con un angulo de %.2f grados.\n", posicion_deseada, letra_deseada, angulo_actual);

    while (1) {
        printf("Ingrese un posible valor de perturbacion entre -100 y 100: ");
        scanf(" %lf", &perturbacion);

        if (perturbacion < -100 || perturbacion > 100) {
            printf("Error: La perturbacion debe ser un valor entre -100 y 100.\n");
            continue;
        }

        angulo_actual += perturbacion;
        angulo_actual = fmod(angulo_actual, 360.0);
        if (angulo_actual < 0.0) {
            angulo_actual += 360.0;
        }

        integral = 0.0;
        error_previo = 0.0;

        printf("Iteracion tras perturbacion - Angulo actual: %.2f grados\n", angulo_actual);

        tiempo_inicial = clock();
        while (fabs(calculo_error_minimo(angulo_deseado, angulo_actual)) >= 0.01) {
            tiempo_actual = clock();
            tiempo_transcurrido = (double)(tiempo_actual - tiempo_inicial) / CLOCKS_PER_SEC;

            senal_de_control = pid_control(angulo_deseado, angulo_actual, &integral, &error_previo, tiempo_transcurrido);
            angulo_actual += senal_de_control;

            if (angulo_actual >= 360.0) {
                angulo_actual -= 360.0;
            } else if (angulo_actual < 0.0) {
                angulo_actual += 360.0;
            }

            fprintf(archivo, "%.2lf\n", angulo_actual);
            tiempo_inicial = clock(); // Actualiza el tiempo inicial para la próxima iteración
        }

        printf("La posicion %d (letra %c) ha sido alcanzada con un angulo de %.2f grados tras la perturbacion.\n", posicion_deseada, letra_deseada, angulo_actual);
    }

    return 0;
}
