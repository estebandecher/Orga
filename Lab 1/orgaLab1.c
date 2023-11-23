#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>





//Funciones
void pedirNombreCodigo(char *nombre);
void pedirNombreSenales(char *nombre);
void crearSalidaSenales(FILE *stream,FILE  *streamEscribir);
void pedirNombreSalida(int n,char *nombre);
void escribirSenalesControl(FILE *stream);
int verificarGrupo(char *operacion);
int hacerOperacionR(char *operacion,int rs,int rt);
int hacerOperacionI(char *operacion,int rs,int rt);
void buscarLabel(FILE *streamLectura,char *label);
void escribirRegitros(int *PC,int *PCActual,FILE *streamLectura,FILE *streamEscritura);
void imprimirRegistros(FILE *stream);
int obtenerIndice(char *registro);






//Arreglos 
char intrucR[10][10]={"add","sub","mul","div","addi","subi"};
char intrucI[10][10]={"beq","blt","bgt"};
char intrucJ[10][10]={"j","jal","jr"};
char intrucM[10][10]={"lw","sw","la"};

int valoresRegistro[32]={0};
char indicesRegistro[34][8]={"$zero," ,"$at,","$v0,","$v1,","$a0,","$a1,","$a2,","$a3,","$t0,","$t1,","$t2,","$t3,","$t4,","$t5,","$t6,","$t7,","$s0,","$s1,","$s2,","$s3,","$s4,","$s5,","$s6,","$s7,","$t8,","$t9,","$k0,","$k1,","$gp,","$sp,","$fp,","$ra,"};
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









//Definicion de funciones 


void pedirNombreCodigo(char *nombre){
	
	printf("\nIngrese el nombre correspondiente a su archivo de lectura(Codigo en Asemble ) \n");
	printf("Nombre: ");
	
	
	scanf("%s",nombre);
	printf("\n El nombre del archivo es: %s ", nombre);
	

}

void pedirNombreSenales(char *nombre){
	
	printf("\nIngrese el nombre correspondiente a su archivo de lectura(Valores de señales de control) \n");
	printf("Nombre: ");
	
	
	scanf("%s",nombre);
	printf("\n El nombre del archivo es: %s ", nombre);
	

}



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



void pedirNombreSalida(int n,char *nombre){
	
	//char texto[20];
	if (n == 0){
		
		
		printf("\nIngrese el nombre de su primer archivo de salida \n");
		printf("Nombre: ");
		scanf("%s",nombre);
		//nombre=texto;
	}
	else if(n == 1) {

		printf("\nIngrese el nombre de su segundo archivo de salida \n");
		printf("Nombre: ");
		scanf("%s",nombre);
		//nombre=texto;
	}
	
}



void escribirSenalesControl(FILE *stream){
	
	fprintf(stream,"%s","Instruccion 		||RegDst||Jump||Branch||MemRead||MemToReg||ALUOP||MemWrite||ALUSrc||RegWrite||\n");
	
}




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



void buscarLabel(FILE *streamLectura,char *label){

	char branch[20];
	rewind(streamLectura);

	//*PC=0;


	int dif;

	int instrucciones;

    int finArchivo;

	
	finArchivo=feof(streamLectura);

	

	while (finArchivo == 0){


		fscanf(streamLectura,"%s",branch);
		instrucciones=verificarGrupo(branch);
		dif=strcmp(branch,label);

		//Si se encuentra el label
		if (dif ==0 ){
			
			return;

		}
		//si es una isntuccion normal 
		else if (instrucciones != -1){
			//*PC=*PC+4;

		}
		//Si no es una istruccion no se aumenta el PC
		else if (instrucciones == -1 ){
			//no se debe hacer nada

		}

	}//while 


}


//Devulve el indice en el cual se debe ubicar
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


//Funcion para ubicarse en la linea de control indicada
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






