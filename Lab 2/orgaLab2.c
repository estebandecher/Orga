#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>





//Funciones
void pedirNombreCodigo(char *nombre);
void pedirNombreSenales(char *nombre);
void pedirNombreSalida(char *nombre);
int leerCodigo(FILE *stream);
int verificarGrupo(char *operacion);
int hacerOperacionR(char *operacion,int rs,int rt);
int hacerOperacionI(char *operacion,int rs,int rt);
int buscarLabelCodigo(char *label, int largo);
int encontarInstruccion(FILE *stream, char *instruccionCompleta );
void encontarLinea(FILE *streamLineasControl,int nInstruccion);
void compararLineasControl(FILE *streamEscritura,char *operacion,FILE *streamCodigo);
//void trazaCodigo(FILE *streamCodigo,FILE  *streamEscribir, int largo,IFID bufferrIFID);
void imprimirRegistros(FILE *stream);
int obtenerIndice(char *registro);


//Arreglos 
char intrucR[10][10]={"add","sub","mul","div","addi","subi"};
char intrucI[10][10]={"beq","blt","bgt"};
char intrucJ[10][10]={"j","jal","jr"};
char intrucM[10][10]={"lw","sw","la"};

int valoresRegistro[32]={0};
char indicesRegistro[34][8]={"$zero" ,"$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};
char arrCodigo[400][16]={'\0'};
//Estructura que simula el stack pointer 
int stackPointer[400];


// Valores de las señales de control
// ||regDest||Jump||Branch||MemRead||MemToReg||ALUOP||MemWrite||ALUsrc||RegWrite||
char lineasAdd[20][4]={"1","0","0","0","0","10","0","0","1"};
char lineasAddi[20][4]={"0","0","0","0","0","00","0","1","1"};
char lineasSubi[20][4]={"0","0","0","0","0","01","0","1","1"};
char lineasLw[20][4]={"0","0","0","1","1","00","0","1","1"};
char lineasSw[20][4]={"X","0","0","0","X","00","1","1","0"};
char lineasBeq[20][4]={"X","0","1","0","X","01","0","0","0"};
//				 						"op"
char lineasJ[20][4]={"X","1","0","0","X","X","0","X","0"};

//Arreglos para guardar info de traza
int arrTrazaCodigo[100];
char arrHazardC[100][4];
char arrHazardD[100][4];

//Estructuras para para los buffer 





typedef struct  IFID
{
	char instruccion[40];
	char rd[8];
	char rs[8];
	char rt[8];
	char* label;
	int singExtended;
	int indiceCode;

	char*regDest;
	char*Jump;
	char*Branch;
	char*MemRead;
	char*MemToRead;
	char*ALUOP;
	char*MemWrite;
	char*ALUsrc;
	char*RegWrite;
	


	int PC;

	
}IFID;


typedef struct  IDEX
{
	char instruccion[40];
	char rd[8];
	char rs[8];
	char rt[8];
	char* label;
	int singExtended;
	int indiceCode;

	char*regDest;
	char*Jump;
	char*Branch;
	char*MemRead;
	char*MemToRead;
	char*ALUOP;
	char*MemWrite;
	char*ALUsrc;
	char*RegWrite;


	int PC;
	
}IDEX;


typedef struct  EXMEM
{
	char instruccion[40];
	char rd[8];
	char rs[8];
	char rt[8];
	char* label;
	int singExtended;
	int indiceCode;

	char*regDest;
	char*Jump;
	char*Branch;
	char*MemRead;
	char*MemToRead;
	char*ALUOP;
	char*MemWrite;
	char*ALUsrc;
	char*RegWrite;


	int PC;
	
}EXMEM;


typedef struct  MEMWB
{
	char instruccion[40];
	char rd[8];
	char rs[8];  
	char rt[8];
	char* label;
	int singExtended;
	int indiceCode;

	char*regDest;
	char*Jump;
	char*Branch;
	char*MemRead;
	char*MemToRead;
	char*ALUOP;
	char*MemWrite;
	char*ALUsrc;
	char*RegWrite;

	int PC;
	
}MEMWB;


typedef struct Buffers{
	struct IFID *bufferIFID;
	struct IDEX *bufferIDEX;
	struct EXMEM *bufferEXMEM;
	struct MEMWB *bufferMEMWB; 

}Buffers;









//Definicion de funciones 


//Funcion que solicita el nombre del archivo de entrada correspondiente al codigo en mips
void pedirNombreCodigo(char *nombre){
	
	printf("\nIngrese el nombre correspondiente a su archivo de lectura (Codigo en Mips) \n");
	printf("Nombre: ");
	
	
	scanf("%s",nombre);
	//printf("\n El nombre del archivo es: %s ", nombre);
	

}



//Funcion que pide el nombre del archivo de lectura correspondiente a los valores de las señales de control 
void pedirNombreSenales(char *nombre){
	
	printf("\nIngrese el nombre correspondiente a su archivo de lectura (Valores de senales de control) \n");
	printf("Nombre: ");
	
	
	scanf("%s",nombre);
	//printf("\n El nombre del archivo es: %s ", nombre);
	

}

 
//Se pide el nombre del archivo en donde se escribira
void pedirNombreSalida(char *nombre){
	
	//char texto[20];

		
	printf("\nIngrese el nombre de su archivo de salida \n");
	printf("Nombre: ");
	scanf("%s",nombre);
	//nombre=texto;
	
	
}







//Funcion que lee todo el codigo de mips y almacena cada cadena de caracteres en una posicion en el arreglo arrCodigo
//Retorna el largo del arreglo arrCodigo
int leerCodigo(FILE *stream){

	int finArchivo;
	finArchivo=feof(stream);

	int indice=0;

	while (finArchivo == 0){

		fscanf(stream,"%s",arrCodigo[indice]);

		finArchivo=feof(stream);
		indice++;

	}

	return indice;
	//arrCodigo[indice] = "fin";



}








//Funcion que retorna un indice segun el tipo de instruccion 
int verificarGrupo(char *operacion){
	
	
	int i;
	int dif;
	int grupo;
	
	
	
	//Buscar en grupo 1 (R)
	for(i=0;i<6;i++){
		dif=-1;
		dif=strcmp(operacion,intrucR[i]);
		
		if (dif==0){
			return 1;
		}
		
	}
	
	//Buscar en grupo 2 (I)
	for(i=0;i<3;i++){
		dif=-1;
		dif=strcmp(operacion,intrucI[i]);
		if (dif==0){
			return 2;
		}
		
	}
	
	
	//Buscar em grupo 3 (J)
	for(i=0;i<3;i++){
		dif=-1;
		dif=strcmp(operacion,intrucJ[i]);
		if (dif==0){
			return 3;
		}
		
	}

	//Buscar em grupo 4 (M)
	for(i=0;i<3;i++){
		dif=-1;
		dif=strcmp(operacion,intrucM[i]);
		if (dif==0){
			return 4;
		}
		
	}



	
	
	return -1;
	
}



//Funcion que realiza las operacioones de tipo R entre los registros 
//retorna el resultado de la operacion matematica
int hacerOperacionR(char *operacion,int rs,int rt){

	int rd;
	int add;
	int sub;
	int mul;
	int div;
	int addi;
	int subi;

	//printf("\n La operacion es: %s",operacion);

	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);


	if (add == 0 || addi == 0){
		//printf("\nSUMA");
		rd=rs+rt;
		return rd;
	}
	else if (sub == 0 || subi == 0){
		//printf("\nRESTA");
		rd=rs-rt;
		return rd;

	}
	else if (mul == 0){

		rd=rs*rt;
		//printf("\nMULTIPLICACION ");
		return rd;
	}
	else if (div == 0){
		//printf("\nDIVISION");
		rd=rs/rt;
		//rd=floor(rd);
		return rd;


	}


}




//Funcion que realiza la compatracion entre los registro, para la instruccion beq
//retorna 1 = cumple condicion 
// retorna 0 == no cumple con la condicion 
int hacerOperacionI(char *operacion,int rs,int rt){

	int beq;
	int blt;
	int bgt;

	
	beq=strcmp(intrucI[0],operacion);
	blt=strcmp(intrucI[1],operacion);
	bgt=strcmp(intrucI[2],operacion);



	if (beq==0){
		if(rs==rt){
			return 1;
		}
	}

	else if (blt==0){
		if(rs < rt){
			return 1;
		}

	}
	else if (bgt==0){
		if(rs>rt){
			return 1;
		}
	}

	return 0;

}



//Devulve el indice en el cual se debe ubicar correspondiente a la ubicacion del label 
int buscarLabelCodigo(char *label, int largo){

	int indice=0;
	int dif;

	while (indice<largo){

		dif=strcmp(arrCodigo[indice],label);
		if (dif == 0){

			return indice;
		}

		indice++;

	}
	return -1;
}






