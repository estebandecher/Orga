#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>


//Estructura de una palabra
typedef struct Palabra {
	
	int dato;
}Palabra;


//Estructura de un bloque
typedef struct Bloque{  

	//Para el FIFO
	int nIngreso;
	
	//para MRU y LRU
	int nUsado;

	Palabra* palabra;  
}Bloque;


//Estructura de un sector
typedef struct Sector{

	Bloque* bloque; 
}Sector;


//Estructura del cache
typedef struct CACHE
{
	int miss;
	int hits;

	int politica;

	int nSectores;
	int nPalabras;
	int nBloques;

	Sector* sector;
}CACHE;





//Declaracion de funciones 
void leerDatos(FILE *stream,CACHE *cache, FILE *streamEscritura);



//Funcion que crea el cache con los datos que se ingresaron
void crearCache(CACHE *cache){

	int s=0;
	int b=0;
	int p=0;

	//Cantidad de sectores
	cache->sector= (Sector*) malloc( sizeof (Sector)* cache->nSectores );

	for (s=0;s<cache->nSectores ;s++){

		//cantidad de bloques
		cache->sector[s].bloque= (Bloque*)malloc( sizeof (Bloque) * cache->nBloques );

		for(b=0;b<cache->nBloques;b++){

			//palabras
			cache->sector[s].bloque[b].palabra = (Palabra*) malloc(sizeof (Palabra)*cache->nPalabras);

			for (p=0;p<cache->nPalabras;p++){

				//asignando valores en las palabras
				cache->sector[s].bloque[b].palabra[p].dato=-1;

			}
			p=0;
		}
		b=0;
	}
	s=0;
}


//Funcion que imprime los datos del cache por pantalla
void imprimirCache(CACHE *cache){

	int s=0;
	int b=0;
	int p=0;

	for (s=0;s<cache->nSectores;s++){

		for (b=0;b<cache->nBloques;b++){

			for (p=0;p<cache->nPalabras;p++){

				printf("\n S%d->B%d->P%d: %d" ,s,b,p,cache->sector[s].bloque[b].palabra[p].dato);
			}
			p=0;
		}
		b=0;
	}
}


//Funcion que imprime los datos de cabecera para un cache en un archivo csv 
void cabeceraCSV(FILE *stream,CACHE *cache){

	int s=0;
	int b=0;
	int p=0;

	for (s=0;s<cache->nSectores;s++){

		for (b=0;b<cache->nBloques;b++){

			for (p=0;p<cache->nPalabras;p++){

				fprintf(stream,"S%d->B%d->P%d;" ,s,b,p);
				
			}
			p=0;
		}
		b=0;
	}

	fprintf(stream,"%s","\n");

}


//funcion que imprime los datos del cache en un archivo csv
void imprimirCacheCSV(FILE *stream,CACHE *cache){

	int s=0;
	int b=0;
	int p=0;

	for (s=0;s<cache->nSectores;s++){

		for (b=0;b<cache->nBloques;b++){

			for (p=0;p<cache->nPalabras;p++){

				if (cache->sector[s].bloque[b].palabra[p].dato != -1){

					fprintf(stream,"%d;",cache->sector[s].bloque[b].palabra[p].dato);
				}
				else {
					fprintf(stream,"%s;"," ");
				}
				
			}
			p=0;
		}
		b=0;
	}

	fprintf(stream,"%s","\n");
}


