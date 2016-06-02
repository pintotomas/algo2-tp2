#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "heapdouble.h"


typedef struct mediana{
	heap_t* heap_max;
	heap_t* heap_min;
	size_t tam_heap_max;
	size_t tam_heap_min;
}mediana_t;


//Funcion de comparacion.
int cmp(const double *n1, const double *n2){
	if(n1 > n2){
		return 1;
	}else if(n1 < n2){
		return -1;
	}
	return 0;
}


//Funcion de comparacion invertida.
int cmp_min(const double *n1, const double *n2){ 
	if(n1 > n2){
		return -1;
	}else if(n1 < n2){
		return 1;
	}
	return 0;
}



//inicializa el TDA en O(1).
mediana_t* calculador_mediana_crear(){
	mediana_t* calculador_mediana = malloc(sizeof(mediana_t));
	if(calculador_mediana == NULL){
		return NULL;
	}
	calculador_mediana->heap_max = heap_crear(cmp);
	calculador_mediana->heap_min = heap_crear(cmp_min);
	calculador_mediana->tam_heap_max = 0;
	calculador_mediana->tam_heap_min = 0;
	return calculador_mediana;
}



//agrega un valor a la estructura en O(log n).
//Devuelve true cuando se agrega el valor exitosamente y false en caso contrario.
bool calculador_mediana_agregar_valor(mediana_t* mediana, double valor){
	if(mediana->tam_heap_max == 0){
		return heap_encolar(mediana->heap_max, &valor);
		mediana->tam_heap_max++;
	}else{
		if(mediana->heap_max->comparar_prioridad(heap_ver_max(mediana->heap_max), &valor) > 0){
			return heap_encolar(mediana->heap_max, &valor);
			mediana->tam_heap_max++;
		}
		return heap_encolar(mediana->heap_min, &valor);
		mediana->tam_heap_min++;
	}
}


//Balancea los heaps para que la diferencia de tamanos entre ellos no sea mayor a 1.
void heaps_balancear(mediana_t* mediana){
	size_t diferencia = mediana->tam_heap_max - mediana->tam_heap_min;
	while((diferencia < -1) || (diferencia > 1)){
		if(mediana->tam_heap_max > mediana->tam_heap_min){
			heap_encolar(mediana->heap_min, heap_desencolar(mediana->heap_max));
			mediana->tam_heap_min++, mediana->tam_heap_max--;
		}else{
			heap_encolar(mediana->heap_max, heap_desencolar(mediana->heap_min));
			mediana->tam_heap_max++, mediana->tam_heap_min--;
		}
		diferencia = mediana->tam_heap_max - mediana->tam_heap_min;
	}
}



//devolverá la mediana de acuerdo a los elementos que le fueron ingresados en O(1).
double calculador_mediana_obtener_mediana(mediana_t* mediana){
	double mediana_valor;
	heaps_balancear(mediana);
	size_t diferencia = mediana->tam_heap_max - mediana->tam_heap_min;
	if(diferencia == 0){
		mediana_valor = (*heap_ver_max(mediana->heap_max) + *heap_ver_max(mediana->heap_min))/2;
	}else{
		if(mediana->tam_heap_max > mediana->tam_heap_min){
			mediana_valor = *heap_ver_max(mediana->heap_max);
		}else{
			mediana_valor = *heap_ver_max(mediana->heap_min);
		}
	}
	return mediana_valor;
}



//que destruirá la estructura en O(n).
void calculador_mediana_destruir(mediana_t* mediana){
	heap_destruir(mediana->heap_max, NULL);
	heap_destruir(mediana->heap_min, NULL);
	free(mediana);
}
