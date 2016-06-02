#include "heapdouble.h"
#include <stdlib.h>
#include <stdbool.h>
#define TAMANO_INICIAL 20
#define DOBLE 2

struct heap
{	double** datos;
	size_t cantidad_elementos;
	size_t capacidad;
	cmp_func_t comparar_prioridad;
};


//Aplica la division y luego la funcion techo
size_t division_techo(size_t dividendo, size_t divisor)
    {return (dividendo + (divisor / 2)) / divisor;}

heap_t *heap_crear(cmp_func_t cmp)
{
	heap_t *heap_nuevo = malloc(sizeof(heap_t));
	double** datos_genericos = malloc(sizeof(double*)*TAMANO_INICIAL);
	if (!heap_nuevo || !datos_genericos)
		{return NULL;}
	heap_nuevo->datos = datos_genericos;
	heap_nuevo->capacidad = TAMANO_INICIAL;
	heap_nuevo->cantidad_elementos = 0;
	heap_nuevo->comparar_prioridad = cmp;
	return heap_nuevo;
}

void heap_destruir(heap_t *heap, void destruir_elemento(double *e))
{	if (destruir_elemento)
	{
		size_t limite = heap->cantidad_elementos;
		for (size_t i = 0; i < limite; i++)
			{destruir_elemento(heap->datos[i]);}
	}
	free(heap->datos);
	free(heap);
}

bool heap_esta_vacio(const heap_t *heap)
	{return !heap->cantidad_elementos;}

size_t heap_cantidad(const heap_t *heap)
	{return heap->cantidad_elementos;}
/*
  Pre: Recibe un puntero a un heap ya creado y tam_nuevo del
	   tipo size_t

  Post: Devuelve true si se pudo redimensionar la pila a tamano
        tam_nuevo, caso contrario, false
*/

bool heap_redimensionar(heap_t* heap, size_t nuevo_tam)
{
 	double** datos_nuevo = realloc(heap->datos,(sizeof(double*))*nuevo_tam);
 	if (!datos_nuevo){
 		return false;
	 	}
	heap->datos = datos_nuevo;
	heap->capacidad = nuevo_tam;
	return true;
}

/*
    Pre: Recibe un puntero a un arreglo de punteros genericos y
    dos posiciones que hacen referencia a dos valores del arreglo
    
    Post: Los valores del arreglo fueron intercambiados de lugar
    entre ellos
*/
void swap_arr(double** arr, size_t pos1, size_t pos2)
{
    double* aux = arr[pos1];
	arr[pos1] =  arr[pos2];
	arr[pos2] = aux;
}

/*
    Pre: Recibe un puntero a un arreglo de punteros genericos y
    dos posiciones que hacen referencia a dos valores del arreglo,
    una funcion de comparacion para comparar los elementos del 
    arreglo, la cantidad de elementos que tiene el arreglo y un
    puntero a una funcion que recibe los mismos parametros (excepto que
    solo recibe la posicion de un valor del arreglo, no dos)
    
    
    Post: Llamara a la funcion swap para los valores pos_actual y 
    pos_reemplazante, luego llamara a la funcion recibida por parametros,
    a partir de la posicion del reemplazante
*/

void intercambiar_y_continuar(double** datos, size_t pos_actual, size_t pos_reemplazante,cmp_func_t comparar_prioridad,size_t cantidad_elementos, void func(double**, cmp_func_t,size_t,size_t))
{
    swap_arr(datos,pos_actual,pos_reemplazante);
    func(datos,comparar_prioridad,cantidad_elementos,pos_reemplazante);
}

/*
    Pre: Recibe un puntero a punteros genericos ya creado, una funcion
    de comparacion, la cantidad de elementos y la posicion actual

    Post: Si el elemento actual es mayor a el elemento de la posicion
    (elemento_actual-2)/2 los intercambia y se vuelve a ejecutar el algoritmo.
*/

void upheap(double** datos,cmp_func_t cmp,size_t cantidad_elementos,size_t elemento_actual)
{
	if (elemento_actual == 0)
		{return;}
	size_t padre_actual = division_techo(elemento_actual-2,2);
	if (cmp(datos[elemento_actual],datos[padre_actual]) > 0)
	    {intercambiar_y_continuar(datos,elemento_actual,padre_actual,cmp,0,upheap);}
}