//Funcion para seleccionar e indicar el bloque en el cual se debe ubicar un dato
//segun las politicas de reemplazo
int selectBloque(CACHE *cache,int sector){

	

	//Si es mapeo directo, se entrega 0 puesto que no hay que eleguir el bloque 
	if (cache->nSectores==cache->nBloques){

		return 0;

	}


	//Depende de la politica de reemplazo 

	int b=0; 

	//FIFO
	if (cache->politica==1){

		int primero=cache->sector[sector].bloque[b].nIngreso;
		int indicePrimero=0;
		while (b<cache->nBloques){

			if (cache->sector[sector].bloque[b].nIngreso<primero){
				primero=cache->sector[sector].bloque[b].nIngreso;
				indicePrimero=b;
			}

			b++;
		}

		return indicePrimero;

	}

	b=0;
	//LRU
	if (cache->politica==2){

		int primeroUsado=cache->sector[sector].bloque[b].nUsado;
		int indicePrimero=0;
		while (b<cache->nBloques){

			if (cache->sector[sector].bloque[b].nIngreso<primeroUsado){
				primeroUsado=cache->sector[sector].bloque[b].nUsado;
				indicePrimero=b;
			}

			b++;
		}
	
		return indicePrimero;
	}

	b=0;
	//MRU
	if(cache->politica==3){

		int ultimoUsado=cache->sector[sector].bloque[b].nUsado;
		int indiceUltimo=0;
		while (b<cache->nBloques){

			if (cache->sector[sector].bloque[b].nUsado>ultimoUsado){
				ultimoUsado=cache->sector[sector].bloque[b].nUsado;
				indiceUltimo=b;
			}

			b++;
		}
	
		return indiceUltimo;

	}
}


//Funcion que inserta un valor en el cache
void insertarValor(int valorInsertar, CACHE *cache, int nIngreso){


	//indice de sector en que se debe colocar 
	int indice;
	indice = valorInsertar/cache->nPalabras;
	indice = indice%cache->nSectores;

	int primerValor=(valorInsertar/cache->nPalabras)*cache->nPalabras;
	int ultimoValor=primerValor+cache->nPalabras-1;


	int b=0;
	//bool de ingreso
	int insertado=0;
	int p=0;
	while(b<cache->nBloques && insertado != 1){

		//La posicion tiene basura
		//Se asigna el valor y es un miss
		if (cache->sector[indice].bloque[b].palabra[p].dato == -1){

			p=0;
			while (p<cache->nPalabras){
				cache->sector[indice].bloque[b].palabra[p].dato =primerValor;
				primerValor++;
				p++;
				

			}

			cache->sector[indice].bloque[b].nIngreso=nIngreso;
			cache->sector[indice].bloque[b].nUsado=nIngreso;
			cache->miss=cache->miss+1;
			//printf("\nM");
			insertado=1;
			
		}
		//la posicion esta acupada por el mismo valor
		//la pos no se modifica y es un hit 
	 	else if(cache->sector[indice].bloque[b].palabra[p].dato == primerValor){

	 		cache->sector[indice].bloque[b].nUsado=nIngreso;
	 		cache->hits=cache->hits+1;
	 		insertado=1;
	 		//printf("\nH");
		}



		//Se pasa a ver el siguiente bloque 
		b++;

	}

	//Si ya se recorrieron todos los bloques 
	//la posicion esta ocupada con un dato
	//es un miss y se debe aplicar politica de reemplazo
	if (insertado!=1){

		int indiceBloque =selectBloque(cache,indice);
		p=0;
		while (p<cache->nPalabras){

			cache->sector[indice].bloque[indiceBloque].palabra[p].dato =primerValor;
			primerValor++;
			p++;				

		}

		cache->sector[indice].bloque[indiceBloque].nIngreso=nIngreso;
		cache->sector[indice].bloque[indiceBloque].nUsado=nIngreso;
		cache->miss=cache->miss+1;
		insertado=1;
		//printf("\nM");
	}
}



//Funcion para leer los datos que se deben insertar en el cache
void leerDatos(FILE *stream,CACHE *cache, FILE *streamEscritura){


	int finArchivo;
	finArchivo=feof(stream);
	char valorString[5];
	int valor;

	int i=1;

	while (finArchivo == 0){

		//leyendo el valor a insertar 
		fscanf(stream,"%s",valorString);
		valor=atoi(valorString);
		//printf("\n --------------------------- ");
		//printf("\n valor: %d", valor);


		//insertar el valor 
		insertarValor(valor,cache,i);
		//imprimirCache(cache);
		imprimirCacheCSV(streamEscritura,cache);
		//printf("\n --------------------------- ");


		i++;
		finArchivo=feof(stream);
		
	}
}

