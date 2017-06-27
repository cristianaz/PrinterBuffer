#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>

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
int idEmImpressao = 0;
bool fim = false;

pthread_t thread_usuario[QUANTIDADE_USUARIOS];
pthread_t thread_impressora;
pthread_t thread_interface;

pthread_mutex_t mutexSuspensos;
pthread_mutex_t mutexImpressora;

void *acaoImpressora();
void *acaoUsuario(void *posicao);
void adicionarNaFila(FILA *fila, PROCESSO *novoNodo);
void removerDaFila(FILA *fila, int id);
bool modificarFilaImpressora(int operacao, PROCESSO *nodo);
bool modificarFilaSuspensos(int operacao, PROCESSO *nodo);
void consumirNodo(FILA *fila);
void exibirFila();
void removerProcesso(FILA *fila);
void suspenderImpressao(FILA *fila);
void habilitarImpressao(FILA *fila);
void *menu();
void inicializarUsuarios();
void inicializarFilas();
void exibirSaidas();

int main(){	
	fim = false;
	inicializarUsuarios();
	inicializarFilas();
	
	pthread_t thread_usuario[QUANTIDADE_USUARIOS];
	pthread_t thread_impressora;
	pthread_t thread_controle;
	
	pthread_mutex_init(&mutexImpressora, NULL);
	pthread_mutex_init(&mutexSuspensos, NULL);
	
	void *thread_result;
	
	(void)pthread_create(&thread_controle, NULL, menu, 0);
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
		if(filaImpressora->tamanhoFila>0){
			PROCESSO *processoAtual = filaImpressora->inicio;
			idEmImpressao = processoAtual->id;
			int tamanho = processoAtual->paginas;
		
			for(int i = 0; i<tamanho & processoAtual->status==APTO; i++){
				sleep(1);
			}			
			consumirNodo(filaImpressora);
		}
	}
	pthread_exit(0);
}

void *acaoUsuario(void *posicao){
	int num = *(int*) posicao;
	int tempo = usuario[num].frequencia;
	
	while(!fim){
		PROCESSO *novoNodo = (PROCESSO*)malloc(sizeof(PROCESSO));
		novoNodo->id = -1;
		novoNodo->status = APTO;
		novoNodo->remetente = num;
		novoNodo->paginas = rand()%10;
		bool enviado = false;
		while(!enviado){
			enviado = modificarFilaImpressora(ADICIONAR, novoNodo);
		}
		usleep(tempo*1000000);
	}
	pthread_exit(0);
}

void adicionarNaFila(FILA *fila, PROCESSO *novoNodo){
	PROCESSO *ultimoNodo = fila->fim;
	if(novoNodo->id==-1){
		idProcesso++;
		novoNodo->id = idProcesso;
	}	
	if(fila->tamanhoFila==0){
		novoNodo->anterior = NULL;
		novoNodo->proximo = NULL;
		fila->inicio = novoNodo;
		fila->fim = novoNodo;
	}else{			
		ultimoNodo->proximo = novoNodo;
		novoNodo->anterior = ultimoNodo;
		novoNodo->proximo = NULL;
		fila->fim = novoNodo;
	}
	fila->tamanhoFila++;
}

void removerDaFila(FILA *fila, int id){
	bool removido = false;
	
	PROCESSO *processo = fila->inicio;
	bool encontrado = false;
    while(processo!=NULL & !encontrado){
        if(processo->id==id){
            encontrado = true;
        }else{
	        processo = processo->proximo;
	    }
    }
	
	if(encontrado){
		processo->anterior->proximo = processo->proximo;
        processo->proximo->anterior = processo->anterior;
		removido = true;
	}
}

bool modificarFilaImpressora(int operacao, PROCESSO *nodo){
	if(operacao==ADICIONAR){	
		bool enviado = false;
		while(!enviado){
			pthread_mutex_lock(&mutexImpressora);
			adicionarNaFila(filaImpressora, nodo);
			enviado = true;
			pthread_mutex_unlock(&mutexImpressora);
		}
		return enviado;
	}else{
		bool removido = false;
		while(!removido){
			pthread_mutex_lock(&mutexImpressora);
			removerProcesso(filaImpressora);
			removido = true;
			pthread_mutex_unlock(&mutexImpressora);
		}
	return removido;
	}
}

bool modificarFilaSuspensos(int operacao, PROCESSO *nodo){
	if(operacao==ADICIONAR){
		bool enviado = false;
		while(!enviado){
			pthread_mutex_lock(&mutexSuspensos);
			adicionarNaFila(processosSuspensos, nodo);
			enviado = true;
			pthread_mutex_unlock(&mutexSuspensos);
		}
		return enviado;
	}else{
		bool removido = false;
		while(!removido){
			pthread_mutex_lock(&mutexImpressora);	
			adicionarNaFila(filaImpressora, nodo);
			removido = true;
			pthread_mutex_unlock(&mutexImpressora);
		}
		return removido;
	}
}

void consumirNodo(FILA *fila){
	PROCESSO *consumido = fila->inicio;
	PROCESSO *novoInicio = consumido->proximo;
	novoInicio->anterior = NULL;
	fila->inicio = novoInicio;
	fila->tamanhoFila--;
}

