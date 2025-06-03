#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>

#define N 5                // Broj mjesta na vrtuljku
#define M 15               // Broj posjetitelja
#define SEMNAME_MJESTA "mjesta"  // Ime semafora

// Funkcija koja simulira ponašanje posjetitelja
void posjetitelj(int i){
    sem_t* mjesta = sem_open(SEMNAME_MJESTA, 0);  // Otvaranje semafora

    while(1){
        sem_wait(mjesta);  // Čekanje na slobodno mjesto
        printf("Proces %d je usao\n", i);  // Posjetitelj je ušao	
        sleep(3);  // Simulacija vremena provedenog na vrtuljku
    }   
}

// Funkcija koja simulira ponašanje vrtuljka
void vrtuljak(){
    int br = 0;  // Brojač krugova
    sem_t* mjesta = sem_open(SEMNAME_MJESTA, 0);  // Otvaranje semafora
    int *value = malloc(sizeof(int));

    while(1){
        do{
            sem_getvalue(mjesta, value);
            printf("Broj slobodnih mjesta: %d\n", *value);  // Ispisivanje broja slobodnih mjesta
            sleep(2);
        } while(*value != 0);  // Petlja se izvršava dok sva mjesta nisu popunjena

        sleep(2);
        printf("Vrtuljak se vrti %d\n", br++);  // Trenutni krug
        sleep(3);  // Simulacija vrtnje vrtuljka
        printf("Vrtuljak se zaustavio\n");
        sleep(2);

        for(int i = 0; i < N; i++){
            sem_post(mjesta);  // Oslobađanje mjesta na vrtuljku
            printf("Sjedalo br %d na vrtuljku je prazno\n", i + 1);  // Ispisivanje da je sjedalo prazno
        }
        sleep(2);
    }
    sleep(2);
}

void main(){
    sem_t* mjesta = sem_open(SEMNAME_MJESTA, O_CREAT, 0644, N);

    for(int i = 0; i < N; i++){
        sem_post(mjesta);  // Povećanje vrijednosti semafora na N
    }

    if(fork() == 0){
        vrtuljak();  // Kreiranje procesa koji simulira vrtuljak
    }

    sleep(2);

    for(int i = 0; i < M; i++){
        if(fork() == 0){
            posjetitelj(i);  // Kreiranje procesa koji simulira posjetitelje
        }
    }

    for(int i = 0; i < M + 1; i++){
        wait(NULL);  // Čekanje svih kreiranih procesa
    }

    sem_destroy(mjesta);  // Uništavanje semafora
}