//Funcion para encontrar una instruccion en el codigo
int encontarInstruccion(FILE *stream, char *instruccionCompleta ){

	char *instruccionLeida= malloc( sizeof(char) * (40) );
	rewind(stream);

	int nInstruccion=1;

	int finArchivo;
	finArchivo=feof(stream);

	int comparacion;
	int comparacionLabel;

	int largoPalabra;

	int difSalto;

	while (finArchivo == 0){

		

		fgets(instruccionLeida,40,stream);
		largoPalabra=strlen(instruccionLeida);
		difSalto=strcmp(instruccionLeida,"\n");


		if (difSalto != 0){

			
			instruccionLeida[largoPalabra-1]='\0';

			//printf("\n [%s]",instruccionLeida);


			comparacion=strcmp(instruccionLeida,instruccionCompleta);
			//comparacionLabel=strcmp(instruccionLeida[largoPalabra-2],":");

			if (comparacion==0){

				printf("\nFuncion encontrada [%d]", nInstruccion);
				return nInstruccion;
			}

			if (instruccionLeida[largoPalabra-2] != ':'){

				nInstruccion++;
				finArchivo=feof(stream);

			}

		}
		else {

			//printf("\n SALTO DE LINEA" );

		}

		

		finArchivo=feof(stream);


	}

	free(instruccionLeida);
	return -1;

}


//Funcion para ubicarse en la linea de control indicada segun el numero de instruccion 
void encontarLinea(FILE *streamLineasControl,int nInstruccion){

	char *instruccionLeida= malloc( sizeof(char) * (100) );
	rewind(streamLineasControl);

	int i=1;

	int finArchivo;
	finArchivo=feof(streamLineasControl);

	int comparacion;
	fgets(instruccionLeida,100,streamLineasControl);


	while (finArchivo == 0){

		
		if (nInstruccion == i){

			//Ubicados en la 
			//Leer o guardar en un arreglo 
			free(instruccionLeida);
			//printf("\nEncontrado");
			return;

		}

	
		fgets(instruccionLeida,20,streamLineasControl);


		i++;
		finArchivo=feof(streamLineasControl);
	}

	free(instruccionLeida);


}