//Funcion que escribe en un archivo de salida el estado del cache actual
void crearSalidaCache(FILE *stream,CACHE *cache){

	int s=0;
	int b=0;
	int p=0;

	for (s=0;s<cache->nSectores;s++){

		for (b=0;b<cache->nBloques;b++){

			for (p=0;p<cache->nPalabras;p++){

				if(cache->sector[s].bloque[b].palabra[p].dato!=-1){
					fprintf(stream,"S%d->B%d->P%d: %d\n" ,s,b,p,cache->sector[s].bloque[b].palabra[p].dato);
				}
				else{
					fprintf(stream,"S%d->B%d->P%d: %s\n" ,s,b,p," ");	
				}

				
			}
			p=0;
		}
		b=0;
	}
}


//Funcion que escribe en un archivo de salida los porcentajes de hits y de miss
void crearSalidaTasa(FILE *stream,CACHE *cache){

	double nTotal=cache->hits+cache->miss;
	double tasaHit=(cache->hits/nTotal)*100.0;
	double tasaMiss=(cache->miss/nTotal)*100.0;
	fprintf(stream,"Tasa de hits: %f",tasaHit);
	fprintf(stream,"\nTasa de miss: %f",tasaMiss);

}

//Funcion oara validar los datos de entrada desde la consola
//Todos menos politica 
int validarDatosEntrada(int nVias, int nPalabras, int nBloques){

	int malas=0;
	//nVias
	if (nVias < 1 || (nVias%2 != 0 && nVias>1)) {
		printf("\nCantidad de vias invalidas (negativas o impar)");
		malas++;
	}
	//nPalabras
	if (nPalabras < 1 || (nPalabras%2 != 0 && nPalabras>1)){
		printf("\nCantidad de palabras invalidas (negativas o impar)");
		malas++;
	}
	//n
	if (nBloques < 1 || (nBloques%2 != 0 && nBloques>1)){
		printf("\nCantidad de bloques invalidas (negativas o impar)");
		malas++;
	}
	return malas;

}