void trazaCodigo(FILE *streamCodigo,FILE  *streamEscribir, int largo, FILE *streamLineasControl){

	//char instruccion[6];
	char *instruccion = malloc( sizeof(char) * (10) );
	//char rd[4];
	char *rd = malloc( sizeof(char) * (10) );
	//char rs[6];
	char *rs = malloc( sizeof(char) * (10) );
	//char rt[6];
	char *rt = malloc( sizeof(char) * (10) );
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



	while (indiceCode < largo){
	
	

	
		char *instruccionCompleta = malloc( sizeof(char) * (40) );
		strcpy(instruccion,arrCodigo[indiceCode]);
		grupo=verificarGrupo(instruccion);
		
		//printf("\nEl grupo es: %d\n",grupo);
		switch (grupo) {


			//Tipo R
			case 1:
				
				//Leer instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
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

				//Buscar instrucion en el archivo original 
				num= encontarInstruccion(streamCodigo,instruccionCompleta);
				//buscar instruccion en la linea de señales
				encontarLinea(streamLineasControl,num);

				//comparar lineas de control 
				fprintf(streamEscribir, "[%d]", num);
				compararLineasControl(streamEscribir,instruccion,streamLineasControl);


				//Hacer la operacion  
				
				//Depende si son de branch o si son de addi y/o subi
				addi=-1;
				subi=-1;
				addi=strcmp(instruccion,intrucR[4]);
				subi=strcmp(instruccion,intrucR[5]);
				if(addi==0 || subi==0){
					//pasar el string a int 
					rtC=atoi(rt);
					//printf("\nRTC: %d",rtC);	
				}
				else {
					//printf("\nRT: %s",rt);
				}
				

				//Realizar la intruccion

				//Obtener valores de registros
				indice=obtenerIndice(rs);
				valorrs=valoresRegistro[indice];
				
				//rt	
				//Si addi o subi
				if (addi==0 ||subi==0){

					valorrd=hacerOperacionR(instruccion,valorrs,rtC);
					indice=obtenerIndice(rd);
					valoresRegistro[indice]=valorrd;
		
				}
				//Si es instruccion R normal 
				else{

	    			rt[3]=',';
	    			rt[4]='\0';

	    			indice=obtenerIndice(rt);
					valorrt=valoresRegistro[indice];
					valorrd=hacerOperacionR(instruccion,valorrs,valorrt);
					indice=obtenerIndice(rd);
					valoresRegistro[indice]=valorrd;
					
				}		

				
				//imprimirRegistros(streamEscribir);

				printf("\n");
				indiceCode=indiceCode+4;
				free(instruccionCompleta);
				break;




			//Tipo I	
			case 2:
	
				
				//Leer instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
				//rd
				strcpy(rd,arrCodigo[indiceCode+1]);
				//rs
				strcpy(rs,arrCodigo[indiceCode+2]);
				//Label de branch
				strcpy(branch,arrCodigo[indiceCode+3]);

				//Juntar instruccion completa
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rd);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rs);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,branch);
				printf("\nInstruccion completa: %s",instruccionCompleta);

				//Buscar instrucion en el archivo original 
				num= encontarInstruccion(streamCodigo,instruccionCompleta);
				encontarLinea(streamLineasControl,num);

				//comparar lineas de control
				fprintf(streamEscribir, "[%d]", num);
				compararLineasControl(streamEscribir,instruccion,streamLineasControl);


				//Hacer operacion del Branch
				valorrd=valoresRegistro[obtenerIndice(rd)];
				valorrs=valoresRegistro[obtenerIndice(rs)];	
				boolean=hacerOperacionI(instruccion,valorrd,valorrs);



				//agregar los dos puntos a los label
				largoPalabra=strlen(branch);
				branch[largoPalabra]=':';
				branch[largoPalabra+1]='\0';

				//printf("\nLabel a buscar es: |%s|",branch);

				//si entra debe buscar el labels
					if (boolean == 1){
						indiceEncontrado=buscarLabelCodigo(branch,largo);
						indiceCode=indiceEncontrado+1;

					}
					//si NO cumple la condicion, saltar al branch 
					else if (boolean == 0){
						indiceCode=indiceCode+4;	
					}


				printf("\n");
				
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

				//Buscar instrucion en el archivo original 
				num= encontarInstruccion(streamCodigo,instruccionCompleta);
				encontarLinea(streamLineasControl,num);

				//comparar lineas de control
				fprintf(streamEscribir, "[%d]", num);
				compararLineasControl(streamEscribir,instruccion,streamLineasControl);

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


				
				printf("\n");
				free(instruccionCompleta);
				

				break;
			


			//Tipo M
			case 4:
				

				//Instruccion
				strcpy(instruccionCompleta,arrCodigo[indiceCode]);
				//rd
				strcpy(rd,arrCodigo[indiceCode+1]);
				//shampt
				strcpy(shampt,arrCodigo[indiceCode+2]);

				//Juntar instruccion
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,rd);
				strcat(instruccionCompleta," ");
				strcat(instruccionCompleta,shampt);
				printf("\nInstruccion completa: %s",instruccionCompleta);

				//Buscar instrucion en el archivo original 
				num= encontarInstruccion(streamCodigo,instruccionCompleta);
				encontarLinea(streamLineasControl,num);

				//comparar lineas de control
				fprintf(streamEscribir, "[%d]", num);
				compararLineasControl(streamEscribir,instruccion,streamLineasControl);

				
				int lw;
				int sw;
				lw=strcmp(instruccion,intrucM[0]);
				sw=strcmp(instruccion,intrucM[1]);

				//Hacer operacion de memoria 

				//recuperar registros
				//separar la parte del entero y la parte del registro
				int l=1;
				largoPalabra=strlen(shampt);
				//printf("\nEl ultimo elemto del SHAMPT es: %c",shampt[largoPalabra-1]);
				//strcpy(rm,shampt[largoPalabra-4]);
				rm[0]=shampt[largoPalabra-4];
				//strcat(rm,shampt[largoPalabra-3]);
				rm[1]=shampt[largoPalabra-3];
				//strcat(rm,shampt[largoPalabra-2]);
				rm[1]=shampt[largoPalabra-2];
				//añadir coma 
				rm[3]=',';
				rm[4]='\0';
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


				//lw 
				if (lw == 0){
					//desplazar el salto al medio del arreglo (stackPointer)
					saltoEntero=saltoEntero+200;
					valorrd=stackPointer[saltoEntero];
					indice=obtenerIndice(rd);
					valoresRegistro[indice]=valorrd;
					//printf("\nEl valor guardado en los registros es: %d",valoresRegistro[indice]);
				}
				else if (sw == 0){
					//desplazar el salto al medio del arreglo (stackPointer)
					saltoEntero=saltoEntero+200;
					indice=obtenerIndice(rd);
					valorrd=valoresRegistro[indice];
					stackPointer[saltoEntero]=valorrd;
					//printf("\nEl valor guardado en los registros es: %d",stackPointer[saltoEntero]);
				}


				printf("\n");
				
				indiceCode=indiceCode+3;
				free(instruccionCompleta);
				break;



			//Si la instruccion no esta dentro de las demas instrucciones 
			case -1:

				indiceCode=indiceCode+1;

				//fprintf(streamEscribir, "%s", "\n");
			
				break;

			

		} //switch-case

		
		finArchivo=feof(streamCodigo);

	} //while (fin archivo)
	
	
	free(instruccion);
	free(rd);
	free(rs);
	free(rt);
}





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