//Funcion que compara todas las lineas de control del archivo, con los valores teoricos y reales 
//Escribe en el archivo de salida el resultado de dicha comparacion
//ELIMINAR
void compararLineasControl(FILE *streamEscritura,char *operacion,FILE *streamCodigo){

	int add;
	int sub;
	int mul;
	int div;
	int addi;
	int subi;

	int branch;
	int jump;
	int lw;
	int sw;

	int difSalto;

	difSalto=strcmp(operacion,"\n");

	if (difSalto == 0){
		printf("MAL");
	}

	//printf("\n La operacion es: %s",operacion);

	fprintf(streamEscritura, "\t%s", "   ");

	//Tipo R
	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);
	//Branch
	branch=strcmp(intrucI[0],operacion);
	//Jump
	jump=strcmp(intrucJ[0],operacion);
	//Lw
	lw=strcmp(intrucM[0],operacion);
	//Sw
	sw=strcmp(intrucM[1],operacion);

	char *senal = malloc( sizeof(char) * (10) ); 

	int i=0;
	int dif=-1;
	int difX=-1;

	if (add == 0 || sub == 0 || mul==0 || div == 0){

		//printf("\nADD o SUB o MUL o DIV");

		int estado=0;

		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasAdd[i],"X");	
			dif=strcmp(lineasAdd[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}


		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}

		return;
	}
	else if (addi == 0 ){

		//printf("\nADDI ");
		int estado=0;
		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasAddi[i],"X");	
			dif=strcmp(lineasAddi[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;

	}
	else if (subi == 0){

		//printf("\n SUBI");
		int estado=0;
		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasSubi[i],"X");	
			dif=strcmp(lineasSubi[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;

	}
	else if (branch == 0){

		//printf("\nBRANCH");
		int estado=0;
		while(i<9){

			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasBeq[i],"X");	
			dif=strcmp(lineasBeq[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;
	}
	else if (jump == 0){

		//printf("\nJUMP");
		int estado=0;
		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasJ[i],"X");	
			dif=strcmp(lineasJ[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;

	}
	else if (lw == 0 ){

		//printf("\nLW");
		int estado=0;
		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasLw[i],"X");	
			dif=strcmp(lineasLw[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;
	}
	else if (sw == 0 ){

		//printf("\nSW");
		int estado=0;
		while(i<9){
			fscanf(streamCodigo,"%s",senal);
			difX=strcmp(lineasSw[i],"X");	
			dif=strcmp(lineasSw[i],senal);
			if (difX == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else if(dif == 0){
				fprintf(streamEscritura, "%s", "-");fprintf(streamEscritura, "%s\t", "");
			}
			else {
				fprintf(streamEscritura, "%s",senal);fprintf(streamEscritura, "%s\t", "");
				estado=estado +1;
			}
			i++;
		}
		
		fprintf(streamEscritura, "%s", "");

		if (estado == 0 ){

			fprintf(streamEscritura, "%s\n","Correcto");

		}
		else {
			fprintf(streamEscritura, "%s\n","Error");	
		}
		return;

	}


}



//Funcion para asignar los valores de las señales de control en el buffer IFID
void asignarLineasControl(Buffers *buffers, char* operacion){

	int add=-1;
	int sub=-1;
	int mul=-1;
	int div=-1;
	int addi=-1;
	int subi=-1;
	int branch=-1;
	int jump=-1;
	int lw=-1;
	int sw=-1;


	//Tipo R
	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);
	//Branch
	branch=strcmp(intrucI[0],operacion);
	//Jump
	jump=strcmp(intrucJ[0],operacion);
	//Lw
	lw=strcmp(intrucM[0],operacion);
	//Sw
	sw=strcmp(intrucM[1],operacion);



	if(add==0 || sub==0 || mul==0 || div==0){


		buffers->bufferIFID->regDest = lineasAdd[0];
		buffers->bufferIFID->Jump = lineasAdd[1];
		buffers->bufferIFID->Branch = lineasAdd[2];
		buffers->bufferIFID->MemRead = lineasAdd[3];
		buffers->bufferIFID->MemToRead =lineasAdd[4];
		buffers->bufferIFID->ALUOP = lineasAdd[5];
		buffers->bufferIFID->MemWrite = lineasAdd[6];
		buffers->bufferIFID->ALUsrc = lineasAdd[7];
		buffers->bufferIFID->RegWrite = lineasAdd[8]; 	

	}


	else if(addi==0){

		buffers->bufferIFID->regDest = lineasAddi[0];
		buffers->bufferIFID->Jump = lineasAddi[1];
		buffers->bufferIFID->Branch = lineasAddi[2];
		buffers->bufferIFID->MemRead = lineasAddi[3];
		buffers->bufferIFID->MemToRead =lineasAddi[4];
		buffers->bufferIFID->ALUOP = lineasAddi[5];
		buffers->bufferIFID->MemWrite = lineasAddi[6];
		buffers->bufferIFID->ALUsrc = lineasAddi[7];
		buffers->bufferIFID->RegWrite = lineasAddi[8]; 	

	}
	else if(subi==0){

		buffers->bufferIFID->regDest = lineasSubi[0];
		buffers->bufferIFID->Jump = lineasSubi[1];
		buffers->bufferIFID->Branch = lineasSubi[2];
		buffers->bufferIFID->MemRead = lineasSubi[3];
		buffers->bufferIFID->MemToRead =lineasSubi[4];
		buffers->bufferIFID->ALUOP = lineasSubi[5];
		buffers->bufferIFID->MemWrite = lineasSubi[6];
		buffers->bufferIFID->ALUsrc = lineasSubi[7];
		buffers->bufferIFID->RegWrite = lineasSubi[8]; 

	}
	else if(branch==0){


		buffers->bufferIFID->regDest = lineasBeq[0];
		buffers->bufferIFID->Jump = lineasBeq[1];
		buffers->bufferIFID->Branch = lineasBeq[2];
		buffers->bufferIFID->MemRead = lineasBeq[3];
		buffers->bufferIFID->MemToRead =lineasBeq[4];
		buffers->bufferIFID->ALUOP = lineasBeq[5];
		buffers->bufferIFID->MemWrite = lineasBeq[6];
		buffers->bufferIFID->ALUsrc = lineasBeq[7];
		buffers->bufferIFID->RegWrite = lineasBeq[8]; 

	}
	else if(jump==0){

		buffers->bufferIFID->regDest = lineasJ[0];
		buffers->bufferIFID->Jump = lineasJ[1];
		buffers->bufferIFID->Branch = lineasJ[2];
		buffers->bufferIFID->MemRead = lineasJ[3];
		buffers->bufferIFID->MemToRead =lineasJ[4];
		buffers->bufferIFID->ALUOP = lineasJ[5];
		buffers->bufferIFID->MemWrite = lineasJ[6];
		buffers->bufferIFID->ALUsrc = lineasJ[7];
		buffers->bufferIFID->RegWrite = lineasJ[8]; 

	}
	else if(lw==0){

		buffers->bufferIFID->regDest = lineasLw[0];
		buffers->bufferIFID->Jump = lineasLw[1];
		buffers->bufferIFID->Branch = lineasLw[2];
		buffers->bufferIFID->MemRead = lineasLw[3];
		buffers->bufferIFID->MemToRead =lineasLw[4];
		buffers->bufferIFID->ALUOP = lineasLw[5];
		buffers->bufferIFID->MemWrite = lineasLw[6];
		buffers->bufferIFID->ALUsrc = lineasLw[7];
		buffers->bufferIFID->RegWrite = lineasLw[8]; 

	}
	else if(sw==0){

		buffers->bufferIFID->regDest = lineasSw[0];
		buffers->bufferIFID->Jump = lineasSw[1];
		buffers->bufferIFID->Branch = lineasSw[2];
		buffers->bufferIFID->MemRead = lineasSw[3];
		buffers->bufferIFID->MemToRead =lineasSw[4];
		buffers->bufferIFID->ALUOP = lineasSw[5];
		buffers->bufferIFID->MemWrite = lineasSw[6];
		buffers->bufferIFID->ALUsrc = lineasSw[7];
		buffers->bufferIFID->RegWrite = lineasSw[8]; 

	}


}



//Funcion que cuenta la cantidad de instrucciones ejecutadas, incluyendo los nop
int contarInstrucciones(){
	int i=0;

	while (i<100){

		if (arrTrazaCodigo[i]==-2){
			return i;

		}

		i++;
	}


}




//Funcion para saber si hay Hazar de datos 
int checkHazarDatos2(Buffers *buffers){


	int add;
	int sub;
	int mul;
	int div;
	int addi;
	int subi;

	int beq;
	int jump;
	int lw;
	int sw;

	int indice;
	char operacion[10];



	//Caso de add(EXMEM) mas otra cosa 
	//si en el exmen hay una instruccion
	indice=buffers->bufferEXMEM->indiceCode;
	strcpy(operacion,arrCodigo[indice]);
	//Tipo R
	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);
	//Branch
	beq=strcmp(intrucI[0],operacion);
	//Jump
	jump=strcmp(intrucJ[0],operacion);
	//Lw
	lw=strcmp(intrucM[0],operacion);
	//Sw
	sw=strcmp(intrucM[1],operacion);



	if ( (add==0 || sub==0 ||mul==0 || div==0) && (buffers->bufferEXMEM->indiceCode != -1) ){


		int difRS=strcmp(buffers->bufferEXMEM->rd,buffers->bufferIDEX->rs);		
		int difRT=strcmp(buffers->bufferEXMEM->rd,buffers->bufferIDEX->rt);		 
		printf("\nDif rs: %d",difRS);
		printf("\nDif rt: %d",difRT);

		//Comparar con instruccion que va mas atras
		indice=buffers->bufferIDEX->indiceCode;
		strcpy(operacion,arrCodigo[indice]);
		//Tipo R
		add=strcmp(intrucR[0],operacion);
		sub=strcmp(intrucR[1],operacion);
		mul=strcmp(intrucR[2],operacion);
		div=strcmp(intrucR[3],operacion);
		addi=strcmp(intrucR[4],operacion);
		subi=strcmp(intrucR[5],operacion);
		//Branch
		beq=strcmp(intrucI[0],operacion);
		//Jump
		jump=strcmp(intrucJ[0],operacion);
		//Lw
		lw=strcmp(intrucM[0],operacion);
		//Sw
		sw=strcmp(intrucM[1],operacion);


		//+ add + beq + sw      
		if (difRS==0 && (add==0 || beq==0 || sub==0 || mul==0 || div==0 || sw==0 || addi == 0 || subi == 0)){
			return 1;

		}

		//Que no sea un addi porque este solo sobre escribe
		if (difRT==0 && addi != 0 && subi != 0 && lw != 0 ){
			return 1;
		}

		
	}


	//Caso de addi(EXMEM) mas otra cosa 
	//si en el exmen hay una instruccion
	indice=buffers->bufferEXMEM->indiceCode;
	strcpy(operacion,arrCodigo[indice]);
	//Tipo R
	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);
	//Branch
	beq=strcmp(intrucI[0],operacion);
	//Jump
	jump=strcmp(intrucJ[0],operacion);
	//Lw
	lw=strcmp(intrucM[0],operacion);
	//Sw
	sw=strcmp(intrucM[1],operacion);



	if ( (addi==0 || subi==0) && (buffers->bufferEXMEM->indiceCode != -1) ){


		int difRS=strcmp(buffers->bufferEXMEM->rt,buffers->bufferIDEX->rs);		
		int difRT=strcmp(buffers->bufferEXMEM->rt,buffers->bufferIDEX->rt);		 
		printf("\nDif rs: %d",difRS);
		printf("\nDif rt: %d",difRT);

		//Comparar con instruccion que va mas atras
		indice=buffers->bufferIDEX->indiceCode;
		strcpy(operacion,arrCodigo[indice]);
		//Tipo R
		add=strcmp(intrucR[0],operacion);
		sub=strcmp(intrucR[1],operacion);
		mul=strcmp(intrucR[2],operacion);
		div=strcmp(intrucR[3],operacion);
		addi=strcmp(intrucR[4],operacion);
		subi=strcmp(intrucR[5],operacion);
		//Branch
		beq=strcmp(intrucI[0],operacion);
		//Jump
		jump=strcmp(intrucJ[0],operacion);
		//Lw
		lw=strcmp(intrucM[0],operacion);
		//Sw
		sw=strcmp(intrucM[1],operacion);

	
		//+ add + beq + sw      
		if (difRS==0 && (add==0 || beq==0 || sub==0 || mul==0 || div==0 || sw==0 || addi == 0 || subi == 0)){
			return 1;

		}

		//Que no sea un addi porque este solo sobre escribe
		if (difRT==0 && addi != 0 && subi != 0 && lw != 0 ){
			return 1;
		}

		
	}


	//Caso de lw(EXMEM) mas otra cosa 
	//si en el exmen hay una instruccion
	indice=buffers->bufferEXMEM->indiceCode;
	strcpy(operacion,arrCodigo[indice]);
	//Tipo R
	add=strcmp(intrucR[0],operacion);
	sub=strcmp(intrucR[1],operacion);
	mul=strcmp(intrucR[2],operacion);
	div=strcmp(intrucR[3],operacion);
	addi=strcmp(intrucR[4],operacion);
	subi=strcmp(intrucR[5],operacion);
	//Branch
	beq=strcmp(intrucI[0],operacion);
	//Jump
	jump=strcmp(intrucJ[0],operacion);
	//Lw
	lw=strcmp(intrucM[0],operacion);
	//Sw
	sw=strcmp(intrucM[1],operacion);



	if ( lw==0 && (buffers->bufferEXMEM->indiceCode != -1) ){


		int difRS=strcmp(buffers->bufferEXMEM->rt,buffers->bufferIDEX->rs);		
		int difRT=strcmp(buffers->bufferEXMEM->rt,buffers->bufferIDEX->rt);		 
		printf("\nDif rs: %d",difRS);
		printf("\nDif rt: %d",difRT);

		//Comparar con instruccion que va mas atras
		indice=buffers->bufferIDEX->indiceCode;
		strcpy(operacion,arrCodigo[indice]);
		//Tipo R
		add=strcmp(intrucR[0],operacion);
		sub=strcmp(intrucR[1],operacion);
		mul=strcmp(intrucR[2],operacion);
		div=strcmp(intrucR[3],operacion);
		addi=strcmp(intrucR[4],operacion);
		subi=strcmp(intrucR[5],operacion);
		//Branch
		beq=strcmp(intrucI[0],operacion);
		//Jump
		jump=strcmp(intrucJ[0],operacion);
		//Lw
		lw=strcmp(intrucM[0],operacion);
		//Sw
		sw=strcmp(intrucM[1],operacion);


		//+ add + beq + sw      
		if (difRS==0 && (add==0 || beq==0 || sub==0 || mul==0 || div==0 || sw==0 || addi == 0 || subi == 0)){
			return 1;

		}

		//Que no sea un addi porque este solo sobre escribe
		if (difRT==0 && addi != 0 && subi != 0 && lw != 0 ){
			return 1;
		}

		
	}
	


	
	printf("\n\n");
	return 0;

}



//Funcion para saber si hay Hazar de datos 
int checkHazarDatos(Buffers *buffers){

	//si en el exmen hay una instruccion
	if (buffers->bufferEXMEM->indiceCode != -1){
		printf("\n bufferEXMEM->RegWrite: %s",buffers->bufferEXMEM->RegWrite);
		printf("\n bufferEXMEM->regDest: %s",buffers->bufferEXMEM->regDest);
		printf("\n bufferEXMEM->rd: %s",buffers->bufferEXMEM->rd);

		printf("\n bufferIDEX->rs: %s",buffers->bufferIDEX->rs);
		printf("\n bufferIDEX->rt: %s",buffers->bufferIDEX->rt);

		if (buffers->bufferEXMEM->RegWrite=="1" && buffers->bufferEXMEM->regDest != "0" && buffers->bufferEXMEM->rd == buffers->bufferIDEX->rs){
			return 1;

		}

		if (buffers->bufferEXMEM->RegWrite=="1" && buffers->bufferEXMEM->regDest != "0" && buffers->bufferEXMEM->rd == buffers->bufferIDEX->rt){
			return 1;
			}

	}
	
	//Si en el memwb hay una instruccion 
	if (buffers->bufferMEMWB->indiceCode!=-1){
			printf("\n bufferMEMWB->RegWrite: %s",buffers->bufferMEMWB->RegWrite);
			printf("\n bufferMEMWB->regDest: %s",buffers->bufferMEMWB->regDest);
			printf("\n bufferMEMWB->rd: %s",buffers->bufferMEMWB->rd);

			printf("\n bufferIDEX->rs: %s",buffers->bufferIDEX->rs);
			printf("\n bufferIDEX->rt: %s",buffers->bufferIDEX->rt);

		if (buffers->bufferMEMWB->RegWrite=="1" && buffers->bufferMEMWB->regDest != "0" && buffers->bufferMEMWB->rd == buffers->bufferIDEX->rs){
			return 1;

		}

		if (buffers->bufferMEMWB->RegWrite=="1" && buffers->bufferMEMWB->regDest != "0" && buffers->bufferMEMWB->rd == buffers->bufferIDEX->rt){
			return 1;
		}
	}
	
	printf("\n\n");
	return 0;

}





//Funcion para saber si hay Hazard de control 



//Funcion para saber si se puede hacer el forwarding
int checkForwarding(Buffers *buffers){


	if (buffers->bufferIDEX->MemRead=="1" && buffers->bufferIDEX->rt==buffers->bufferIFID->rs ){
		return 1;

	}
	if (buffers->bufferIDEX->MemRead=="1" && buffers->bufferIDEX->rt==buffers->bufferIFID->rt ){
		return 1;
	}

	return 0;

}



//apoyo_>imprime los datos que hay en los buffers
void imprimirBuffer(Buffers *buffers, int nBuffers){


	switch (nBuffers){

		//Etapa de 
		case 1:
			if(buffers->bufferIFID->indiceCode != -1){
				printf("\n IFID:");
				printf("\n instruccion:  %s",buffers->bufferIFID->instruccion);
				printf("\n rd:  %s",buffers->bufferIFID->rd);
				printf("\n rs:  %s",buffers->bufferIFID->rs);
				printf("\n rt:  %s",buffers->bufferIFID->rt);
				printf("\n singExtended:  %d",buffers->bufferIFID->singExtended);
				printf("\n label:  %s",buffers->bufferIFID->label);
				printf("\n indiceCode:  %d",buffers->bufferIFID->indiceCode);
				printf("\n regDest:  %s",buffers->bufferIFID->regDest);
				printf("\n Jump:  %s",buffers->bufferIFID->Jump);
				printf("\n Branch:  %s",buffers->bufferIFID->Branch);
				printf("\n MemRead:  %s",buffers->bufferIFID->MemRead);
				printf("\n MemToRead:  %s",buffers->bufferIFID->MemToRead);
				printf("\n ALUOP:  %s",buffers->bufferIFID->ALUOP);
				printf("\n MemWrite:  %s",buffers->bufferIFID->MemWrite);
				printf("\n ALUsrc:  %s",buffers->bufferIFID->ALUsrc);
				printf("\n RegWrite:  %s",buffers->bufferIFID->RegWrite);
				printf("\n PC:  %d",buffers->bufferIFID->PC);

			} 
		
			break;

		//Case de ID
		case 2:
			if(buffers->bufferIDEX->indiceCode != -1){
				printf("\n IDEX:");
				printf("\n instruccion:  %s",buffers->bufferIDEX->instruccion);
				printf("\n rd:  %s",buffers->bufferIDEX->rd);
				printf("\n rs:  %s",buffers->bufferIDEX->rs);
				printf("\n rt:  %s",buffers->bufferIDEX->rt);
				printf("\n singExtended:  %d",buffers->bufferIDEX->singExtended);
				printf("\n label:  %s",buffers->bufferIDEX->label);
				printf("\n indiceCode:  %d",buffers->bufferIDEX->indiceCode);
				printf("\n regDest:  %s",buffers->bufferIDEX->regDest);
				printf("\n Jump:  %s",buffers->bufferIDEX->Jump);
				printf("\n Branch:  %s",buffers->bufferIDEX->Branch);
				printf("\n MemRead:  %s",buffers->bufferIDEX->MemRead);
				printf("\n MemToRead:  %s",buffers->bufferIDEX->MemToRead);
				printf("\n ALUOP:  %s",buffers->bufferIDEX->ALUOP);
				printf("\n MemWrite:  %s",buffers->bufferIDEX->MemWrite);
				printf("\n ALUsrc:  %s",buffers->bufferIDEX->ALUsrc);
				printf("\n RegWrite:  %s",buffers->bufferIDEX->RegWrite);
				printf("\n PC:  %d",buffers->bufferIDEX->PC);
			} 
	

			break;

		case 3: 

			if(buffers->bufferEXMEM->indiceCode != -1){
				printf("\n EXMEM:");
				printf("\n instruccion:  %s",buffers->bufferEXMEM->instruccion);
				printf("\n rd:  %s",buffers->bufferEXMEM->rd);
				printf("\n rs:  %s",buffers->bufferEXMEM->rs);
				printf("\n rt:  %s",buffers->bufferEXMEM->rt);
				printf("\n singExtended:  %d",buffers->bufferEXMEM->singExtended);
				printf("\n label:  %s",buffers->bufferEXMEM->label);
				printf("\n indiceCode:  %d",buffers->bufferEXMEM->indiceCode);
				printf("\n regDest:  %s",buffers->bufferEXMEM->regDest);
				printf("\n Jump:  %s",buffers->bufferEXMEM->Jump);
				printf("\n Branch:  %s",buffers->bufferEXMEM->Branch);
				printf("\n MemRead:  %s",buffers->bufferEXMEM->MemRead);
				printf("\n MemToRead:  %s",buffers->bufferEXMEM->MemToRead);
				printf("\n ALUOP:  %s",buffers->bufferEXMEM->ALUOP);
				printf("\n MemWrite:  %s",buffers->bufferEXMEM->MemWrite);
				printf("\n ALUsrc:  %s",buffers->bufferEXMEM->ALUsrc);
				printf("\n RegWrite:  %s",buffers->bufferEXMEM->RegWrite);
				printf("\n PC:  %d",buffers->bufferEXMEM->PC);

			} 
	
			break;

		case 4:

			if(buffers->bufferMEMWB->indiceCode != -1){
				printf("\n MEMWB:");
				printf("\n instruccion:  %s",buffers->bufferMEMWB->instruccion);
				printf("\n rd:  %s",buffers->bufferMEMWB->rd);
				printf("\n rs:  %s",buffers->bufferMEMWB->rs);
				printf("\n rt:  %s",buffers->bufferMEMWB->rt);
				printf("\n singExtended:  %d",buffers->bufferMEMWB->singExtended);
				printf("\n label:  %s",buffers->bufferMEMWB->label);
				printf("\n indiceCode:  %d",buffers->bufferMEMWB->indiceCode);
				printf("\n regDest:  %s",buffers->bufferMEMWB->regDest);
				printf("\n Jump:  %s",buffers->bufferMEMWB->Jump);
				printf("\n Branch:  %s",buffers->bufferMEMWB->Branch);
				printf("\n MemRead:  %s",buffers->bufferMEMWB->MemRead);
				printf("\n MemToRead:  %s",buffers->bufferMEMWB->MemToRead);
				printf("\n ALUOP:  %s",buffers->bufferMEMWB->ALUOP);
				printf("\n MemWrite:  %s",buffers->bufferMEMWB->MemWrite);
				printf("\n ALUsrc:  %s",buffers->bufferMEMWB->ALUsrc);
				printf("\n RegWrite:  %s",buffers->bufferMEMWB->RegWrite);
				printf("\n PC:  %d",buffers->bufferMEMWB->PC);
			} 
		

			break;


	}

}



//Funcion para desplazar datos de los buffers
void moverBuffers(Buffers *buffers){



	//Pasar las ultimas primero 


	//EXMEM-> MEMWB
	//buffers->bufferMEMWB->instruccion = buffers->bufferEXMEM->instruccion;
	strcpy(buffers->bufferMEMWB->instruccion,buffers->bufferEXMEM->instruccion);
	//buffers->bufferMEMWB->rd = buffers->bufferEXMEM->rd;
	//buffers->bufferMEMWB->rs = buffers->bufferEXMEM->rs;
	//buffers->bufferMEMWB->rt = buffers->bufferEXMEM->rt;
	strcpy(buffers->bufferMEMWB->rd,"\0");
	strcpy(buffers->bufferMEMWB->rs,"\0");
	strcpy(buffers->bufferMEMWB->rt,"\0");
	strcpy(buffers->bufferMEMWB->rd,buffers->bufferEXMEM->rd);
	strcpy(buffers->bufferMEMWB->rs,buffers->bufferEXMEM->rs);
	strcpy(buffers->bufferMEMWB->rt,buffers->bufferEXMEM->rt);
	buffers->bufferMEMWB->singExtended = buffers->bufferEXMEM->singExtended;
	buffers->bufferMEMWB->label = buffers->bufferEXMEM->label;
	buffers->bufferMEMWB->indiceCode = buffers->bufferEXMEM->indiceCode;
	buffers->bufferMEMWB->regDest = buffers->bufferEXMEM->regDest;
	buffers->bufferMEMWB->Jump = buffers->bufferEXMEM->Jump;
	buffers->bufferMEMWB->Branch = buffers->bufferEXMEM->Branch;
	buffers->bufferMEMWB->MemRead = buffers->bufferEXMEM->MemRead;
	buffers->bufferMEMWB->MemToRead = buffers->bufferEXMEM->MemToRead;
	buffers->bufferMEMWB->ALUOP = buffers->bufferEXMEM->ALUOP;
	buffers->bufferMEMWB->MemWrite = buffers->bufferEXMEM->MemWrite;
	buffers->bufferMEMWB->ALUsrc = buffers->bufferEXMEM->ALUsrc;
	buffers->bufferMEMWB->RegWrite = buffers->bufferEXMEM->RegWrite;
	buffers->bufferMEMWB->PC = buffers->bufferEXMEM->PC;

	//IDEX->EXMEM
	//buffers->bufferEXMEM->instruccion = buffers->bufferIDEX->instruccion;
	strcpy(buffers->bufferEXMEM->instruccion,buffers->bufferIDEX->instruccion);
	//buffers->bufferEXMEM->rd = buffers->bufferIDEX->rd;
	//buffers->bufferEXMEM->rs = buffers->bufferIDEX->rs;
	//buffers->bufferEXMEM->rt = buffers->bufferIDEX->rt;
	strcpy(buffers->bufferEXMEM->rd,"\0");
	strcpy(buffers->bufferEXMEM->rs,"\0");
	strcpy(buffers->bufferEXMEM->rt,"\0");
	strcpy(buffers->bufferEXMEM->rd,buffers->bufferIDEX->rd);
	strcpy(buffers->bufferEXMEM->rs,buffers->bufferIDEX->rs);
	strcpy(buffers->bufferEXMEM->rt,buffers->bufferIDEX->rt);
	buffers->bufferEXMEM->singExtended = buffers->bufferIDEX->singExtended;
	buffers->bufferEXMEM->label = buffers->bufferIDEX->label;
	buffers->bufferEXMEM->indiceCode = buffers->bufferIDEX->indiceCode;
	buffers->bufferEXMEM->regDest = buffers->bufferIDEX->regDest;
	buffers->bufferEXMEM->Jump = buffers->bufferIDEX->Jump;
	buffers->bufferEXMEM->Branch = buffers->bufferIDEX->Branch;
	buffers->bufferEXMEM->MemRead = buffers->bufferIDEX->MemRead;
	buffers->bufferEXMEM->MemToRead = buffers->bufferIDEX->MemToRead;
	buffers->bufferEXMEM->ALUOP = buffers->bufferIDEX->ALUOP;
	buffers->bufferEXMEM->MemWrite = buffers->bufferIDEX->MemWrite;
	buffers->bufferEXMEM->ALUsrc = buffers->bufferIDEX->ALUsrc;
	buffers->bufferEXMEM->RegWrite = buffers->bufferIDEX->RegWrite;
	buffers->bufferEXMEM->PC = buffers->bufferIDEX->PC;

	//IFID->IDEX
	//buffers->bufferIDEX->instruccion = buffers->bufferIFID->instruccion;
	strcpy(buffers->bufferIDEX->instruccion,buffers->bufferIFID->instruccion);
	//buffers->bufferIDEX->rd = buffers->bufferIFID->rd;
	//buffers->bufferIDEX->rs = buffers->bufferIFID->rs;
	//buffers->bufferIDEX->rt = buffers->bufferIFID->rt;
	strcpy(buffers->bufferIDEX->rd,"\0");
	strcpy(buffers->bufferIDEX->rs,"\0");
	strcpy(buffers->bufferIDEX->rt,"\0");
	strcpy(buffers->bufferIDEX->rd,buffers->bufferIFID->rd);
	strcpy(buffers->bufferIDEX->rs,buffers->bufferIFID->rs);
	strcpy(buffers->bufferIDEX->rt,buffers->bufferIFID->rt);
	buffers->bufferIDEX->singExtended = buffers->bufferIFID->singExtended;
	buffers->bufferIDEX->label = buffers->bufferIFID->label;
	buffers->bufferIDEX->indiceCode = buffers->bufferIFID->indiceCode;
	buffers->bufferIDEX->regDest = buffers->bufferIFID->regDest;
	buffers->bufferIDEX->Jump = buffers->bufferIFID->Jump;
	buffers->bufferIDEX->Branch = buffers->bufferIFID->Branch;
	buffers->bufferIDEX->MemRead = buffers->bufferIFID->MemRead;
	buffers->bufferIDEX->MemToRead = buffers->bufferIFID->MemToRead;
	buffers->bufferIDEX->ALUOP = buffers->bufferIFID->ALUOP;
	buffers->bufferIDEX->MemWrite = buffers->bufferIFID->MemWrite;
	buffers->bufferIDEX->ALUsrc = buffers->bufferIFID->ALUsrc;
	buffers->bufferIDEX->RegWrite = buffers->bufferIFID->RegWrite;
	buffers->bufferIDEX->PC = buffers->bufferIFID->PC;

	

	//IF
	//flush de IFID
	//buffers->bufferIDEX->instruccion = buffers->bufferIFID->instruccion;
	strcpy(buffers->bufferIFID->instruccion,"fin");
	//buffers->bufferIDEX->rd = buffers->bufferIFID->rd;
	//buffers->bufferIDEX->rs = buffers->bufferIFID->rs;
	//buffers->bufferIDEX->rt = buffers->bufferIFID->rt;
	strcpy(buffers->bufferIFID->rd,"\0");
	strcpy(buffers->bufferIFID->rs,"\0");
	strcpy(buffers->bufferIFID->rt,"\0");
	strcpy(buffers->bufferIFID->rd,"-1");
	strcpy(buffers->bufferIFID->rs,"-1");
	strcpy(buffers->bufferIFID->rt,"-1");
	buffers->bufferIFID->singExtended =-1 ;
	buffers->bufferIFID->label ="-1" ;
	buffers->bufferIFID->indiceCode = -1;
	buffers->bufferIFID->regDest ="-1" ;
	buffers->bufferIFID->Jump ="-1" ;
	buffers->bufferIFID->Branch ="-1" ;
	buffers->bufferIFID->MemRead = "-1";
	buffers->bufferIFID->MemToRead = "-1";
	buffers->bufferIFID->ALUOP = "-1";
	buffers->bufferIFID->MemWrite = "-1";
	buffers->bufferIFID->ALUsrc = "-1";
	buffers->bufferIFID->RegWrite ="-1" ;
	buffers->bufferIFID->PC = 0;


}



//crea archivo de salida con la traza
void crearSalidaTraza(FILE *streamEscritura, int nCiclos, int nInstrucciones){


	//Escribir etapas 
	fprintf(streamEscritura,"Etapas;");
	//Escribir cantidad de ciclos 
	int i=1;
	while (i<=nCiclos){
		fprintf(streamEscritura, "C %d;",i);
		i++;
	}

	//Escribir las instrucciones a ejecutar + etapas
	int j=0;
	int eliminarInstruccion;
	int inicio;
	int nComasAntes;
	int nComasDespues;
	inicio=j;
	while (j<5){

		//Se 
		if (j==0){
			fprintf(streamEscritura, "\nIF;");
		}
		if (j==1){
			fprintf(streamEscritura, "\nID;");	
		}
		if (j==2){
			fprintf(streamEscritura, "\nEX;");
		}
		if (j==3){
			fprintf(streamEscritura, "\nMEM;");
		}
		if (j==4){
			fprintf(streamEscritura, "\nWB;");
		}

		//Escribir comas antes
		nComasAntes=j;
		while (nComasAntes>0){
			fprintf(streamEscritura, ";");
			nComasAntes--;

		}

		//Escribir instruciones
		int k=0;
		int indice; 
		while (k<nInstrucciones){
			indice =arrTrazaCodigo[k];

			//Si es un nop
			if (indice==-1){
				fprintf(streamEscritura, "NOP;");
			}



			else{

				//instruccion
				fprintf(streamEscritura, "%s ",arrCodigo[indice]);
				char *instruccion = malloc( sizeof(char) * (10) );
				strcpy(instruccion,arrCodigo[indice]);
				int grupo=verificarGrupo(instruccion);


				//Asignando los valores en los buffers
				switch (grupo) {


					//Tipo R
					case 1:
				
						fprintf(streamEscritura, "%s ",arrCodigo[indice+1] );
						fprintf(streamEscritura, "%s ",arrCodigo[indice+2] );
						fprintf(streamEscritura, "%s;",arrCodigo[indice+3] );
					
						break;




					//Tipo I	
					case 2:
			
						fprintf(streamEscritura, "%s ",arrCodigo[indice+1] );
						fprintf(streamEscritura, "%s ",arrCodigo[indice+2] );
						fprintf(streamEscritura, "%s;",arrCodigo[indice+3] );

						break;
						


					//Tipo J
					case 3:

						fprintf(streamEscritura, "%s;",arrCodigo[indice+1] );


						break;
					


					//Tipo M
					case 4:
						
						fprintf(streamEscritura, "%s ",arrCodigo[indice+1] );
						fprintf(streamEscritura, "%s;",arrCodigo[indice+2] );
						
						break;



		

				} 


			}



			
			k++;
		}



		j++;

	}
	
}


void crearSalidaHazard(FILE *streamEscritura, int nCiclos){

	//Encabezados
	fprintf(streamEscritura, "Hazard;"); 
	int c=1;
	while(c<=nCiclos){

		fprintf(streamEscritura, "C%d;",c); 
		c++;
	}


	//HC
	fprintf(streamEscritura, "\nHC;");
	int i=0;
	while (i<nCiclos){
		fprintf(streamEscritura, "%s;",arrHazardC[i]);
		i++;
	}


	//HD
	fprintf(streamEscritura, "\nHD;");
	int j=0;
	while (j<nCiclos){
		fprintf(streamEscritura, "%s;",arrHazardD[i]);
		j++;
	}

}


//Funcion que lee todo el arreglo donde se encuentran las instrucciones del codigo en mips
int trazaCodigo(FILE *streamCodigo,FILE  *streamEscribir, int largo,Buffers *buffers){

	int nCiclos=0;
	//Declaracion de variables 
		//char instruccion[6];
		char *instruccion = malloc( sizeof(char) * (10) );
		//char rd[4];
		char *rd = malloc( sizeof(char) * (8) );
		//char rs[6];
		char *rs = malloc( sizeof(char) * (8) );
		//char rt[6];
		char *rt = malloc( sizeof(char) * (8) );
		//char branch[30];
		char *branch = malloc( sizeof(char) * (20) );

		char *shampt = malloc( sizeof(char) * (20) );
		//registro de lw o sw (rm)
		char *rm = malloc( sizeof(char) * (20) );
		char *salto = malloc( sizeof(char) * (20) );
		int saltoEntero;

		//char *instruccionCompleta = malloc( sizeof(char) * (40) );

		//int shampt;
		int rtC;
		int grupo;

		int subi;
		int addi;

		int indice;
		int valorrs;
		int valorrt;
		int valorrd;

		int num;

		int boolean;
		int largoPalabra;


		int finArchivo;
		finArchivo=feof(streamCodigo);

		int indiceCode=0;
		int indiceEncontrado;
		int difFin= strcmp(buffers->bufferMEMWB->instruccion,"fin");
		//printf("\nDif: %d",difFin);


	//Recorrer hasta que terminen todos los buffers
	while (indiceCode < largo || difFin != 0){
	
		difFin= strcmp(buffers->bufferMEMWB->instruccion,"fin");
		//printf("\nDif: %d",difFin);


		//Si ya no se estan leyendo mas instrucciones
		if (indiceCode >= largo){
			arrTrazaCodigo[nCiclos]=-2;
		}



		//----------------IF------------------  
		if (difFin != 0){


		//Intruccion a leer en el codigo
		char *instruccionCompleta = malloc( sizeof(char) * (40) );
		strcpy(instruccion,arrCodigo[indiceCode]);
		grupo=verificarGrupo(instruccion);
		arrTrazaCodigo[nCiclos]=indiceCode;

		//Asignando los valores en los buffers
		switch (grupo) {


			//Tipo R
			case 1:
				
				//Leer instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);

				//Si es add o sub
				if (arrCodigo[indiceCode+3][0] =='$'){
					//rd
					strcpy(rd,arrCodigo[indiceCode+1]);
					//rs
					strcpy(rs,arrCodigo[indiceCode+2]);
					//rt
					strcpy(rt,arrCodigo[indiceCode+3]);
					//Juntar instruccion completa
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rd);
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rs);
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rt);
					printf("\nInstruccion completa: %s",instruccionCompleta);


					//asignar valores de de instrucciones
					strcpy(buffers->bufferIFID->instruccion,instruccionCompleta);
					largoPalabra=strlen(rd);
					rd[largoPalabra-1]='\0';
					strcpy(buffers->bufferIFID->rd,rd);
					strcpy(buffers->bufferIFID->rt,rt);
					largoPalabra=strlen(rs);
					rs[largoPalabra-1]='\0';
					strcpy(buffers->bufferIFID->rs,rs);
					buffers->bufferIFID->singExtended=-1;
					buffers->bufferIFID->label="-1";
					buffers->bufferIFID->indiceCode=indiceCode;
				}
				else{
					//rt
					strcpy(rt,arrCodigo[indiceCode+1]);
					
					//rs
					strcpy(rs,arrCodigo[indiceCode+2]);
					//singExtended
					rd=arrCodigo[indiceCode+3];
					saltoEntero=atoi(rd);

					//Juntar instruccion completa
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rt);
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rs);
					strcat(instruccionCompleta," ");
					strcat(instruccionCompleta,rd);
					printf("\nInstruccion completa: %s",instruccionCompleta);
					//asignar valores de de instrucciones
					strcpy(buffers->bufferIFID->instruccion,instruccionCompleta);
					largoPalabra=strlen(rt);
					rt[largoPalabra-1]='\0';
					strcpy(buffers->bufferIFID->rt,rt);
					largoPalabra=strlen(rs);
					rs[largoPalabra-1]='\0';
					strcpy(buffers->bufferIFID->rs,rs);
					buffers->bufferIFID->singExtended=saltoEntero;
					strcpy(buffers->bufferIFID->rd,"-1");
					buffers->bufferIFID->label="-1";
					buffers->bufferIFID->indiceCode=indiceCode;
				} 

				



				//Guardar los datos en los buffers
				//valores de las señales de control
				asignarLineasControl(buffers,arrCodigo[indiceCode]);
				


				//printf("\n ||rs: %s||",rs );
			
				//imprimirBuffer(buffers,1);




				printf("\n");
				indiceCode=indiceCode+4;
				free(instruccionCompleta);
				break;




			//Tipo I	
			case 2:
	
				
				//Leer instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
				//rd
				strcpy(rs,arrCodigo[indiceCode+1]);
				//rs
				strcpy(rt,arrCodigo[indiceCode+2]);
				//Label de branch
				strcpy(branch,arrCodigo[indiceCode+3]);

				//Juntar instruccion completa
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rs);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rt);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,branch);
				printf("\nInstruccion completa: %s",instruccionCompleta);


				//Guardar los datos en los buffers
				//valores de las señales de control
				asignarLineasControl(buffers,arrCodigo[indiceCode]);
				//asignar valores de de instrucciones
				//buffers->bufferIFID->instruccion=instruccionCompleta;
				strcpy(buffers->bufferIFID->instruccion,instruccionCompleta);
				//buffers->bufferIFID->rs=rs;
				//buffers->bufferIFID->rt=rt;
				//buffers->bufferIFID->rt="-1";
				largoPalabra=strlen(rd);
				rd[largoPalabra-1]='\0';
				strcpy(buffers->bufferIFID->rd,"-1");
				largoPalabra=strlen(rs);
				rs[largoPalabra-1]='\0';
				strcpy(buffers->bufferIFID->rs,rs);
				rt[largoPalabra-1]='\0';
				strcpy(buffers->bufferIFID->rt,rt);
				buffers->bufferIFID->singExtended=-1;
				buffers->bufferIFID->label=branch;
				buffers->bufferIFID->indiceCode=indiceCode;
				//imprimirBuffer(buffers,1);

	

			
			
				printf("\n");
				indiceCode=indiceCode+4;
				free(instruccionCompleta);
					


				break;
				


			//Tipo J
			case 3:

				//Instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
				//branch
				strcpy(branch,arrCodigo[indiceCode+1]);
				

				//Juntar instruccion
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,branch);
				printf("\nInstruccion completa: %s",instruccionCompleta);



				//Guardar los datos en los buffers
				//valores de las señales de control
				asignarLineasControl(buffers,arrCodigo[indiceCode]);
				//asignar valores de de instrucciones
				//buffers->bufferIFID->instruccion=instruccionCompleta;
				strcpy(buffers->bufferIFID->instruccion,instruccionCompleta);
				buffers->bufferIFID->indiceCode=indiceCode;
				//buffers->bufferIFID->rd="-1";
				//buffers->bufferIFID->rs="-1";
				//buffers->bufferIFID->rt="-1";
				strcpy(buffers->bufferIFID->rd,"-1");
				strcpy(buffers->bufferIFID->rs,"-1");
				strcpy(buffers->bufferIFID->rt,"-1");
				buffers->bufferIFID->singExtended=-1;
				buffers->bufferIFID->label=branch;
				//imprimirBuffer(buffers,1);




				
				printf("\n");
				free(instruccionCompleta);
				

				break;
			


			//Tipo M
			case 4:
				

				//Instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
				//rd
				strcpy(rt,arrCodigo[indiceCode+1]);
				//shampt
				strcpy(shampt,arrCodigo[indiceCode+2]);

				//Juntar instruccion
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rt);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,shampt);
				printf("\nInstruccion completa: %s",instruccionCompleta);

				
		

				//Guardar los datos en los buffers
				//valores de las señales de control
				asignarLineasControl(buffers,arrCodigo[indiceCode]);
				//asignar valores de de instrucciones
				//buffers->bufferIFID->instruccion=instruccionCompleta;
				strcpy(buffers->bufferIFID->instruccion,instruccionCompleta);
				//buffers->bufferIFID->rt=rt;
				//buffers->bufferIFID->rd="-1";
				strcpy(buffers->bufferIFID->rd,"-1");
				largoPalabra=strlen(rt);
				rt[largoPalabra-1]='\0';
				strcpy(buffers->bufferIFID->rt,rt);
				buffers->bufferIFID->indiceCode=indiceCode;
								


				//recuperar registros
				//separar la parte del entero y la parte del registro
				int l=1;
				largoPalabra=strlen(shampt);
				//printf("\nEl ultimo elemto del SHAMPT es: %c",shampt[largoPalabra-1]);
				//strcpy(rm,shampt[largoPalabra-4]);
				rs[0]=shampt[largoPalabra-4];
				//strcat(rm,shampt[largoPalabra-3]);
				rs[1]=shampt[largoPalabra-3];
				//strcat(rm,shampt[largoPalabra-2]);
				rs[1]=shampt[largoPalabra-2];
				//añadir coma 
				rs[3]=',';
				rs[4]='\0';
				//obtener el entero de desplazamiento
				//strcpy(salto,shampt[0]);
				salto[0]=shampt[0];
				while (l<largoPalabra-4){
					//strcat(salto,shampt[l]);
					salto[l]=shampt[l];
					l++;

				}
				//convertir string de salto a entero
				saltoEntero=atoi(salto);

				//printf("\nEl salto entero es: %d", saltoEntero);


				//rs en buffer
				//buffers->bufferIFID->rs=rs;
				strcpy(buffers->bufferIFID->rs,"$sp");
				buffers->bufferIFID->singExtended=saltoEntero;
				buffers->bufferIFID->label="-1";
				//imprimirBuffer(buffers,1);



				printf("\n");
				
				indiceCode=indiceCode+3;
				free(instruccionCompleta);
				break;



			//Si la instruccion no esta dentro de las demas instrucciones 
			case -1:

				indiceCode=indiceCode+1;

				//fprintf(streamEscribir, "%s", "\n");
			
				break;	

		} //switch-case de asignacion de buffers


		}
		//----------------IF------------------
		//agregar instruccion al arreglo con la traza con indices



		/*
		printf("\n*************************************");
		imprimirBuffer(buffers,1);
		imprimirBuffer(buffers,2);
		imprimirBuffer(buffers,3);
		imprimirBuffer(buffers,4);
		printf("\n*************************************");
		*/




		//detectar HZ
		int boolHazard =checkHazarDatos2(buffers);
		//printf("\nBool Hazard: %d", boolHazard);
		if (boolHazard!=0){
			printf("\nHAY UN HAZARD\n\n");
		}


		//etapa de id 
		if (buffers->bufferIFID->Jump[0] == '1'){

			//saltar al label del jump
			//agregar dos puntos al final del label
			largoPalabra=strlen(branch);
			branch[largoPalabra]=':';
			branch[largoPalabra+1]='\0';

			//printf("\nLabel a buscar es: |%s|",branch);
					
			//buscar label en el arreglo de con las instrucciones del codigo
			indiceEncontrado=buscarLabelCodigo(branch,largo);
			indiceCode=indiceEncontrado;

		}





		//-----------------EX-----------------
		//Hacer operaciones correspondientes
		//Segun el buffer de EXMEM
		/*
		int indiceOperacion;
		if (buffers->bufferEXMEM->indiceCode != -1){

			indiceOperacion=buffers->bufferEXMEM->indiceCode;
			instruccion=arrCodigo[indiceOperacion];
			grupo=verificarGrupo(instruccion);



			switch (grupo){

				//Tipo R
				case 1:


					//Hacer la operacion 
					rt=buffers->bufferEXMEM->rt;
					rs=buffers->bufferEXMEM->rs;
					rd=buffers->bufferEXMEM->rd;
					
					//Depende si son de branch o si son de addi y/o subi
					addi=-1;
					subi=-1;
					addi=strcmp(instruccion,intrucR[4]);
					subi=strcmp(instruccion,intrucR[5]);

					if(addi==0 || subi==0){

						rs=buffers->bufferEXMEM->rs;
						indice=obtenerIndice(rs);
						valorrs=valoresRegistro[indice];
						valorrt=hacerOperacionR(instruccion,valorrs,buffers->bufferEXMEM->singExtended);
						indice=obtenerIndice(rt);
						valoresRegistro[indice]=valorrt;

							
					}
					else {

						indice=obtenerIndice(rt);
						valorrt=valoresRegistro[indice];
						indice=obtenerIndice(rs);
						valorrs=valoresRegistro[indice];
						valorrd=hacerOperacionR(instruccion,valorrs,valorrt);
						indice=obtenerIndice(rd);
						valoresRegistro[indice]=valorrd;
						
						
					}
					

				
					
					//imprimirRegistros(streamEscribir);
					

					break;

				//Tipo I
				case 2:

					//=buffers->bufferEXMEM->rs;
					rt=buffers->bufferEXMEM->rt;
					branch=buffers->bufferEXMEM->Branch;

					//Hacer operacion del Branch
					//valorrs=valoresRegistro[obtenerIndice(rs)];
					valorrt=valoresRegistro[obtenerIndice(rt)];	
					boolean=hacerOperacionI(instruccion,valorrs,valorrt);



					//agregar los dos puntos a los label
					largoPalabra=strlen(branch);
					branch[largoPalabra]=':';
					branch[largoPalabra+1]='\0';


					
					//si entra debe buscar el labels
					if (boolean == 1){
						indiceEncontrado=buscarLabelCodigo(branch,largo);
						indiceCode=indiceEncontrado+1;

					}
					//si NO cumple la condicion, saltar al branch 
					else if (boolean == 0){
						indiceCode=indiceCode+4;	
					}

					

					break;

				//Tipo J
				case 3:

					branch=buffers->bufferEXMEM->label;

					//Hacer operacion 

					//agregar dos puntos al final del label
					largoPalabra=strlen(branch);
					branch[largoPalabra]=':';
					branch[largoPalabra+1]='\0';

					//printf("\nLabel a buscar es: |%s|",branch);
					
					//buscar label en el arreglo de con las instrucciones del codigo
					indiceEncontrado=buscarLabelCodigo(branch,largo);
					//printf("\nEl indice encontrado es: %d / con el label: %s",indiceEncontrado,arrCodigo[indiceEncontrado] );
					indiceCode=indiceEncontrado+1;
					


					break;

				//Tipo M 
				case 4:


					saltoEntero=buffers->bufferEXMEM->singExtended;
					rt=buffers->bufferEXMEM->rt;
					//Hacer operacion de memoria 	
					
					
					int lw;
					int sw;
					lw=strcmp(instruccion,intrucM[0]);
					sw=strcmp(instruccion,intrucM[1]);

					//lw 
					if (lw == 0){
						//desplazar el salto al medio del arreglo (stackPointer)
						saltoEntero=saltoEntero+200;
						valorrt=stackPointer[saltoEntero];
						indice=obtenerIndice(rt);
						valoresRegistro[indice]=valorrt;
						//printf("\nEl valor guardado en los registros es: %d",valoresRegistro[indice]);
					}
					else if (sw == 0){
						//desplazar el salto al medio del arreglo (stackPointer)
						saltoEntero=saltoEntero+200;
						indice=obtenerIndice(rt);
						valorrt=valoresRegistro[indice];
						stackPointer[saltoEntero]=valorrt;
						//printf("\nEl valor guardado en los registros es: %d",stackPointer[saltoEntero]);
					}
		
					


					break;

			}	//switch de ejecucion de datos en los buffers

		}
		
		*/
		//-----------------EX-----------------






		//--------------mover instrucciones--------------
		moverBuffers(buffers);
		//--------------mover instrucciones--------------
		


		//Se pasa al siguente ciclo
		nCiclos++;
		finArchivo=feof(streamCodigo);

	} //while (fin archivo)
	
	
	free(instruccion);
	free(rd);
	free(rs);
	free(rt);
	return nCiclos;
}




