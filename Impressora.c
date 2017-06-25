#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define QUANTIDADE_USUARIOS 3

struct struct_requisicao{
	int id; 
	int remetente;
	int paginas;	
	struct struct_requisicao *proximo;
};
typedef struct struct_requisicao REQUISICAO;

struct struct_usuario{
	int id;
	char nome[30];
	REQUISICAO *filaProcessos;
	int tamanhoFila;
};
typedef struct struct_usuario USUARIO;

REQUISICAO *filaImpressora = NULL;
REQUISICAO *filaUsuario = NULL;
USUARIO usuario[QUANTIDADE_USUARIOS];
int tamanhoFilaImpressora = 0;
int indiceThread[QUANTIDADE_USUARIOS];
int idProcesso;
int idEmImpressao;

pthread_t thread_usuario[QUANTIDADE_USUARIOS];
pthread_t thread_impressora;
pthread_t thread_interface;

void createThreads();
void joinThreads();
void *acaoImpressora();
void *acaoUsuario(void *posicao);
void adicionarNodoPrioritario(REQUISICAO *novoNodo);
void adicionarNodo(REQUISICAO *novoNodo);
void consumirNodo();
void exibirFila();
bool retirarDaFila();
void suspenderImpressao();
void visualizarImpressora();
void *menu();

int main(){	
	
	pthread_t thread_usuario[QUANTIDADE_USUARIOS];
	pthread_t thread_impressora;
	pthread_t thread_controle;
	
	void *thread_result;
	
	(void)pthread_create(&thread_controle, NULL, menu, 0); //pesquisar sobre o quarto parâmetro
	(void)pthread_create(&thread_impressora, NULL, acaoImpressora, 0);
	
	for(int i = 0; i<QUANTIDADE_USUARIOS; i++){
		indiceThread[i] = i;
		(void)pthread_create(&thread_usuario[i], NULL, acaoUsuario, &indiceThread[i]);
	}

	(void)pthread_join(thread_controle, &thread_result);
	(void)pthread_join(thread_impressora, &thread_result);
	
	for(int i = 0; i<QUANTIDADE_USUARIOS; i++){
		(void)pthread_join(thread_usuario[i], &thread_result);
	}
	
	return 0;
}

void *acaoImpressora(){
	if(tamanhoFilaImpressora>0){
		REQUISICAO *processoAtual = filaImpressora;
		int tamanho = processoAtual->paginas;
		
		for(int i = 0; i<tamanho; i++){
			printf("Imprimindo processo #%d. Página %d de %d. \n", processoAtual->id, (i+1), tamanho);
		}
		printf("\nProcesso #%d impresso com sucesso! \n\n", processoAtual->id);
		consumirNodo();
	}
}

void *acaoUsuario(void *posicao){
	int num = *(int*) posicao;
	int novoProcesso = rand()%10;
	
}

void adicionarNodoPrioritario(REQUISICAO *novoNodo){
	REQUISICAO *nodoAtual = filaImpressora;
	
    if(tamanhoFilaImpressora==0){
        filaImpressora = novoNodo;
    }else{
        novoNodo->proximo = nodoAtual;        
        filaImpressora = novoNodo;        
        nodoAtual = novoNodo->proximo;
    }
}


void adicionarNodo(REQUISICAO *novoNodo){
	REQUISICAO *nodoAtual = filaImpressora;
	if(tamanhoFilaImpressora==0){
		filaImpressora = novoNodo;
	}else{			
		while(nodoAtual->proximo!=NULL){
			nodoAtual = nodoAtual->proximo;
		}
		nodoAtual->proximo = novoNodo;
	}
	tamanhoFilaImpressora++;
}

void consumirNodo(){
	if(tamanhoFilaImpressora>0){
		filaImpressora = filaImpressora->proximo;
		tamanhoFilaImpressora--;			
	}	
}

void exibirFila(){
	bool sair = false;
	char resposta = ' ';
	while(!sair){
		REQUISICAO *nodoAtual = filaImpressora;
		printf("FILA PARA IMPRESSÃO");
		if(tamanhoFilaImpressora==0){
		    printf("Nenhum processo até aguardando impressão");
		}else{
		    printf("- Processo #%d - ", nodoAtual->id);
		          
		    nodoAtual = nodoAtual->proximo;
		}
	}
}

bool retirarDaFila(){
	bool encontrado = false;
	REQUISICAO *nodoAtual = filaImpressora;
	REQUISICAO *nodoAnterior = NULL;
    while(nodoAtual!=NULL & !encontrado){
        if(nodoAtual->id==idProcesso){
        	nodoAnterior = nodoAtual->proximo;
            encontrado = true;
        }
        nodoAnterior = nodoAtual;
        nodoAtual = nodoAtual->proximo;
    }
    
    return encontrado;
}

void suspenderImpressao(){
	
}

void visualizarImpressora(){
	
}

void *menu(){
	char opcao = ' ';

	while(opcao=' '){
		printf("IMPRESSORA - MENU \n");
		printf("(F) Exibir fila \n");
		printf("(R) Remover um processo da fila \n");
		printf("(S) Suspender impressão \n");
		printf("(D)	Desligar a impressora \n");
	
		printf("Selecione uma opção válida: ");
		scanf("%c", &opcao);
		switch(opcao){
			case 'F':
				
				break;
			case 'R':
				
				break;
			case 'S':
				
				break;
			case 'D':
				
				break;
			default:
				//comando de limpar a tela
				opcao = ' ';
				break;
		}
	}
}
