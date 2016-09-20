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
 * irem embora para poder entrar no bar.  */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SEATS 5
#define MAX_CLIENTES 15

sem_t mutex, block;

bool espera = false;

/* contadores */
int esperando = 0, comendo = 0;

/* auxiliar */
int min(int x, int y){
    return (x < y) ? x : y;
}

void exibe_bar(){
    // TODO atualizacao de cada status do bar.
    // animacao ocorre aqui
    
    /* possível design da animação
     *      ---------------------------
     *      |      _____________      |
     *  	|     |_____BAR_____|     |
     * 	2	       __ __ __ __ __     |
     * 		|       4 15  8 10  1     |
     * 		|                         |
     * 		---------------------------
     * Onde os números são os clientes
     * *Bar lotado*
     * */
}

void* f_cliente(void *v){
    int i, cli_id = *(int *) v, n;

    sem_wait(&mutex);
    if(espera){
        printf("Cliente %d está esperando na porta\n", cli_id);
        esperando++;
        sem_post(&mutex);
        sem_wait(&block); // espera todos os lugares liberarem
        while (comendo > 0);
    }else{
        comendo++;
        espera = (comendo == 5);
        sem_post(&block); // COMENTAR AQUI
        sem_post(&mutex);
    }
    printf("Cliente %d entrou no bar\n", cli_id);
    // começou a comer
    printf("Cliente %d começou a comer\n", cli_id);
    sleep(7);
    exibe_bar();

    // acabou de comer
    printf("Cliente %d está indo embora\n", cli_id);
    sem_wait(&mutex);
    comendo--;
    if(comendo == 0){
        printf("Cliente %d avisou que está vazio!\n", cli_id);
        n = min(5, esperando); // libera os 5 clientes, ou a qtd que tiver esperando comer
        esperando -= n;
        comendo += n;
        espera = (comendo == 5);
        for (i = 0; i <  n; i++)
			sem_post(&block);  // sem_post(&block, n)? como liberar os N que estao esperando em C? R: fazer um laço para isso!
    }
    sem_post(&mutex);
    return (void*) v;
}


int main(){
    pthread_t thr[MAX_CLIENTES];
    int i;

    sem_init(&mutex, 0, 1); // iniciando mutex
    sem_init(&block, 0, SEATS); // como inicia esse semaforo em C? (pergunta identica a de cima) R: o numero de SEATS

    // exibe bar inicial
    exibe_bar();

    // iniciando as threads de todos os clientes
    for(i = 1; i <= MAX_CLIENTES; i++){
        pthread_create(&thr[i-1], NULL, f_cliente, (void *) &i);
        sleep(1); // espera 1 segundo pra cada cliente entrar 
    }

    // espera todos os clientes comerem
    for(i = 0; i < MAX_CLIENTES; i++) 
        pthread_join(thr[i], NULL);

    return 0;
}