int main (){
	
	
	//Pedir nombre archivo entrada corespondiente al codigo de Mips
	char *nombreCodigo=(char*)malloc(sizeof(char*));
	//pedirNombreCodigo(nombreCodigo);
	FILE *archivoLectura;	
	//archivoLectura = fopen(nombreCodigo,"r");
	archivoLectura = fopen("mipsEjemplo1v2.asm","r");


	//Si no se encuentra 
	if (archivoLectura == NULL){
		printf("\nNo se encontro el archivo con el nombre: %s",nombreCodigo);
		return 0;
	}




	//Archivo de salida 

	//Pedir nombre archivo entrada corespondiente al codigo de Mips
	char *nombreSal=(char*)malloc(sizeof(char*));
	//pedirNombreSenales(nombreSal);
	FILE *archivoSal;	
	//archivoSal = fopen(nombreSal,"w");
	archivoSal = fopen("sal.txt","w");


	//Si no se encuentra 
	if (archivoSal == NULL){
		printf("\nNo se encontro el archivo con el nombre: %s",nombreSal);
		return 0;
	}



	//Se  Lee el codigo completo y guarda en un arreglo
	int largo=leerCodigo(archivoLectura);


	
	//archivo que posee las lineas de control
	FILE *archivoLineas;	
	archivoLineas = fopen("lineasControl1v2.txt","r");


	//**********************************************************************************************************************************+
	//
	//Encabezado archivo de salida 
	fprintf(archivoSal, "\t%s\tEstado\n","RegDst Jump   Branch MemRead MemToReg ALUOp MemWrite ALUSrc RegWrite ");
	//
	//Funcion principal para analizar el codigo y los dos archivos 
	trazaCodigo(archivoLectura,archivoSal,largo,archivoLineas);




	//printf("%s\n", );


	//FILE *archivoLectura;	
	//archivoLectura = fopen(nombre,"r");




	//Pedir nombre de entrada correspondiente al archivo que posee el nombre de 


	//*******************************************************************************************************************************************************
	
	//Se libera la memoria utilizada 

	//Liberan las cadenas de caracteres de los nombre de los archivos 

	//Se cierran los archivos leidos 
	fclose(archivoLineas);
	fclose(archivoLectura);
	fclose(archivoSal);



	return 0;
} 