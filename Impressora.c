#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
#include<conio.h>

#define QUANTIDADE_USUARIOS 3
#define APTO 1
#define SUSPENSO 2
#define ADICIONAR 1
#define REMOVER 2

struct struct_processo{
	int id; 
	int status;
	int remetente;
	int paginas;
	struct struct_processo *anterior;
	struct struct_processo *proximo;
};
typedef struct struct_processo PROCESSO;

struct struct_fila{
	struct struct_processo *inicio;
	struct struct_processo *fim;
	int tamanhoFila;
};
typedef struct struct_fila FILA;

struct struct_usuario{
	int id;
	char nome[30];
	int frequencia;
	FILA *filaProcessos;
};
typedef struct struct_usuario USUARIO;

FILA *filaImpressora = NULL;
FILA *processosSuspensos = NULL;
USUARIO usuario[QUANTIDADE_USUARIOS];
int indiceThread[QUANTIDADE_USUARIOS];
int idProcesso = 0;
int idEmImpressao;
bool fim;

pthread_t thread_usuario[QUANTIDADE_USUARIOS];
pthread_t thread_impressora;
pthread_t thread_interface;

void createThreads();
void joinThreads();
void *acaoImpressora();
void *acaoUsuario(void *posicao);
bool adicionarNaFila(FILA *fila, PROCESSO *novoNodo);
void consumirNodo(FILA *fila);
void exibirFila();
FILA buscarProcesso(FILA *fila, int id);
void removerProcesso(FILA *fila, int id);
void suspenderImpressao(FILA *fila, int id);
void suspenderImpressao();
void visualizarImpressora();
void *menu();
void inicializarUsuarios();

int main(){	
	fim = false;
	inicializarUsuarios();
	
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
	while(!fim){
		if(filaImpressora->tamanho>0){
			PROCESSO *processoAtual = filaImpressora;
			idEmImpressao = processoAtual->id;
			int tamanho = processoAtual->paginas;
		
			for(int i = 0; i<tamanho; i++){
				printf("Imprimindo processo #%d. Página %d de %d. \n", processoAtual->id, (i+1), tamanho);
			}
		
			printf("\nProcesso #%d impresso com sucesso! \n\n", processoAtual->id);
			consumirNodo();
			usleep(3);
		}
	}
	pthread_exit(0);
}

void *acaoUsuario(void *posicao){
	int num = *(int*) posicao;
	int tempo = usuario[num]->frequencia;
	
	while(!fim){
		PROCESSO *novoNodo = (PROCESSO*)malloc(sizeof(PROCESSO));
		novoNodo->id = idProcesso++;
		novoNodo->remetente = num;
		novoNodo->paginas = rand()%10;
		bool enviado = false;
		while(!enviado){
			enviado = adicionarFilaImpressora(usuario[num].filaProcessos, novoNodo, usuario[num]->filaProcessos->tamanhoFila);
		}
		usleep(tempo);
	}
	pthread_exit(0);
}

void adicionarNaFila(FILA *fila, PROCESSO *novoNodo){
	PROCESSO *ultimoNodo = fila->fim->anterior;
	if(fila->tamanhoFila==0){
		fila->inicio = novoNodo;
		fila->fim = novoNodo;
	}else{			
		ultimoNodo->proximo = novoNodo;
		novoNodo->anterior = ultimoNodo;
		novoNodo->proximo = ultimo->proximo;
	}
	fila->tamanhoFila++;
}

void removerDaFila(FILA *fila, int id){
	bool removido = false;
	PROCESSO *processo = buscarProcesso(filaImpressora, id);
	if(processo!=NULL){
		processo->anterior->proximo = processo->proximo;
        processo->proxmio->anterior = processo->anterior;
		removido = true;
	}
}

bool modificarFilaImpressora(int operacao, FILA *fila, PROCESSO *novoNodo){
	if(operacao==ADICIONAR){	
		bool enviado = false;
		pthread_mutex_lock(&mutex);
		adicionarNaFila(filaImpressora, novoProcesso);
		enviado = true;
		pthread_mutex_unlock(&mutex);
	
		return enviado;
	}else{
		bool removido = false;
		pthread_mutex_lock(&mutex);
		adicionarNaFila(filaImpressora, novoProcesso);
		removido = true;
		pthread_mutex_unlock(&mutex);
	
	return removido;
	}
}

bool modificarFilaSuspensos(FILA *fila, PROCESSO *novoNodo){
	if(operacao==ADICIONAR){
		bool enviado = false;
		pthread_mutex_lock(&mutex);
		adicionarNaFila(filaSuspensos, novoProcesso);
		enviado = true;
		pthread_mutex_unlock(&mutex);
	
		return enviado;
	}else{
		bool removido = false;
		pthread_mutex_lock(&mutex);
		adicionarNaFila(filaImpressora, novoProcesso);
		removido = true;
		pthread_mutex_unlock(&mutex);
	
	return removido;
	}
}

