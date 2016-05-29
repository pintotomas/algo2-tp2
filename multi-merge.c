#define _POSIX_C_SOURCE 200809L

#include "heap.h"
#include "validaciones.h"
#include <stdlib.h>

#define ERROR "Uso:\n\t ./multi-merge [archivo-1],[archivo-2],...,[archivo-n]\n"

int strcmp_negativo(const void** info1,const void** info2)
{
    const char* s1 = info1[0];
    const char* s2 = info2[0];
    for ( ; *s1 == *s2; s1++, s2++)
	if (*s1 == '\0')
	    return 0;
    return (-((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1));
}

////ESTA FUNCION ES REPETIDA DE JOIN!!! UTILIZAR EN UNA LIBRERIA EN COMUN LUEGO
void cerrar_archivos(FILE** argf, size_t cantidad_archivos)
{   
    size_t k;
    for(k = 0; k < cantidad_archivos; k++)
    {
        fclose(argf[k]);
    }
    free(argf);
}

bool abrir_archivos(size_t cantidad_archivos,FILE * argf[], char* argv[])
{
    for(size_t k = 0; k < cantidad_archivos; k++) 
    {
        FILE* arch_nuevo = fopen(argv[k+1], "r");
        if(arch_nuevo == NULL)
        {
            fprintf(stderr, "No pudo abrirse \"%s\"!\n", argv[k+1]);
            cerrar_archivos(argf,k);
	    	return false;
	    }
        argf[k] = arch_nuevo;
    }
    return true;
}

void imprimir_en_orden(FILE* argf[],size_t cantidad_archivos)
{
    int* archivos_info = malloc(sizeof(int*)*cantidad_archivos);
    const size_t seguir_leyendo = 1;
    const size_t termino_lectura = 0;
    for (size_t i = 0; i < cantidad_archivos; i++)
    {
        archivos_info[i] = seguir_leyendo; //1 si el archivo actual tiene lineas para leer, 0 si no
    }
    heap_t* heap_min = heap_crear(strcmp_negativo);
    size_t actual = 0;
    char* buffer = NULL;
    size_t capacidad = 0;
    size_t arch_restantes = cantidad_archivos;
    const size_t contenedor_linea = 0;
    const size_t contenedor_nro_arch = 1;
    while (arch_restantes > 0)
    {
        if (heap_cantidad(heap_min) == arch_restantes)
        {
            void** info = heap_desencolar(heap_min);
            char* linea = info[contenedor_linea];
            size_t* nro_archivo = info[contenedor_nro_arch];
            printf("%s",linea);
            free(linea);
            free(info);
            actual = *nro_archivo;
            free(nro_archivo);
            continue;
        }
        if (archivos_info[actual] == seguir_leyendo)
        {
            FILE *arch_actual = argf[actual];
            int linea_actual = getline(&buffer,&capacidad,arch_actual);
            if (linea_actual < 0) //getline dice que se termino el archivo
            {
                free(buffer);
                buffer = NULL;
                capacidad = 0;
                archivos_info[actual] = termino_lectura;
                arch_restantes--;
                continue;
            }
            void** info_linea_actual = malloc(sizeof(void*)*2);
            size_t* nro_arch_actual = malloc(sizeof(size_t));
            *nro_arch_actual = actual;
            info_linea_actual[contenedor_linea] = buffer; //Guardo la linea
            info_linea_actual[contenedor_nro_arch] = nro_arch_actual; //Guardo el nro de archivo
            heap_encolar(heap_min,info_linea_actual);
            buffer = NULL;
            capacidad = 0;
        }
        actual++;
    }
    free(archivos_info);
    heap_destruir(heap_min,NULL);      
}

int main(int argc, char *argv[]) 
{
    const size_t cantidad_archivos = argc-1;
	FILE **archivos = malloc((sizeof(FILE*))*cantidad_archivos);
    if (!cantidad_parametros_correctos(argc,argv,2,ERROR,1))
    {
		return 1;
	}
    if (!abrir_archivos(cantidad_archivos,archivos,argv))
    {
        return 1;
    }
    
    imprimir_en_orden(archivos,cantidad_archivos);
    cerrar_archivos(archivos,cantidad_archivos);
	return 0;
}