//Funcion de apoyo para imprimir los valores de los registro 
void imprimirRegistros(FILE *stream){

	int i; 

	//fprintf(stream, "%s","\t");
	
	for (i=0;i<32;i++){


		printf("//%s -> %d\n",indicesRegistro[i],valoresRegistro[i]);

	}
	/*
	fprintf(stream, "%s -> %d",indicesRegistro[0],valoresRegistro[0]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[1],valoresRegistro[1]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[2],valoresRegistro[2]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[3],valoresRegistro[3]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[4],valoresRegistro[4]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[5],valoresRegistro[5]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[6],valoresRegistro[6]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[7],valoresRegistro[7]);fprintf(stream, "%s","\t");fprintf(stream, "%s","\n\t");
	fprintf(stream, "%s -> %d",indicesRegistro[8],valoresRegistro[8]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[9],valoresRegistro[9]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[10],valoresRegistro[10]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[11],valoresRegistro[11]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[12],valoresRegistro[12]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[13],valoresRegistro[13]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[14],valoresRegistro[14]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[15],valoresRegistro[15]);fprintf(stream, "%s","\t");fprintf(stream, "%s","\n\t");
	fprintf(stream, "%s -> %d",indicesRegistro[16],valoresRegistro[16]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[17],valoresRegistro[17]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[18],valoresRegistro[18]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[19],valoresRegistro[19]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[20],valoresRegistro[20]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[21],valoresRegistro[21]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[22],valoresRegistro[22]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[23],valoresRegistro[23]);fprintf(stream, "%s","\t");fprintf(stream, "%s","\n\t");
	fprintf(stream, "%s -> %d",indicesRegistro[24],valoresRegistro[24]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[25],valoresRegistro[25]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[26],valoresRegistro[26]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[27],valoresRegistro[27]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[28],valoresRegistro[28]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[29],valoresRegistro[29]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[30],valoresRegistro[30]);fprintf(stream, "%s","\t");
	fprintf(stream, "%s -> %d",indicesRegistro[31],valoresRegistro[31]);fprintf(stream, "%s","\t");fprintf(stream, "%s","\n\n");
	*/

}



