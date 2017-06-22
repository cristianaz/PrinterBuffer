#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define QUANTIDADE_CIDADES 3


struct struct_paciente;

PACIENTE *fila_inicio = NULL;
int tamanhoFila = 0;

void adicionarNodoNaFila(NODO *novoNodo);
void consumirNodo();

int main(){
	
 	
	return 0;
}

struct struct_paciente{
	int id; 
	char nome[30];
	char cidade[30];
	int idade;
	
	struct struct_nodo *proximo;
};
typedef struct struct_paciente PACIENTE;

void adicionarNodoNaFila(PACIENTE *novoNodo){
	PACIENTE *nodoAtual = fila_inicio;
	if(tamanhoFila==0){
		fila_inicio = novoNodo;
	}else{			
		while(nodoAtual->proximo!=NULL){
			nodoAtual = nodoAtual->proximo;
		}
		nodoAtual->proximo = novoNodo;
	}
	tamanhoFila++;
}

void consumirNodo(){
	pthread_mutex_lock(&mutex);
	if(tamanhoFila>0){
		fila_inicio = fila_inicio->proximo;
		tamanhoFila--;			
	}
	pthread_mutex_unlock(&mutex);		
}