bool heap_encolar(heap_t *heap, double *elem)
{
	heap->datos[heap->cantidad_elementos] = elem;
	upheap(heap->datos,heap->comparar_prioridad,0,heap->cantidad_elementos);
	heap->cantidad_elementos++;
	if (heap->cantidad_elementos == heap->capacidad)
		{heap_redimensionar(heap,DOBLE*heap->capacidad);}
	return true;
}

double *heap_ver_max(const heap_t *heap)
{
	if (heap_esta_vacio(heap))
		{return NULL;}
	return heap->datos[0];
}

/*
    Pre: Recibe un puntero a punteros genericos ya creado, una funcion
    de comparacion, la cantidad de elementos y la posicion actual

    Post: Si el elemento actual es menor a el elemento de la posicion
    2*(elemento_actual+(1 o 2))/ los intercambia y se vuelve a ejecutar el algoritmo.
*/

void downheap(double **datos,cmp_func_t comparar_prioridad,size_t cantidad_elementos, size_t posicion_actual)
{	size_t pos_h_izquierdo = (2*posicion_actual)+1;
	size_t pos_h_derecho = (2*posicion_actual)+2;
    if (pos_h_izquierdo >= cantidad_elementos) //Arbol izquierdista
       {return;}
	else if (pos_h_derecho >= cantidad_elementos) //Solo un hijo izquierdo
	{
		if (comparar_prioridad(datos[posicion_actual],datos[pos_h_izquierdo]) == -1)
		{
            intercambiar_y_continuar(datos,posicion_actual,pos_h_izquierdo,comparar_prioridad,cantidad_elementos,downheap);
		}
	}
	else //Hay dos hijos
	{
		if ((comparar_prioridad(datos[posicion_actual],datos[pos_h_derecho])) > 0)
			{
			if ((comparar_prioridad(datos[posicion_actual],datos[pos_h_izquierdo])) > 0)
				{return;}
			}
		size_t pos_prioritaria = pos_h_izquierdo;
		if ((comparar_prioridad(datos[pos_h_izquierdo],datos[pos_h_derecho]) < 0))
		    {pos_prioritaria = pos_h_derecho;}
        intercambiar_y_continuar(datos,posicion_actual,pos_prioritaria,comparar_prioridad,cantidad_elementos,downheap);
	}
}

double *heap_desencolar(heap_t *heap)
{
	if (heap_esta_vacio(heap))
		{return NULL;}
	double *primero = heap->datos[0];
	heap->datos[0] = heap->datos[heap->cantidad_elementos-1];
	heap->cantidad_elementos--;
	downheap(heap->datos,heap->comparar_prioridad,heap->cantidad_elementos,0);
	if (heap->capacidad != TAMANO_INICIAL)
	{
		size_t capacidad = heap->capacidad;
		size_t cantidad = heap->cantidad_elementos;
		size_t resultado = division_techo(capacidad,4);
		if (cantidad <= resultado)
			{heap_redimensionar(heap,capacidad/DOBLE);}
	}
	return primero;
}

//Convierte a un arreglo en un heap
void heapify(double* arreglo[],size_t cantidad_elementos,cmp_func_t comparar_prioridad )
{
	int inicio = (int)cantidad_elementos;
	for (int i = inicio-1; i >= 0; i--)
		{downheap(arreglo,comparar_prioridad,cantidad_elementos,i);}
}

heap_t *heap_crear_arr(double *arreglo[], size_t n, cmp_func_t cmp)
{
	heap_t* heap_arr = malloc(sizeof(heap_t));
	double** arreglo_copia = malloc(sizeof(double*)*n);
	if (!heap_arr || !arreglo_copia)
		{return NULL;}
	for (size_t i = 0; i < n; i++)
		{arreglo_copia[i] = arreglo[i];}
	heap_arr->datos = arreglo_copia;
	heap_arr->cantidad_elementos = n;
	heap_arr->capacidad = 2*n;
	heap_arr->comparar_prioridad = cmp;
	heapify(heap_arr->datos,n-1,cmp);
	return heap_arr;
}

void heap_sort(double *elementos[], size_t cant, cmp_func_t cmp)
{

	int ultimo = ((int)cant)-1;
    heapify(elementos,cant,cmp);

	int primero = 0;
	while (ultimo >= 0)
	{
        double* aux = elementos[primero];
		elementos[primero] = elementos[ultimo];
		elementos[ultimo] = aux;
		ultimo--;
		if (ultimo < 0) {break;}
        downheap(elementos,cmp,ultimo+1,primero);
	}
	return;
}
