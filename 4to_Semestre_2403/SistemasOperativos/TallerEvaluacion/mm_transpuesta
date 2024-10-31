/**************************************************************
		Pontificia Universidad Javeriana
	Autor: Sergio Lopez
	Fecha: 31/10/2024
	Materia: Sistemas Operativos
	Tema: Taller de Evaluación de Rendimiento
	Fichero: fuente de multiplicación de matrices NxN por hilos.
	Objetivo: Evaluar el tiempo de ejecución del 
		      algoritmo clásico de multiplicación de matrices.
			  Se implementa con la Biblioteca POSIX Pthreads
****************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define DATA_SIZE (1024*1024*64*3)  // Tamaño para el bloque de memoria

pthread_mutex_t MM_mutex;  // Mutex para sincronización
static double MEM_CHUNK[DATA_SIZE];  // Memoria compartida
double *mA, *mB, *mC;  // Punteros a matrices A, B y C

struct parametros {  // Estructura para pasar parámetros a cada hilo
	int nH;  // Número total de hilos
	int idH;  // ID del hilo
	int N;    // Tamaño de la matriz
};

struct timeval start, stop;  // Variables para medir el tiempo de ejecución

// Función para llenar matrices A y B y establecer valores iniciales en C
void llenar_matriz(int SZ) { 
	srand48(time(NULL));  // Inicialización de semilla para generación de números pseudoaleatorios
	for(int i = 0; i < SZ*SZ; i++) {
		mA[i] = 1.1 * i;  // Asigna valores a la matriz A
		mB[i] = 2.2 * i;  // Asigna valores a la matriz B
		mC[i] = 0;  // Inicializa la matriz C en cero
	}	
}

// Función para imprimir matrices si el tamaño es pequeño
void print_matrix(int sz, double *matriz) {
	if(sz < 12) {  // Solo imprime si el tamaño es menor a 12
		for(int i = 0; i < sz*sz; i++) {
			if(i % sz == 0) printf("\n");  // Nueva línea para cada fila
			printf(" %.3f ", matriz[i]);
		}	
		printf("\n>-------------------->\n");
	}
}

// Función para iniciar la medición de tiempo
void inicial_tiempo() {
	gettimeofday(&start, NULL);
}

// Función para finalizar la medición de tiempo y mostrar resultado
void final_tiempo() {
	gettimeofday(&stop, NULL);
	stop.tv_sec -= start.tv_sec;  // Calcula el tiempo transcurrido
	printf("\n:-> %9.0f µs\n", (double)(stop.tv_sec * 1000000 + stop.tv_usec));
}

// Función de multiplicación de matrices realizada por cada hilo
void *mult_thread(void *variables) {
	struct parametros *data = (struct parametros *)variables;  // Recibe los parámetros del hilo
	
	int idH = data->idH;  // ID del hilo
	int nH  = data->nH;   // Número total de hilos
	int N   = data->N;    // Tamaño de la matriz
	int ini = (N / nH) * idH;   // Índice de inicio para el hilo
	int fin = (N / nH) * (idH + 1);  // Índice de fin para el hilo

    // Realiza la multiplicación de la matriz en el rango de filas asignadas
    for (int i = ini; i < fin; i++) {
        for (int j = 0; j < N; j++) {
			double *pA = mA + (i * N);  // Puntero a la fila i de A
			double *pB = mB + j;  // Puntero a la columna j de B
			double sumaTemp = 0.0;
            for (int k = 0; k < N; k++, pA++, pB += N) {
				sumaTemp += (*pA * *pB);  // Acumula el producto de los elementos correspondientes
			}
			mC[i * N + j] = sumaTemp;  // Asigna el valor calculado a la posición correspondiente en C
		}
	}

	pthread_mutex_lock(&MM_mutex);  // Bloquea el mutex
	pthread_mutex_unlock(&MM_mutex);  // Desbloquea el mutex
	pthread_exit(NULL);  // Finaliza el hilo
}

// Función principal que inicia el programa
int main(int argc, char *argv[]) {
	if (argc < 2) {  // Verifica la entrada de argumentos
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		return -1;	
	}
    int SZ = atoi(argv[1]);  // Tamaño de la matriz
    int n_threads = atoi(argv[2]);  // Número de hilos

    pthread_t p[n_threads];  // Arreglo para los identificadores de hilos
    pthread_attr_t atrMM;  // Atributos para hilos

	mA = MEM_CHUNK;       // Asigna punteros a las posiciones en la memoria compartida
	mB = mA + SZ * SZ;
	mC = mB + SZ * SZ;

	llenar_matriz(SZ);  // Llena las matrices A y B
	print_matrix(SZ, mA);  // Imprime la matriz A
	print_matrix(SZ, mB);  // Imprime la matriz B

	inicial_tiempo();  // Inicia la medición de tiempo
	pthread_mutex_init(&MM_mutex, NULL);  // Inicializa el mutex
	pthread_attr_init(&atrMM);  // Inicializa los atributos de hilos
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);  // Configura los hilos como joinable

    for (int j = 0; j < n_threads; j++) {
		struct parametros *datos = (struct parametros *)malloc(sizeof(struct parametros));  // Asigna memoria para parámetros del hilo
		datos->idH = j;       // ID del hilo
		datos->nH  = n_threads;  // Número de hilos
		datos->N   = SZ;         // Tamaño de la matriz
        pthread_create(&p[j], &atrMM, mult_thread, (void *)datos);  // Crea el hilo
	}

    for (int j = 0; j < n_threads; j++)
        pthread_join(p[j], NULL);  // Espera a que cada hilo finalice
	final_tiempo();  // Finaliza la medición de tiempo
	
	print_matrix(SZ, mC);  // Imprime la matriz C

	pthread_attr_destroy(&atrMM);  // Destruye los atributos de hilos
	pthread_mutex_destroy(&MM_mutex);  // Destruye el mutex
	pthread_exit(NULL);  // Finaliza el hilo principal
}
