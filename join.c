#define _POSIX_C_SOURCE 200809L
#define ERROR "Uso:\n\t ./join [archivo-1],[archivo-2],..,[archivo-k]\n"

#include "hash.h"
#include "validaciones.h"
#include "strutil.h"
#include <string.h>
#include <stdlib.h>
 

/* Destruye <cantidad> de hashes almacenados en un puntero,
 * luego, libera a este ultimo.
*/

void liberar_datos(hash_t** datos,size_t cantidad)
{
    for(size_t k = 0; k < cantidad; k++)
    {   
        hash_destruir(datos[k]);
    }
    free(datos);
}

/*
 * Almacena la informacion de <cantidad_archivos> abiertos en un
 * puntero pasado por parametro, en un puntero de hashes.
 * Devuelve true si se pudo almacenar toda la informacion, false
 * en caso contrario.
*/

bool almacenar_archivos(size_t cantidad_archivos, FILE **archivos, hash_t** contenedor_datos)
{
    size_t j = 0;
    size_t k = 1;
    while (k < cantidad_archivos)
    {
        
        char* buffer = NULL;
        size_t capacidad = 0;
        FILE *arch_actual = archivos[k];
        hash_t* hash_nuevo = hash_crear(free);
        
        if (!hash_nuevo)
        {
            liberar_datos(contenedor_datos,k-1);
            return false;
        }
        contenedor_datos[j] = hash_nuevo;
        
        while(getline(&buffer,&capacidad,arch_actual)>=0)
        {
            
	    	//char **clave_valor = linea_a_clave_valor(buffer);
            char** split_linea = split(buffer,' ');
            size_t cantidad_substr = longitud_strv(split_linea);
            char** clave_valor = separar_primer_campo(split_linea,cantidad_substr);
            free_strv(split_linea);
            char* campo = clave_valor[0];
            char* valor = clave_valor[1];
            if (!clave_valor)
            {
                liberar_datos(contenedor_datos,k);
                return false;
            }
            hash_guardar(contenedor_datos[j],campo,valor);
            free(campo);
            free(clave_valor); 
            free(buffer);
            buffer = NULL;
	    	capacidad = 0;
	    }
	    free(buffer);
        k++;
        j++;
	}
    return true;
}

/* Imprime la informacion de los archivos rejuntada, solo si 
 * el campo (lineas de la primer columna de <arch_referencia>)
 * se encuentra en exactamente <cantidad_archivos> (los restantes)
 * como clave en cada diccionario de <contenido_archivos>, luego
 * se imprime el campo y cada valor en contenido_arcvos hasta haber
 * leido todo el archivo <arch_referencia>
*/

void imprimir(FILE* arch_referencia,size_t cantidad_archivos, hash_t** contenido_archivos)
{
    
    char *buffer = NULL;
    size_t capacidad = 0;        
    while(getline(&buffer,&capacidad,arch_referencia)>=0)
        {
            char **split_linea = split(buffer,' ');
            size_t cantidad_substr = longitud_strv(split_linea); 
	    	char **clave_valor = separar_primer_campo(split_linea,cantidad_substr);
            free_strv(split_linea);
            char* campo = clave_valor[0];
            char* valor = clave_valor[1];
            bool clave_pertenece_todos_archivos = true;
            for (size_t i = 0; i < cantidad_archivos; i++)
            {
                if (!hash_pertenece(contenido_archivos[i],campo))
                {                
                    clave_pertenece_todos_archivos = false;
                    break;
                }
            }
            if (clave_pertenece_todos_archivos)
            {
                printf("%s %s ",campo,valor);
                for (size_t i = 0; i < cantidad_archivos; i++)
                {
                    char* valores = hash_borrar(contenido_archivos[i],campo);
                    printf("%s\n",valores);
                    free(valores);
                }
                if (cantidad_archivos == 0)
                {
                    printf("\n");
                }
            }
            free(buffer);
            buffer = NULL;
	    	capacidad = 0; 
            free_strv(clave_valor);
	    }
	free(buffer);
}

int main(int argc, char *argv[]) 
{
    const size_t cantidad_argumentos = argc;
    const size_t cantidad_archivos = cantidad_argumentos-1;
	if (!cantidad_parametros_correctos(cantidad_argumentos,argv,2,ERROR,1))
    {
		return 1;
	}
    hash_t **contenido_archivos = malloc(sizeof(hash_t*)*(cantidad_argumentos-2));
    if (!contenido_archivos)
    {
        return 1;
    }
    FILE **archivos = malloc(sizeof(FILE*)*(cantidad_archivos));  
    if (!archivos)
    {
        free(contenido_archivos);
        return 1;
    }
    bool abrir = abrir_archivos(cantidad_archivos,archivos,argv);
    if (!abrir)
    {
        free(contenido_archivos);
        return 1;
    }    
    bool almaceno = almacenar_archivos(cantidad_archivos,archivos,contenido_archivos);
    if (!almaceno)
    {
        cerrar_archivos(archivos,cantidad_archivos);
        return 1;
    }
    const size_t referencia = 0;
    FILE *arch_referencia = archivos[referencia];
    imprimir(arch_referencia,cantidad_archivos-1,contenido_archivos);
    cerrar_archivos(archivos,cantidad_archivos);
    liberar_datos(contenido_archivos,cantidad_archivos-1);
	return 0;
}