int main (int argc ,char* argv[]){



	//***************************************VALORES DE ENTRADA**************************************************
	printf("\nLa politica ingresada es %s",argv[1] );
	printf("\nVias ingresadas: %s",argv[2] );
	printf("\nPalabras ingresadas: %s",argv[3] );
	printf("\nBloques ingresados: %s",argv[4] );

	//Se pasan los guardan los datos ingresados
	int nVias=atoi(argv[2]);
	int nPalabras=atoi(argv[3]);
	int nBloques=atoi(argv[4]);
	char* politica=argv[1]; 
	//****************************************************************************************************+



	//*******************************************ASIGNANDO VALORES***************************************************

	//Sectores => 
	//Bloques / vias 
	int nSectores=nBloques/nVias;

	//Bloques por Sectores
	//tantos como vias sean 

	//Palabras: 
	//tantas palabras como se indique en cada bloque

	//se pide memoria para el cache
	CACHE* cache = (CACHE*) malloc( sizeof(CACHE) );
	//estableciendo valores
	cache->nSectores=nSectores;
	cache->nBloques=nVias;
	cache->nPalabras=nPalabras;
	cache->hits=0;
	cache->miss=0;

	//***************************************************************************************************************


	//****************************************VALIDANDO VALORES INGRESADOS***********************************************

	//setear la politica de reeplazo
	int fifo=strcmp(politica,"FIFO");
	int lru=strcmp(politica,"LRU");
	int mru=strcmp(politica,"MRU");

	if (fifo==0){
		//printf("\nPolitica es FIFO");
		cache->politica=1;

	}
	if (lru==0){
		//printf("\nPolitica es LRU");
		cache->politica=2;
	}

	if (mru==0){
		//printf("\nPolitica es MRU");
		cache->politica=3;
	}
	else if (fifo!=0 && lru!=0 && mru!=0){
		printf("\nPolitica ingresada invalida");
		//Termina el programa
		return 0;
	}
	//se valida si los datos relacionados al tamaño del cache son validos o no
	if (validarDatosEntrada(nVias, nPalabras,nBloques) != 0){
		return 0;
	}

	//*************************************************************************************************




	//crear
	//llenar cache con -1
	crearCache(cache);


	//**********************************ARCHIVO DE LECTURA******************************************************
	
	//Archivos de lectura
	//datos a ingresar el cache  
	FILE *archivoDatos;	
	//archivoDatos = fopen("input2.txt","r");
	char nombreDatos[20];
	printf("\n\nIngrese el nombre del archivo de entrada con el estado final del cache(con extension): \n" );
	printf("Nombre: : " );
	scanf("%s", nombreDatos);
	archivoDatos = fopen(nombreDatos,"r");
	//si el archivo no se encuentra 
	if (archivoDatos == NULL){
		printf("\nEl archivo de entrada no se encontro");
		return 0;
	}

	//**************************************************************************************************


	//**************************************ARCHIVOS DE SALIDA****************************************
	//Tasa de miss y Tasa de hit
	//Archivos de escritura 
	FILE *archivoTasa;	
	//archivoTasa = fopen("tasas.txt","w");
	printf("\nIngrese el nombre del archivo de salida con las tasas de Hit y Miss(con extension): \n" );
	printf("Nombre: : " );
	char nombreTasas[20];
	scanf("%s", nombreTasas);
	archivoTasa = fopen(nombreTasas,"w");
	




	//Estado final del Cache
	//Archivos de escritura
	FILE *archivoCache;	
	//archivoTasa = fopen("tasas.txt","w");
	printf("\nIngrese el nombre del archivo de salida con el estado final del cache(con extension): \n" );
	printf("Nombre: : " );
	char nombreCache[20];
	scanf("%s", nombreCache);
	archivoCache = fopen(nombreCache,"w");

	
	
	//***************************************************************************************************

	//Archivos de escritura 
	FILE *trazaCache;
	trazaCache = fopen("cacheCSV.csv","w");


	//se escribe la cabecera del archivo csv
	cabeceraCSV(trazaCache,cache);

	//******************************************PROCESAMIENTOS**************************************************
	//PRINCIPAL
	//funcion que llena con los datos que se van leyendo 
	leerDatos(archivoDatos,cache,trazaCache);

	//SALIDAS
	//Se crea la salida con las tasas del cache
	fprintf(archivoTasa, "Tasas de Cache: \n");
	fprintf(archivoTasa, "Vias: %d \n",nVias);
	fprintf(archivoTasa, "Bloques: %d \n",nBloques);
	fprintf(archivoTasa, "Palabras por bloque: %d \n",nPalabras);
	fprintf(archivoTasa, "Politica: %s\n\n",politica);

	//fprintf(archivoTasa, "Cache de %d Set, %d Bloques y %d Palabras, con politica de remmplazo %s\n",nSectores,nBloques,nPalabras,politica);
	crearSalidaTasa(archivoTasa,cache);

	//se crea la salida con el estado final del cache
	fprintf(archivoCache, "Estado final del cache: \n");
	fprintf(archivoCache, "Vias: %d \n",nVias);
	fprintf(archivoCache, "Bloques: %d \n",nBloques);
	fprintf(archivoCache, "Palabras por bloque: %d \n",nPalabras);
	fprintf(archivoCache, "Politica: %s\n\n",politica);
	//fprintf(archivoCache, "Cache de %d Set, %d Bloques y %d Palabras, con politica de remmplazo %s\n",nSectores,nBloques,nPalabras,politica);
	crearSalidaCache(archivoCache,cache);




	//*****************************************APOYO**********************************************
	//imprimir cache (por pantalla)
	printf("\nEstado final del cache: \n");
	imprimirCache(cache);
	printf("\n\n");

	//******************************************************************************************

	printf("\nSe han creado los archivos exitosamente");
	

	//destruir cache


	//cerrar archivos
	fclose(trazaCache);
	fclose(archivoDatos);
	fclose(archivoTasa);
	fclose(archivoCache);

	return 0;
} 