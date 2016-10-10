/* THE SUSHI BAR PROBLEM 
 * 
 * Igor Gustavo Hitzschky Lema - 155758
 * João Vítor Buscatto Silva - 155951
 * Julianny Favinha Donda - 156059
 * Ronaldo Prata Amorim - 157228
 * 
 * 2º SEMESTRE DE 2016 - MC504 - Profa. Islene Calciolari Garcia
 * 
 * Resumo: Imagine um bar de sushi com 5 lugares. Se você chegar quando
 * ainda existe um lugar vazio, você pode usá-lo. Mas se você chegar e 
 * os 5 lugares estiverem ocupados, isso significa que todas as 5 
 * pessoas estão comendo juntas. Dessa forma, você deve esperar todos 
 * irem embora para poder entrar no bar.  
 * 
 */
 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

/* definição de cores para serem utilizadas no terminal */
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RESET  "\x1b[0m"
#define PRINT_RED(str)    printf("%s%s%s", ANSI_COLOR_RED, str, ANSI_COLOR_RESET)
#define PRINT_GREEN(str)    printf("%s%s%s", ANSI_COLOR_GREEN, str, ANSI_COLOR_RESET)
#define PRINT_YELLOW(str) printf("%s%s%s", ANSI_COLOR_YELLOW, str, ANSI_COLOR_RESET)
#define PRINT_BLUE(str)   printf("%s%s%s", ANSI_COLOR_BLUE, str, ANSI_COLOR_RESET)

/* total de cadeiras no bar */
#define SEATS 5
 
/* eventos que cada cliente pode executar, em ordem */
typedef enum {
    aguardo,
    entra,
    come,
    sai,
    vazio
} evento;

/* mutex garante exclusão mútua e
 * porta garante que só existem no máximo 5 pessoas dentro do bar
 * imprimelock lock para impressão da animação */
sem_t porta;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t imprime = PTHREAD_MUTEX_INITIALIZER;

/* indica se existe espera ou não para entrar no bar */
volatile bool espera = false;

