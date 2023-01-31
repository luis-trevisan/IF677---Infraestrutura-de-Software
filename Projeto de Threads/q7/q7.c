#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM 3
#define TAM_VETOR TAM * TAM

//struct para auxiliar na detecção de elementos repetidos
typedef struct latino {
    int usado;
    int repeticoes;
    int elemento;
} Latino;

Latino *vet;
int matriz[TAM][TAM] = {{0, 1, 2}, {4, 5, 3}, {6, 9, 9}};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *rotina(void *thread_id) {
    int id = *(int *) thread_id;
    int aux;

    for(int i = 0; i < TAM; i++) {
        aux = matriz[id][i];

        for(int j = 0; j < TAM_VETOR; j++) {
            pthread_mutex_lock(&mutex); //entrando na região critica, onde a variavel repetições/uso é modificada
            if(vet[j].usado == 1 && aux == vet[j].elemento) {
                vet[j].repeticoes++;
            }
            else if(vet[j].usado == 0) {
                vet[j].usado = 1;
                vet[j].elemento = aux;
                j = TAM_VETOR;
            }
            pthread_mutex_unlock(&mutex); // saindo da região critica
        }
    }

    pthread_exit(NULL);
}

int main() {
    int *t_id, flag = 0;
    pthread_t thread[TAM];

    vet = (Latino *) calloc(TAM_VETOR, sizeof(Latino));

    if(vet == NULL) {
        printf("Falha na alocacao\n");
        exit(1);
    }

    //criando as threads e passando o "i" do for como parametro para ser usado com id da thread
    for(int i = 0; i < TAM; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;

        if(pthread_create(&thread[i], NULL, &rotina, (void*) t_id)) {
            printf("Falha na criacao das threads\n");
            exit(1);
        }
    }

    //esperando as threads
    for(int i = 0; i < TAM; i++) {
        if(pthread_join(thread[i], NULL)) {
            printf("Falha na espera das threads\n");
            exit(1);
        }
    }

    //percorre o vetor e verifica se houve repetições do valor e printa
    for(int i = 0; i < TAM_VETOR; i++) {
        if(vet[i].repeticoes != 0) {
            printf("A matriz nao e quadrado latino, o elemento %d repete\n", vet[i].elemento);
            flag = 1;
        }
    }

    //a flag começa como 0 e caso algum elemento seja repetido ela se torna 1
    if(flag != 1) {
        printf("A matriz e quadrado latino\n");
    }

    //desalocando os recursos usados no programa
    free(t_id);
    free(vet);
    pthread_mutex_destroy(&mutex);

    return 0;
}