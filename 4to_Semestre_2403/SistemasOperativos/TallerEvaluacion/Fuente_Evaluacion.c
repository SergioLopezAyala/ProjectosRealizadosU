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
void llenar_matriz(int SZ){ 
	srand48(time(NULL));
	for(int i = 0; i < SZ*SZ; i++){
		mA[i] = 1.1*i; // Valores de matriz A
		mB[i] = 2.2*i; // Valores de matriz B
		mC[i] = 0;     // Inicializa matriz C en cero
	}	
}

// Función para imprimir matrices si su tamaño es menor a 12
void print_matrix(int sz, double *matriz){
	if(sz < 12){
		for(int i = 0; i < sz*sz; i++){
			if(i%sz==0) printf("\n");
			printf(" %.3f ", matriz[i]);
		}	
	}
	printf("\n>-------------------->\n");
}

// Inicia la cuenta del tiempo
void inicial_tiempo(){
	gettimeofday(&start, NULL);
}

// Finaliza y muestra el tiempo transcurrido
void final_tiempo(){
	gettimeofday(&stop, NULL);
	stop.tv_sec -= start.tv_sec;
	printf("\n:-> %9.0f µs\n", (double) (stop.tv_sec*1000000 + stop.tv_usec));
}

// Función ejecutada por cada hilo para multiplicación de matrices
void *mult_thread(void *variables){
	struct parametros *data = (struct parametros *)variables;
	int idH = data->idH;  // ID del hilo
	int nH  = data->nH;   // Número de hilos
	int N   = data->N;    // Tamaño de la matriz

	int ini = (N/nH)*idH;       // Índice inicial para este hilo
	int fin = (N/nH)*(idH+1);   // Índice final para este hilo

    for (int i = ini; i < fin; i++){
        for (int j = 0; j < N; j++){
			double *pA, *pB, sumaTemp = 0.0;
			pA = mA + (i*N);   // Inicio de fila en A
			pB = mB + j;       // Inicio de columna en B
            for (int k = 0; k < N; k++, pA++, pB+=N){
				sumaTemp += (*pA * *pB);  // Producto y acumulación
			}
			mC[i*N+j] = sumaTemp;  // Almacena el resultado en C
		}
	}

	pthread_mutex_lock (&MM_mutex);   // Bloqueo de mutex
	pthread_mutex_unlock (&MM_mutex); // Desbloqueo de mutex
	pthread_exit(NULL);  // Fin del hilo
}

int main(int argc, char *argv[]){
	if (argc < 2){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		return -1;	
	}
    int SZ = atoi(argv[1]);          // Tamaño de la matriz
    int n_threads = atoi(argv[2]);   // Número de hilos

    pthread_t p[n_threads];          // Arreglo de hilos
    pthread_attr_t atrMM;            // Atributos para los hilos

	mA = MEM_CHUNK;                  // Inicialización de punteros a matrices
	mB = mA + SZ*SZ;
	mC = mB + SZ*SZ;

	llenar_matriz(SZ);               // Llenado de matrices
	print_matrix(SZ, mA);
	print_matrix(SZ, mB);

	inicial_tiempo();                // Inicio de tiempo de ejecución
	pthread_mutex_init(&MM_mutex, NULL);
	pthread_attr_init(&atrMM);
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

    // Creación de hilos para la multiplicación
    for (int j=0; j<n_threads; j++){
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros)); 
		datos->idH = j;
		datos->nH  = n_threads;
		datos->N   = SZ;
        pthread_create(&p[j],&atrMM,mult_thread,(void *)datos);
	}

    // Espera a que todos los hilos terminen
    for (int j=0; j<n_threads; j++)
        pthread_join(p[j],NULL);

	final_tiempo();                  // Fin de la cuenta de tiempo
	
	print_matrix(SZ, mC);            // Imprime la matriz resultado

	pthread_attr_destroy(&atrMM);    // Limpieza de recursos
	pthread_mutex_destroy(&MM_mutex);
	pthread_exit (NULL);
}
