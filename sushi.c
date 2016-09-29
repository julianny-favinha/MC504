/* THE SUSHI BAR PROBLEM 
 * 
 * Igor Gustavo Hitzschky Lema - 155758
 * João Vítor Buscatto Silva - 155951
 * Julianny Favinha Donda - 156059
 * Ronaldo Prata Amorim - 157228
 * 
 * 2º SEMESTRE DE 2016 - MC504
 * 
 * Resumo: Imagine um bar de sushi com 5 lugares. Se você chegar quando
 * ainda existe um lugar vazio, você pode usá-lo. Mas se você chegar e 
 * os 5 lugares estiverem ocupados, isso significa que todas as 5 
 * pessoas estão comendo juntas. Dessa forma, você deve esperar todos 
 * irem embora para poder entrar no bar.  
 *TODO: SHUFFLE Randômico - http://stackoverflow.com/questions/6127503/shuffle-array-in-c
 *TODO: deslocar a mesa de sushi para a direita
 *TODO: introduzir cores
 */
 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_RESET  "\x1b[0m"
#define PRINT_RED(str)    printf("%s%s%s", ANSI_COLOR_RED, str, ANSI_COLOR_RESET)

#define SEATS 5
#define MAX_CLIENTES 15

/* mutex garante exclusão mútua e
 * block garante que só existem no máximo 5 pessoas dentro do bar */
sem_t mutex, block, filalock, entrarlock;

/* indica se existe espera ou não para entrar no bar */
bool espera = false;