void consumirNodo(FILA *fila){
	if(fila->tamanhoFila>0){
		consumido = fila->inicio;
		novoInicio = consumido->proximo;
		novoInicio->anterior = consumido->anterior;
		fila->inicio = novoInicio;
		fila->tamanhoFila--;			
	}	
}

void exibirFila(){
	bool sair = false;
	char resposta = ' ';
	printf("--- FILA PARA IMPRESSÃO ---\n");
	while(!sair){
		PROCESSO *nodoAtual = filaImpressora;
		if(filaImpressora->tamanhoFila==0){
		    printf("Nenhum processo está aguardando impressão.");
		}else{
		    printf("- Processo #%d - ", nodoAtual->id);  
		    nodoAtual = nodoAtual->proximo;
		}
		printf("\nDigite X para voltar ao menu.\n");
		
		if(kbhit()){
	        char resposta = getch();
	        resposta = toupper(resposta);
	        if(resposta=='X'){
	        	sair = true;
	        }
	    }
	}
}

PROCESSO buscarProcesso(FILA *fila, int id){
	bool encontrado = false;
	PROCESSO *nodoAtual = fila->inicio;
    while(nodoAtual!=NULL & !encontrado){
        if(nodoAtual->id==idProcesso){
            encontrado = true;
        }else{
	        nodoAtual = nodoAtual->proximo;
	    }
    }
    
    return nodoAtual;
}

void removerProcesso(FILA *fila){
	int id;
	printf("ID do processo a ser removido da fila: ");
	scanf("%d", id);
	
	PROCESSO *processo = buscarProcesso(filaImpressora, id);
	if(processo!=NULL){
		processo->anterior->proximo = processo->proximo;
        processo->proxmio->anterior = processo->anterior;
   		printf("O processo #%d foi removido da fila para impressão.\n");
	}else{
		printf("O processo #%d não está na fila para impressão.\n");
	}
}

void suspenderImpressao(FILA *fila, int id){
	int id;
	printf("ID do processo a ser suspenso da fila: ");
	scanf("%d", id);
		
	PROCESSO *processo = buscarProcesso(fila, id);
	if(processo!=NULL){
		processo->anterior->proximo = processo->proximo;
        processo->proxmio->anterior = processo->anterior;
        bool enviado = false;
        printf("\n Aguarde... \n");
        while(!enviado){
	        enviado = adicionarFilaSuspensos(filaSuspensos, processo);
        }
   		printf("O processo #%d foi suspenso.\n");
	}else{
		printf("O processo #%d não está na fila para impressão.\n");
	}
}

void habilitarImpressao(FILA *fila, int id){
	int id;
	printf("ID do processo a ser habilitado: ");
	scanf("%d", id);
		
	PROCESSO *processo = buscarProcesso();
	if(processo!=NULL){
		processo->anterior->proximo = processo->proximo;
        processo->proxmio->anterior = processo->anterior;
        bool enviado = false;
        printf("\n Aguarde... \n");
        while(!enviado){
	        enviado = adicionarFilaImpressora(filaImpressora, processo);
        }
   		printf("O processo #%d foi habilitado.\n");
	}else{
		printf("O processo #%d não está suspenso.\n");
	}
}

void *menu(){
	char opcao = ' ';

	while(!fim){
		printf("IMPRESSORA - MENU \n");
		printf("(E) Exibir fila \n");
		printf("(R) Remover um processo da fila \n");
		printf("(S) Suspender impressão \n");
		printf("(H) Habilitar impressão suspensa \n");
		printf("(D)	Desligar a impressora \n");
	
		printf("Selecione uma opção válida: ");
		scanf("%c", &opcao);
		//comando de limpar a tela
		opcao = toupper(opcao);
		switch(opcao){
			case 'E':
				exibirFila();
				break;
			case 'R':
				
				break;
			case 'S':
				suspenderImpressao();
				break;
			case 'S':
				habilitarImpressao();
				break;
			case 'D':
				fim = true;
				printf("\n			A impressora está sendo desligada.\n");
				sleep(1);
				break;
			default:
				opcao = ' ';
				break;
		}
		printf("\n");
	}
	pthread_exit(0);
}

void inicializarUsuarios(){
	for(int i = 0; i<QUANTIDADE_USUARIOS; i++){
		usuario[i].id = i;
		usuario[i].nome = "User %d", i;
		usuario[i].frequencia = rand%3;
		usuario[i].filaProcessos = NULL;
		usuario[i].tamanhoFila = 0;
	}
}