void exibirFila(){
	bool sair = false;
	char resposta = ' ';
	printf("--- FILA PARA IMPRESSÃO ---\n");
	while(!sair){
		PROCESSO *nodoAtual = filaImpressora->inicio;
		if(filaImpressora->tamanhoFila==0){
		    printf("Nenhum processo está aguardando impressão.");
		}else{
			while(nodoAtual!=NULL){
				printf("- Processo #%d - Usuário: %d \n", nodoAtual->id, nodoAtual->remetente);  
				nodoAtual = nodoAtual->proximo;
			}
		}
		printf("\nDigite X para voltar ao menu.\n");
		
		scanf(" %c", &resposta);
		resposta = toupper(resposta);
		if(resposta == 'X'){
			sair = true;
		}		
		system("clear");
	}
}

void removerProcesso(FILA *fila){
	int id;
	printf("ID do processo a ser removido da fila: ");
	scanf("%d", &id);
	
	PROCESSO *processo = fila->inicio;
	bool encontrado = false;
    while(processo!=NULL & !encontrado){
        if(processo->id==id){
            encontrado = true;
        }else{
	        processo = processo->proximo;
	    }
    }
	
	if(encontrado){
		processo->anterior->proximo = processo->proximo;
        processo->proximo->anterior = processo->anterior;
   		printf("O processo #%d foi removido da fila para impressão.\n", id);
	}else{
		printf("O processo #%d não está na fila para impressão.\n", id);
	}
}

void suspenderImpressao(FILA *fila){
	int id;
	printf("ID do processo a ser suspenso da fila: ");
	scanf("%d", &id);
		
	PROCESSO *processo = fila->inicio;
	bool encontrado = false;
    while(processo!=NULL & !encontrado){
        if(processo->id==id){
            encontrado = true;
        }else{
	        processo = processo->proximo;
	    }
    }
	
	if(encontrado){
		modificarFilaImpressora(REMOVER, processo);
        bool enviado = false;
        printf("\n Aguarde... \n");
        while(!enviado){
	        enviado = modificarFilaSuspensos(ADICIONAR, processo);
        }
        processo->status = SUSPENSO;
   		printf("O processo #%d foi suspenso.\n", id);
	}else{
		printf("O processo #%d não está na fila para impressão.\n", id);
	}
}

void habilitarImpressao(FILA *fila){
	int id;
	printf("ID do processo a ser habilitado: ");
	scanf("%d", &id);
	
	PROCESSO *processo = fila->inicio;
	bool encontrado = false;
    while(processo!=NULL & !encontrado){
        if(processo->id==id){
            encontrado = true;
        }else{
	        processo = processo->proximo;
	    }
    }
    
	if(encontrado){
		processo->anterior->proximo = processo->proximo;
        processo->proximo->anterior = processo->anterior;
        bool enviado = false;
        printf("\n Aguarde... \n");
        while(!enviado){
	        enviado = modificarFilaSuspensos(REMOVER, processo);
        }
        processo->status = APTO;
   		printf("O processo #%d foi habilitado.\n", id);
	}else{
		printf("O processo #%d não está suspenso.\n", id);
	}
}

void *menu(){
	char opcao = ' ';

	while(!fim){
		printf("\n\nIMPRESSORA - MENU \n");
		printf("(E) Exibir fila \n");
		printf("(R) Remover um processo da fila \n");
		printf("(S) Suspender impressão \n");
		printf("(H) Habilitar impressão suspensa \n");
		printf("(O) Exibir saidas \n");
	
		printf("Selecione uma opção válida: ");
		scanf("%c", &opcao);
		system("clear");
		opcao = toupper(opcao);
		switch(opcao){
			case 'E':
				exibirFila();
				system("clear");
				break;
			case 'R':
				removerProcesso(filaImpressora);
				system("clear");
				break;
			case 'S':
				suspenderImpressao(processosSuspensos);
				system("clear");
				break;
			case 'H':
				habilitarImpressao(filaImpressora);
				system("clear");
				break;
			case 'O':
				exibirSaidas();
				break;
			default:
				opcao = ' ';
				system("clear");
				break;
		}
		printf("\n");
	}
	pthread_exit(0);
}

void inicializarUsuarios(){
	for(int i = 0; i<QUANTIDADE_USUARIOS; i++){
		usuario[i].id = i;
		strcpy(usuario[i].nome, "User");
		usuario[i].frequencia = rand()%3+1;
		usuario[i].filaProcessos = NULL;
	}
}

void inicializarFilas(){
	FILA *novaFila = (FILA*)malloc(sizeof(FILA));
	novaFila->inicio = NULL;
	novaFila->fim = NULL;
	novaFila->tamanhoFila = 0;
	filaImpressora = novaFila;
	
	FILA *novaFila2 = (FILA*)malloc(sizeof(FILA));
	novaFila2->inicio = NULL;
	novaFila2->fim = NULL;
	novaFila2->tamanhoFila = 0;
	processosSuspensos = novaFila2;
}

void exibirSaidas(){
	PROCESSO *processoAtual = filaImpressora->inicio;
	int tamanho = processoAtual->paginas;

	for(int i = 0; i<tamanho & processoAtual->status==APTO; i++){
		printf("Imprimindo processo #%d do usuário %d. Página %d de %d. \n", processoAtual->id, processoAtual->remetente, (i+1), tamanho);
		sleep(1);
	}

	printf("\n Processo #%d impresso com sucesso! \n\n", processoAtual->id);
}
