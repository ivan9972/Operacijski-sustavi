#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 // Broj filozofa

// Globalne varijable

sem_t sem[N]; // Semafori za svakog filozofa
sem_t mutex; // Mutex za zaštitu pristupa zajedničkim podacima
int state[N]; // Stanja filozofa (1 = gladan, 2 = jede, 3 = misli)

void ispisi_stanja() {
    printf("Stanje filozofa: ");
    for (int i = 0; i < N; i++) {
        if (state[i] == 2)      // JEDE
            printf("X ");
        else if (state[i] == 1) // GLADAN
            printf("o ");
        else                    // MISLI
            printf("O ");
    }
    printf("\n");
    fflush(stdout);
}

void test(int br) { // Funkcija koja provjerava može li filozof jesti
    if (state[br] == 1 && state[(br + 4) % N] != 2 && state[(br + 1) % N] != 2) { // Ako je filozof gladan i susjedi ne jedu
        state[br] = 2; // Filozof počinje jesti
        printf("Filozof %d uzima vilice i jede\n", br + 1);
        fflush(stdin);
        sleep(2); // Simulacija jela
        ispisi_stanja();
        sem_post(&sem[br]); // Signalizira da filozof može jesti
    }
}

void uzima_vilicu(int br) {
    sem_wait(&mutex); // Zatvaranje mutexa za pristup zajedničkim podacima
    state[br] = 1; // Filozof postaje gladan
    printf("Filozof %d je gladan\n", br + 1);
    test(br);
    sem_post(&mutex); // Otvaranje mutexa
    sem_wait(&sem[br]); // Čekanje dok filozof ne može jesti
    fflush(stdin);
    sleep(2); // Simulacija vremena čekanja
}

void vrati_vilice(int br) {
    sem_wait(&mutex);
    printf("Filozof %d ostavlja vilice i ide misliti\n", br + 1);
    fflush(stdin);
    sleep(2); // Simulacija vremena razmišljanja
    state[br] = 3; // Filozof počinje misliti
    test((br + 4) % N);
    test((br + 1) % N);
    sem_post(&mutex);
}

void* filozof(void* pdr) {
    // Funkcija niti za svakog filozofa
    int temp = *((int *)pdr);
    while (1) {
        sleep(2); // Filozof misli
        uzima_vilicu(temp); // Filozof pokušava uzeti vilice
        sleep(2); // Filozof jede
        vrati_vilice(temp); // Filozof vraća vilice i misli
    }
}

void main() {
    pthread_t thread_id[N]; // Niz niti za svakog filozofa
    sem_init(&mutex, 0, 1);
    int br;
    for (int i = 0; i < N; i++) {
        sem_init(&sem[i], 0, 0); // Inicijalizacija semafora za svakog filozofa
        state[i] = 3; // Svi filozofi počinju razmišljati
    }
    for (int i = 0; i < N; i++) {
        int *br = (int *) malloc(sizeof(int));
        *br = i; // Dodjela broja filozofa
        pthread_create(&thread_id[i], NULL, filozof, br); // Kreiranje niti za svakog filozofa
        printf("Filozof %d misli\n", i + 1);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL); // Čekanje da sve niti završe
    }
}