/* contadores */
int esperando = 0, comendo = 0, total = 0;
int fila[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

// Vetor Cliente
// Coluna 1 - Caso 1, cliente presente
// Coluna 2 - posição do cliente
// - 1 -> saindo
// - 2 -> entrando
// - 3 -> comendo
// Coluna 3 - número de cliente
int clientes[5][3] = {
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
};

/* auxiliar */
int min(int x, int y) {
    return (x < y) ? x : y;
}

/////////////////////////////////////////////QUEUE////////////////////////////////////
void insereFila(int i){
	int aux1 = i, aux2;
	for(int h = 0; h < 10; h++){
		aux2 = fila[h];
		fila[h] = aux1;
		aux1 = aux2;		
	}
}

void removeFila(int i){
	for(int h = 9; h >= 0; h--){
		if(fila[h] == i){
			fila[h] = -1;
			return;
		}
	}
}



/////////////////////////////////////////////QUEUE////////////////////////////////////

int buscaCliente(int id){
	if(id == -1){ // busca primeira posição vaga
		for(int i = 0; i < 5; i ++){
			if(clientes[i][0] == 0){
				return i;
			}
		}
	}else{
		for(int i = 0; i < 5; i ++){
			if(clientes[i][2] == id){
				return i;
			}
		}
	}
}

void printMensagem(char s[]){
	char h[58] = "                                                         ";
	printf("%s", s);
	int i = strlen(h) - strlen(s);
	while(i >= 0){
		printf(" ");
		i--;
	}
}

void imprimeCabeca2(int i){
	if(i%2 == 0){
		printf("(◔ ◡ ◔)");
	}else{
		printf("(＾O＾)");
//		printf(" ༼ຈل͜ຈ༽ ");
	}
}

void imprimeCabeca(int i){
	if(fila[i] == -1){
		printf("        ");
	}else{
		imprimeCabeca2(i);
	}
}

void imprimeBraco(int i){
	if(fila[i] == -1){
		printf("           ");
	}else{
		if(fila[i] > 9)
			printf("/| %d|\\    ", fila[i]);
		else
			printf("/| %d |\\    ", fila[i]);
	}
}

void imprimeTronco(int i){
	if(fila[i] == -1){
		printf("           ");
	}else{
		printf(" |___|     ");
	}
}

void imprimePerna(int i){
	if(fila[i] == -1){
		printf("           ");
	}else{
		printf(" /   \\     ");
	}
}

void imprimeFila(){
	for(int i = 0; i < 10; i++){
		imprimeCabeca(i); printf("    ");
	}
	printf("\n");
	for(int i = 0; i < 10; i++)
		imprimeBraco(i);
	printf("\n");
	for(int i = 0; i < 10; i++)
		imprimeTronco(i);
	printf("\n");
	for(int i = 0; i < 10; i++)
		imprimePerna(i);
	printf("\n");
}

void imprimeCliente(int cliente){
	char ch[4] = {'|','|','|','|'};
	if(cliente == 2){
		ch[0] = 'E';
		ch[1] = 'X';
		ch[2] = 'I';
		ch[3] = 'T';
	}
	if(clientes[cliente][0] == 1){
		switch(clientes[cliente][1]){
			case 1:
				printf("%c   ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("                         |              |                                      |                            |\n");
				if(clientes[cliente][2] < 10){
					printf("%c   /| %d |\\                         |              |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}else{
					printf("%c   /| %d|\\                         |              |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}
				printf("%c    |___|	                    |              |                                      |                            |\n", ch[2]);
				printf("%c    /   \\	                    |              |                                      |                            |\n", ch[3]);
			break;
			
			case 2:
				printf("%c              ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("              |              |                                      |                            |\n");
				if(clientes[cliente][2] < 10){
					printf("%c              /| %d |\\              |              |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}else{
					printf("%c              /| %d|\\              |              |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}
				printf("%c               |___|               |              |                                      |                            |\n", ch[2]);
				printf("%c               /   \\               |              |                                      |                            |\n", ch[3]);
			break;
			
			case 3:
				printf("%c                         ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("   |   ,;'@@';,   |                                      |                            |\n");
				if(clientes[cliente][2] < 10){
					printf("%c                         /| %d |\\   |  |',_@@_,'|  |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}else{
					printf("%c                         /| %d|\\   |  |',_@@_,'|  |                                      |                            |\n", ch[1], clientes[cliente][2] );
				}
				printf("%c                          |___|    |  |        |  |                                      |                            |\n", ch[2]);
				printf("%c                          /   \\    |   '.____.'   |                                      |                            |\n", ch[3]);
			break;
		}
	}else{
		printf("%c                                   |              |                                      |                            |\n", ch[0]);
		printf("%c                                   |              |                                      |                            |\n", ch[1]);
		printf("%c                                   |              |                                      |                            |\n", ch[2]);
		printf("%c                                   |              |                                      |                            |\n", ch[3]);
	}
}

void exibe_bar(char s[]) {
	printf("\033[?1049h\033[H"); 
    printf("========================================================================================================================\n");
    printf("|");PRINT_RED(" ┌─┐┬ ┬┌─┐┬ ┬┌─┐┌─┐┌─┐┬┬ "); printf("|        Clientes Esperando: %d   |   Clientes Atendidos: %d   |   Clientes no Bar: %d   \n", esperando, total, comendo);
    printf("|");PRINT_RED(" └─┐│ │└─┐├─┤├─┤└─┐│  ││"); printf(" |---------------------------------------------------------------------------------------------\n");
    printf("|");PRINT_RED(" └─┘└─┘└─┘┴ ┴┴ ┴└─┘└─┘┴┴ ");printf("|"); printMensagem(s);printf("\n");
    printf("========================================================================================================================\n");
    imprimeCliente(0);
    printf("|                                   |              |                                      |                            |\n");
    imprimeCliente(1);
    printf("=                                   |              |                                      |                            |\n");
    imprimeCliente(2);
    printf("=                                   |              |                                      |                            |\n");
    imprimeCliente(3);
    printf("|                                   |              |                                      |                            |\n");
    imprimeCliente(4);
    printf("============| ENTRANCE |================================================================================================\n");
    printf("                                                                                                                        \n");
    imprimeFila();
    printf("                                                                                                                        \n");
    printf("========================================================================================================================\n");
    printf("\033[37A");
    sleep(2);
}

void* f_cliente(void *v) {
    int i, n, cli_id = *(int *) v;
    int teste = 0;

    char s[80];
    
    // cliente entrou no bar e começa a comer
	// wait lock de queue cheio
    sleep(4);
	sem_wait(&entrarlock);
	sem_wait(&mutex);
	if(espera) {
        esperando++;
        snprintf(s, sizeof(s), "  Cliente %d está esperando na porta.  ", cli_id);
        sem_wait(&filalock);
        insereFila(cli_id);
        exibe_bar(s);
        sem_post(&filalock);
        sem_post(&mutex);
        sem_post(&entrarlock);
        sleep(2);
	    sem_wait(&block); // espera todos os lugares liberarem
	    sem_wait(&entrarlock);
	    teste = 1;
    }else{
		if(esperando > 0)
			esperando --;
		comendo++;
		total++; 	
		espera = (comendo == 5);
		sem_post(&mutex);
	}
	// post lock de queue cheio

    int posicao = buscaCliente(-1);
    clientes[posicao][0] = 1;
    clientes[posicao][1] = 2;
    clientes[posicao][2] = cli_id;
    snprintf(s, sizeof(s), "  Cliente %d entrou no bar.  ", cli_id);
    if(teste){
	    sem_wait(&filalock);
        removeFila(cli_id);
	    exibe_bar(s);
	    sem_post(&filalock);
	}else{
		exibe_bar(s);
	}
    sem_post(&entrarlock);
    sleep(2);
    
    //comecou a comer
    sem_wait(&entrarlock);
    snprintf(s, sizeof(s), "  Cliente %d começou a comer.  ", cli_id);
    posicao = buscaCliente(cli_id);
    clientes[posicao][1] = 3;
    exibe_bar(s);
    sem_post(&entrarlock);
    sleep(4);

    // acabou de comer
    sem_wait(&entrarlock);
    snprintf(s, sizeof(s), "  Cliente %d está indo embora.", cli_id);
    posicao = buscaCliente(cli_id);
    clientes[posicao][1] = 1;
    exibe_bar(s);
    sleep(1);
    clientes[posicao][0] = 0;

	sem_wait(&mutex);
    comendo--;    
    if (comendo == 0) {
        snprintf(s, sizeof(s), "  Cliente %d avisou que está vazio!", cli_id);
	    exibe_bar(s);
        n = min(5, esperando); // n é 5 ou é a qtd de clientes que estava esperando na porta
        esperando -= n;
        comendo = n;
        espera = (comendo == 5);
        sem_post(&mutex);
        sem_post(&entrarlock);
        sleep(2);
        for (i = 0; i <  n; i++)
			sem_post(&block);
    }else{
		exibe_bar(s);
		sem_post(&mutex);
		sem_post(&entrarlock);
		sleep(2);
    }
    
    return (void*) v;
}

int main(){
    pthread_t thr[MAX_CLIENTES];
    int i;

    sem_init(&mutex, 0, 1); // iniciando mutex
    sem_init(&block, 0, 0); // iniciando block
    sem_init(&filalock, 0, 1);
    sem_init(&entrarlock, 0, 1);

    // exibe bar inicial
    exibe_bar("  Bar Aberto!");
    sleep(2);

    // iniciando as threads de todos os clientes
    /// ****** TODO: como fazer uma ordem aleatória de clientes? ******* ///
    for(i = 1; i <= MAX_CLIENTES; i++){
        pthread_create(&thr[i-1], NULL, f_cliente, (void *) &i);
        sleep(1); // espera 1 segundo pra cada cliente entrar 
    }

    // espera todos os clientes comerem
    for(i = 0; i < MAX_CLIENTES; i++) 
        pthread_join(thr[i], NULL);

    return 0;
}