//Funcion para obtener el indice de correspondiente al registro de entrada 
int obtenerIndice(char *registro){

	int i;
	int dif=-99;

	for(i=0;i<32;i++){

		dif=strcmp(registro,indicesRegistro[i]);
		if(dif == 0){
			return i;
		} 

	}

}





//Funcion para cargar los valores iniciales en los registros 
void cargarRegistros(FILE* stream ){

	char *strRegistro = malloc( sizeof(char) * (10) );
	int valorRegistro;


	int i=0;
	while (i<32) {

		//Leer registro 

		//agregar la coma 

		//leer el valor a guardar

		//guardar el valor en el registro 


		//Version simple 
		//Se lee el registro en cuestion
		fscanf(stream,"%s",strRegistro);
		//Se lee el valor a guardar 
		fscanf(stream,"%d",&valorRegistro);
		//Se guarda el regitro
		valoresRegistro[i]=valorRegistro;


		i++;
	}
	free(strRegistro);

}





//Funcion para hacer flush de algun buffer en particular cls
void flushBuffers(Buffers *buffers,int nBuffers){


	switch (nBuffers){

		//Etapa de 
		case 1:

			strcpy(buffers->bufferIFID->instruccion,"-1");
			strcpy(buffers->bufferIFID->rd,"-1");
			strcpy(buffers->bufferIFID->rs,"-1");
			strcpy(buffers->bufferIFID->rt,"-1");
			buffers->bufferIFID->singExtended = -1;
			buffers->bufferIFID->label = "-1";
			buffers->bufferIFID->indiceCode =-1;
			buffers->bufferIFID->regDest = "-1";
			buffers->bufferIFID->Jump = "-1";
			buffers->bufferIFID->Branch = "-1";
			buffers->bufferIFID->MemRead = "-1";
			buffers->bufferIFID->MemToRead ="-1";
			buffers->bufferIFID->ALUOP = "-1";
			buffers->bufferIFID->MemWrite = "-1";
			buffers->bufferIFID->ALUsrc = "-1";
			buffers->bufferIFID->RegWrite = "-1"; 
			buffers->bufferIFID->PC = -1;

			break;

		//Case de ID
		case 2:
			strcpy(buffers->bufferIDEX->instruccion,"-1");
			strcpy(buffers->bufferIDEX->rd,"-1");
			strcpy(buffers->bufferIDEX->rs,"-1");
			strcpy(buffers->bufferIDEX->rt,"-1");
			buffers->bufferIDEX->singExtended = -1;
			buffers->bufferIDEX->label = "-1";
			buffers->bufferIDEX->indiceCode =-1;
			buffers->bufferIDEX->regDest = "-1";
			buffers->bufferIDEX->Jump = "-1";
			buffers->bufferIDEX->Branch = "-1";
			buffers->bufferIDEX->MemRead = "-1";
			buffers->bufferIDEX->MemToRead ="-1";
			buffers->bufferIDEX->ALUOP = "-1";
			buffers->bufferIDEX->MemWrite = "-1";
			buffers->bufferIDEX->ALUsrc = "-1";
			buffers->bufferIDEX->RegWrite = "-1"; 
			buffers->bufferIDEX->PC = -1;

			break;

		case 3: 

			strcpy(buffers->bufferEXMEM->instruccion,"-1");
			strcpy(buffers->bufferEXMEM->rd,"-1");
			strcpy(buffers->bufferEXMEM->rs,"-1");
			strcpy(buffers->bufferEXMEM->rt,"-1");
			buffers->bufferEXMEM->singExtended = -1;
			buffers->bufferEXMEM->label = "-1";
			buffers->bufferEXMEM->indiceCode =-1;
			buffers->bufferEXMEM->regDest = "-1";
			buffers->bufferEXMEM->Jump = "-1";
			buffers->bufferEXMEM->Branch = "-1";
			buffers->bufferEXMEM->MemRead = "-1";
			buffers->bufferEXMEM->MemToRead ="-1";
			buffers->bufferEXMEM->ALUOP = "-1";
			buffers->bufferEXMEM->MemWrite = "-1";
			buffers->bufferEXMEM->ALUsrc = "-1";
			buffers->bufferEXMEM->RegWrite = "-1"; 
			buffers->bufferEXMEM->PC = -1;

			break;

		case 4:

			strcpy(buffers->bufferMEMWB->instruccion,"-1");
			strcpy(buffers->bufferMEMWB->rd,"-1");
			strcpy(buffers->bufferMEMWB->rs,"-1");
			strcpy(buffers->bufferMEMWB->rt,"-1");
			buffers->bufferMEMWB->singExtended = -1;
			buffers->bufferMEMWB->label = "-1";
			buffers->bufferMEMWB->indiceCode =-1;
			buffers->bufferMEMWB->regDest = "-1";
			buffers->bufferMEMWB->Jump = "-1";
			buffers->bufferMEMWB->Branch = "-1";
			buffers->bufferMEMWB->MemRead = "-1";
			buffers->bufferMEMWB->MemToRead ="-1";
			buffers->bufferMEMWB->ALUOP = "-1";
			buffers->bufferMEMWB->MemWrite = "-1";
			buffers->bufferMEMWB->ALUsrc = "-1";
			buffers->bufferMEMWB->RegWrite = "-1"; 
			buffers->bufferMEMWB->PC = -1;

			break;


	}

}



