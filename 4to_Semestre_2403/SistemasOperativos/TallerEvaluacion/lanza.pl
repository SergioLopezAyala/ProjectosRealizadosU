#!/usr/bin/perl
#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: Sergio Lopez
#     Fecha: 31/10/2024
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script de automatización para ejecución por lotes
#     Objetivo: Realizar pruebas automáticas de ejecución del 
#               algoritmo de multiplicación de matrices con 
#               diferentes tamaños de matrices y cantidad de hilos.
#****************************************************************/

$Path = `pwd`;  # Obtiene el directorio actual de trabajo
chomp($Path);

$Nombre_Ejecutable = "MM_ejecutable";  # Nombre del ejecutable a correr
@Size_Matriz = ("200", "300");  # Tamaños de matriz a probar
@Num_Hilos = (1, 2);  # Número de hilos a utilizar
$Repeticiones = 2;  # Número de repeticiones por cada combinación

# Bucle para cada tamaño de matriz
foreach $size (@Size_Matriz) {
	# Bucle para cada número de hilos
	foreach $hilo (@Num_Hilos) {
		$file = "$Path/$Nombre_Ejecutable-" . $size . "-Hilos-" . $hilo . ".dat";  # Archivo de salida para los datos
		# Bucle de repeticiones para la ejecución
		for ($i = 0; $i < $Repeticiones; $i++) {
			# Descomentar la línea de abajo para ejecutar el comando
			# system("$Path/$Nombre_Ejecutable $size $hilo >> $file");
			printf("$Path/$Nombre_Ejecutable $size $hilo \n");  # Imprime el comando que se ejecutaría
		}
		close($file);  # Cierra el archivo
		$p = $p + 1;  # Incrementa el contador de pruebas
	}
}
