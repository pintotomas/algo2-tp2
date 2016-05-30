#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"



void filtrar_e_imprimir_lineas(FILE *archivo, hash_t *hash){
	ssize_t linea_tam;
	size_t char_tam = 0;
	while(linea_tam != -1){
		char *linea = NULL;
		linea_tam = getline(&linea, &char_tam, archivo);
		if(hash_pertenece(hash, linea)){
			free(linea);
		}else{
			hash_guardar(hash, linea, linea);
			puts(linea);
		}
	}
}


int main(int argc, char **argv){
	if(argc != 2){
		return 1;
	}
	FILE *archivo = fopen(argv[1], "r");
	if(archivo == NULL){
		fprintf(stderr, "No se pudo abrir el archivo");
		return 1;
	}
	hash_t *hash_lineas = hash_crear(NULL);
	filtrar_e_imprimir_lineas(archivo, hash_lineas);
	hash_destruir(hash_lineas);
	return 0;
}