int main (){
	



	//------------------------------------------------------------------PRUEBAS--------------------------------------------------------------------------------
	//Pedir memoria para las estructuras de los buffers

	//Archivos de lectura 		para leer los registros 
	FILE *archivoRegistros;	
	archivoRegistros = fopen("inputRegistros.txt","r");
	cargarRegistros(archivoRegistros);

	//imprimirRegistros(archivoRegistros);

	fclose(archivoRegistros);

	//pedir memoria para las estructuras (Buffers)
	struct IFID *b_IFID=malloc(sizeof(IFID));
	struct IDEX *b_IDEX=malloc(sizeof(IDEX));
	struct EXMEM *b_EXMEM=malloc(sizeof(EXMEM));
	struct MEMWB *b_MEMWB=malloc(sizeof(MEMWB));

	//pedir memoria para almacenador de buffers
	struct Buffers *buffers=malloc(sizeof(Buffers));
	buffers->bufferIFID=b_IFID;
	buffers->bufferIDEX=b_IDEX;
	buffers->bufferEXMEM=b_EXMEM;
	buffers->bufferMEMWB=b_MEMWB;

	buffers->bufferIFID->indiceCode=-1;
	buffers->bufferIDEX->indiceCode=-1;
	buffers->bufferEXMEM->indiceCode=-1;
	buffers->bufferMEMWB->indiceCode=-1;



	//-------------------------------------------------------------------------------------------------------------------------------------------


	
	
	//*********************************************************************************************************************************************
	//Se cargan los archivos con los cuales se va a trabajar 


	//Pedir nombre archivo entrada corespondiente al codigo de Mips
	//char *nombreCodigo=(char*)malloc(sizeof(char*));
	char *nombreCodigo= malloc( sizeof(char) * (100) );
	//pedirNombreCodigo(nombreCodigo);
	FILE *archivoLectura;	
	//archivoLectura = fopen(nombreCodigo,"r");
	archivoLectura = fopen("ejemploMips2.asm","r");
	//Si no se encuentra 
	if (archivoLectura == NULL){
		printf("\nNo se encontro el archivo con el nombre: %s",nombreCodigo);
		return 0;
	}


	//Archivo de salida con traza
	//char *nombreSal=(char*)malloc(sizeof(char*));
	char *nombreSal= malloc( sizeof(char) * (100) );
	//pedirNombreSalida(nombreSal);
	FILE *archivoSal;	
	//archivoSal = fopen(nombreSal,"w");
	archivoSal = fopen("salida.csv","w");
	//Si no se encuentra 
	if (archivoSal == NULL){
		printf("\nNo se encontro el archivo con el nombre: %s",nombreSal);
		return 0;
	}


	//Archivo de salida con hazard 
	//char *nombreSal=(char*)malloc(sizeof(char*));
	char *nombreSal2= malloc( sizeof(char) * (100) );
	//pedirNombreSalida(nombreSal);
	FILE *archivoSal2;	
	//archivoSal = fopen(nombreSal,"w");
	archivoSal2 = fopen("salida2.csv","w");
	//Si no se encuentra 
	if (archivoSal2 == NULL){
		printf("\nNo se encontro el archivo con el nombre: %s",nombreSal2);
		return 0;
	}




	printf("\n\n*******************************TRAZA****************************");
	//**********************************************************************************************************************************+
	//

	//Se  Lee el codigo completo y guarda en un arreglo
	int largo=leerCodigo(archivoLectura);


	//Funcion principal para analizar el codigo y los dos archivos 
	int totalCiclos=trazaCodigo(archivoLectura,archivoSal,largo,buffers);
	printf("\n El codigo genera un total de %d Ciclos", totalCiclos);

	printf("\n*******************************TRAZA****************************\n\n");

	//arrTrazaCodigo[0]=0;
	//arrTrazaCodigo[1]=-1;
	//arrTrazaCodigo[2]=3;
	//arrTrazaCodigo[3]=7;
	//arrTrazaCodigo[4]=11;
	//arrTrazaCodigo[5]=15;
	//arrTrazaCodigo[6]=18;
	//fin
	//arrTrazaCodigo[7]=-2;


	crearSalidaTraza(archivoSal,totalCiclos,contarInstrucciones());
	crearSalidaHazard(archivoSal2,totalCiclos);
	printf("\nSe ha creado un archivo con el nombre:  %s\n", "salida.csv");


	//*******************************************************************************************************************************************************
	
	//Se libera la memoria utilizada 
	//Liberan las cadenas de caracteres de los nombre de los archivos 
	free(nombreCodigo);
	free(nombreSal);
	free(nombreSal2);

	//Se cierran los archivos leidos 
	fclose(archivoLectura);
	fclose(archivoSal);
	fclose(archivoSal2);

	
	

	return 0;
} 