/* contadores */
volatile int MAX_CLIENTES = 0;
volatile int dentro = 0, comendo = 0, total = 0, chef = 0;
volatile int fila[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

// Vetor clientes
// Coluna 1 - caso 1, cliente presente
// Coluna 2 - posição do cliente
// - 1 -> saindo
// - 2 -> entrando
// - 3 -> comendo
// Coluna 3 - número de cliente
int clientes[5][3] = {
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

/* retorna o menor valor dentre x e y */
int min(int x, int y) {
    return (x < y) ? x : y;
}

/* busca posição vaga no vetor Clientes */
int buscaCliente(int id) {
    int i; 
    
    if (id == -1) { // busca primeira posição vaga
        for (i = 0; i < 5; i ++)
            if (clientes[i][0] == 0)
                return i;
    } else {
        for (i = 0; i < 5; i ++)
            if (clientes[i][2] == id)
                return i;
    }
    
    return -1;
}

/////////////////////////////////////////////QUEUE////////////////////////////////////
/* insere i no início do vetor fila */
void insereFila(int i) {
    int h, pos_i = i, aux;
    
    for (h = 0; h < 10; h++) {
        aux = fila[h];
        fila[h] = pos_i;
        pos_i = aux;        
    }
}

/* remove cliente i da fila */
void removeFila(int i) {
    int h;
    
    for (h = 9; h >= 0; h--) { 
        if (fila[h] == i) {
            fila[h] = -1;
            return;
        }
    }
}

/* verifica se cliente i está na fila */
int existeFila(int i) {
    int h;

    for (h = 0; h < 10; h++) {
        if (fila[h] == i)
            return 1;
    }
    return 0;
}

/* quantidade de pessoas na fila */
int presentesFila() {
    int h, total = 0;

    for (h = 0; h < 10; h++)
        if (fila[h] != -1)
            total++;

    return total;
}

/////////////////////////////////////////////PRINTING//////////////////////////////////
void imprimeCabeca2(int i) {
    if (i%3 == 0) {
        printf("(");PRINT_YELLOW("◔ ");printf("◡");PRINT_YELLOW(" ◔");printf(")");
    } else if (i%3 == 1) {
        printf("(");PRINT_YELLOW("＾");printf("O");PRINT_YELLOW("＾");printf(")");
    } else {
        printf("(");PRINT_YELLOW(" ¬");printf(".");PRINT_YELLOW("¬ ");printf(")");
    }
}

void imprimeCabeca(int i) {
    if (fila[i] == -1)
        printf("       ");
    else
        imprimeCabeca2(i);
}

void imprimeBraco(int i) {
    if (fila[i] == -1) {
        printf("           ");
    } else {
        if (fila[i] > 9)
            printf("/| %d|\\    ", fila[i]);
        else
            printf("/| %d |\\    ", fila[i]);
    }
}

void imprimeTronco(int i) { 
    if (fila[i] == -1)
        printf("           ");
    else
        printf(" |___|     ");
}

void imprimePerna(int i) {
    if (fila[i] == -1)
        printf("           ");
    else
        printf(" /   \\     ");
}

void imprimeSMCabeca(int i) {
	if (chef == i) {
		printf("  ==");PRINT_RED("@");printf("==  ");
	} else {
		printf("         ");
	}
}

void imprimeSMBraco(int i) {
	if (chef == i) {
		printf("A (");PRINT_YELLOW("-");printf(" ^ ");PRINT_YELLOW("-");printf(") A");
	} else {
		printf("           ");
	}
}

void imprimeSMTronco(int i) {
	if (chef == i)
		printf("\\/|   |\\/");
	else
		printf("         ");
}

void imprimeSMPerna(int i) {
	if (chef == i)
		printf("  |   |  ");
	else
		printf("         ");
}

void imprimeFila() {
    int i;
    
    for (i = 0; i < 10; i++) {
        imprimeCabeca(i); printf("    ");
    }
    printf("\n");
    for (i = 0; i < 10; i++)
        imprimeBraco(i);
    printf("\n");
    for (i = 0; i < 10; i++)
        imprimeTronco(i);
    printf("\n");
    for (i = 0; i < 10; i++)
        imprimePerna(i);
    printf("\n");
}

void imprimeCliente(int cliente) {
    char ch[4] = {'|','|','|','|'};

    if (cliente == 2) {
        ch[0] = 'E';
        ch[1] = 'X';
        ch[2] = 'I';
        ch[3] = 'T';
    }
    if (clientes[cliente][0] == 1) { 
        switch (clientes[cliente][1]) {
            case 1:
                printf("%c    ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("                                              |              |    ");imprimeSMCabeca(cliente);printf("                                |\n");
                if (clientes[cliente][2] < 10) {
                    printf("%c    /| %d |\\                                              |              |   ", ch[1], clientes[cliente][2]);imprimeSMBraco(cliente);printf("                               |\n");
                } else {
                    printf("%c    /| %d|\\                                              |              |   ", ch[1], clientes[cliente][2]);imprimeSMBraco(cliente);printf("                               |\n");
                }
                printf("%c     |___|                                               |              |    ", ch[2]);imprimeSMTronco(cliente);printf("                                |\n");
                printf("%c     /   \\                                               |              |    ", ch[3]);imprimeSMPerna(cliente);printf("                                |\n");
                break;
            
            case 2:
                printf("%c                         ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("                         |              |    ");imprimeSMCabeca(cliente);printf("                                |\n");
                if (clientes[cliente][2] < 10) {
                    printf("%c                         /| %d |\\                         |              |   ", ch[1], clientes[cliente][2]);imprimeSMBraco(cliente);printf("                               |\n");
                } else {
                    printf("%c                         /| %d|\\                         |              |   ", ch[1], clientes[cliente][2]);imprimeSMBraco(cliente);printf("                               |\n");
                }
                printf("%c                          |___|                          |              |    ", ch[2]);imprimeSMTronco(cliente);printf("                                |\n");
                printf("%c                          /   \\                          |              |    ", ch[3]);imprimeSMPerna(cliente);printf("                                |\n");
                break;
            
            case 3:
                printf("%c                                              ", ch[0]);imprimeCabeca2(clientes[cliente][2]);printf("    |   ,;'");PRINT_RED("@@");printf("';,   |    ");imprimeSMCabeca(cliente);printf("                                |\n");
                if (clientes[cliente][2] < 10) {
                	printf("%c                                              /| %d |\\    |  |',_", ch[1], clientes[cliente][2]);PRINT_RED("@");PRINT_GREEN("@");printf("_,'|  |   ");imprimeSMBraco(cliente);printf("                               |\n");
                } else {
                    printf("%c                                              /| %d|\\    |  |',_", ch[1], clientes[cliente][2]);PRINT_RED("@");PRINT_GREEN("@");printf("_,'|  |   ");imprimeSMBraco(cliente);printf("                               |\n");
                }
                printf("%c                                               |___|     |  |        |  |    ", ch[2]);imprimeSMTronco(cliente);printf("                                |\n");
                printf("%c                                               /   \\     |   '.____.'   |    ", ch[3]);imprimeSMPerna(cliente);printf("                                |\n");
                break;
        }
    } else {
        printf("%c                                                         |              |    ", ch[0]);imprimeSMCabeca(cliente);printf("                                |\n");
        printf("%c                                                         |              |   ",  ch[1]); imprimeSMBraco(cliente); printf("                               |\n");
        printf("%c                                                         |              |    ", ch[2]);imprimeSMTronco(cliente);printf("                                |\n");
        printf("%c                                                         |              |    ", ch[3]); imprimeSMPerna(cliente);printf("                                |\n");
    }
}

void exibe_bar(char mensagem[]) {
    printf("\033[?1049h\033[H"); 
    printf("========================================================================================================================\n");
    printf("|");PRINT_BLUE(" ┌─┐┬ ┬┌─┐┬ ┬┌─┐┌─┐┌─┐┬┬ "); printf("|        Clientes Esperando: %d   |   Clientes Atendidos: %d   |   Clientes no Bar: %d\n", presentesFila(), total, dentro);
    printf("|");PRINT_BLUE(" └─┐│ │└─┐├─┤├─┤└─┐│  ││"); printf(" |---------------------------------------------------------------------------------------------\n");
    printf("|");PRINT_BLUE(" └─┘└─┘└─┘┴ ┴┴ ┴└─┘└─┘┴┴ ");printf("|"); printf("%s\n", mensagem);
    printf("========================================================================================================================\n");
    imprimeCliente(0);
    printf("|                                                         |              |                                             |\n");
    imprimeCliente(1);
    printf("=                                                         |              |                                             |\n");
    imprimeCliente(2);
    printf("=                                                         |              |                                             |\n");
    imprimeCliente(3);
    printf("|                                                         |              |                                             |\n");
    imprimeCliente(4);
    printf("=======================| ENTRANCE |=====================================================================================\n");
    printf("                                                                                                                        \n");
    imprimeFila();
    printf("                                                                                                                        \n");
    printf("========================================================================================================================\n");
    printf("\033[37A");
    sleep(1);
}

/////////////////////////////////////////////EVENTS////////////////////////////////////
void acao(evento tipo, int cliente) {
    char s[80];
    int posicao;

    // cliente quer imprimir algo. espera mutex imprime
    pthread_mutex_lock(&imprime);

    // efetua o que o cliente declarou que quer fazer
    switch (tipo){
        case aguardo:
            snprintf(s, sizeof(s), "  Cliente %d está esperando na porta.  ", cliente);
            insereFila(cliente);
            break;

        case entra:
	    	dentro++;
            posicao = buscaCliente(-1);
            clientes[posicao][0] = 1;
            clientes[posicao][1] = 2;
            clientes[posicao][2] = cliente;
            snprintf(s, sizeof(s), "  Cliente %d entrou no bar.  ", cliente);
            if (existeFila(cliente))
				removeFila(cliente);
            break;
        
        case come:
            snprintf(s, sizeof(s), "  Cliente %d começou a comer.  ", cliente);
            total++;
            posicao = buscaCliente(cliente);
            clientes[posicao][1] = 3;
            chef = posicao;
            break;

        case sai:
            snprintf(s, sizeof(s), "  Cliente %d está indo embora.", cliente);
            posicao = buscaCliente(cliente);
            clientes[posicao][1] = 1;
            break;

        case vazio:
        	chef = rand()%5;
            snprintf(s, sizeof(s), "  Cliente %d avisou que está vazio!", cliente);
            break;
    }

    exibe_bar(s);

    if(tipo == sai) {
		dentro--;
        clientes[posicao][0] = 0;
    }

    pthread_mutex_unlock(&imprime);
}

void* f_cliente(void *v) {
    int i, n, cli_id = *(int *) v;

    // cliente entrou no bar para comer. wait lock de queue cheio
    pthread_mutex_lock(&mutex);

    // alternância para evitar que threads novas impeçam as presentes de sair do bar (pelo mutex imprime)
    if (presentesFila() > 5) {
        pthread_mutex_unlock(&mutex);
        sleep(6);
        pthread_mutex_lock(&mutex);
    }

    if (espera) {
        acao(aguardo, cli_id);
        pthread_mutex_unlock(&mutex);
        sem_wait(&porta); // espera todos os lugares liberarem
    } else {
        comendo++;
        espera = (comendo == 5);
		pthread_mutex_unlock(&mutex);
    }

    // comecou a fazer as ações
    // observe que várias threads podem fazer isso ao mesmo tempo
    // ganha quem pegar o lock de impressao primeiro
    acao(entra, cli_id);

    sleep(2); // dorme pra desacelerar execução

    acao(come, cli_id);

    sleep(2); // dorme pra desacelerar execução

    // saiu do bar
    pthread_mutex_lock(&mutex);

    comendo--;
    acao(sai, cli_id);
    
    // se for o último a comer, deve avisar os demais que o bar está livre    
    if (comendo == 0) {
        acao(vazio, cli_id);
        n = min(5, presentesFila()); // n é 5 ou é a qtd de clientes que estava esperando na porta
        espera = (n == 5);
        comendo = n;
        for (i = 0; i <  n; i++) // libera os que estiverem esperando (max = 5)
            sem_post(&porta);
    }

    pthread_mutex_unlock(&mutex);
    
    return (void*) v;
}

/* Arrange the elements of ARRAY in random order 
 * Fonte: http://stackoverflow.com/questions/6127503/shuffle-array-in-c */
void shuffle(int *array, int n) {
    int i, j, aux, usec;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    usec = tv.tv_usec;
    srand48(usec);

    if (n > 1) {
        for (i = n - 1; i > 0; i--) {
            j = (unsigned int) (drand48()*(i+1));
            aux = array[j];
            array[j] = array[i];
            array[i] = aux;
        }
    }
}

int main() {
	bool valid_max_clientes = false;
	
	while (!valid_max_clientes) {
		printf("\033[?1049h\033[H"); // clear terminal
		printf("Informe o número máximo de clientes que serão atendidos no bar: ");
		scanf("%d", &MAX_CLIENTES);
		setbuf(stdin, 0);
		if (MAX_CLIENTES > 0)
			valid_max_clientes = true;
	}

    pthread_t thr[MAX_CLIENTES];
    int i, id[MAX_CLIENTES];

    sem_init(&porta, 0, 0); // iniciando semáforo porta

    // inicializando ids dos clientes
    for (i = 0; i < MAX_CLIENTES; i++) {
        id[i] = i;
    }
    
    // rearranja os ids do vetor id
    shuffle(id, random() % MAX_CLIENTES);
    
    // exibe bar inicial
    exibe_bar("  Bar Aberto!");
    sleep(3);
    
    // iniciando as threads de todos os clientes
    for (i = 0; i < MAX_CLIENTES; i++) {
        pthread_create(&thr[i], NULL, f_cliente, (void *) &id[i]);
        sleep(1); // espera 1 segundo pra cada cliente entrar 
    }

    // espera todos os clientes irem embora
    for (i = 0; i < MAX_CLIENTES; i++) 
        pthread_join(thr[i], NULL);

    chef = -1;
    // exibe bar fechado
    exibe_bar("  Bar Fechado!");
    sleep(2);
    printf("\033[?1049h\033[H"); // clear terminal
    
    return 0;
}
