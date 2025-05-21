#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int *PRAVO, *ZASTAVICA, pravoId, zastavicaId;

// Funkcija za ulazak u kritični odsječak
void udi_u_kriticni_odsjecak(int i, int j){
    ZASTAVICA[i] = 1;
    while (!(ZASTAVICA[j] == 0)) { // Proces j želi ući u kritični odsječak
    if (*PRAVO == j) {
        ZASTAVICA[i] = 0;      // Proces i odustaje
        while (*PRAVO == j) { 
        }
        ZASTAVICA[i] = 1;      // Ponovno pokušava ući u kritični odsječak
    }
}

}

// Funkcija za izlazak iz kritičnog odsječka
void izadji_iz_kriticnog_odsjecka(int i, int j){
    ZASTAVICA[i] = 0;
    *PRAVO = j;
}

// Funkcija koju svaki proces izvršava
void proc(int i, int j){
    int k, m;
    for(k = 1; k<5; k++){
        udi_u_kriticni_odsjecak(i, j);
        for(m = 1; m<5; m++){
            printf("i: %d, k: %d, m: %d\n", i, k, m);
            sleep(1);
        }
        izadji_iz_kriticnog_odsjecka(i, j);
    }
}

// Funkcija za brisanje zajedničke memorije i završavanje programa
void brisi(){
    shmdt(PRAVO); // Odspajanje zajedničke memorije
    shmdt(ZASTAVICA);

    shmctl(pravoId, IPC_RMID, NULL); // Brisanje zajedničke memorije
    shmctl(zastavicaId, IPC_RMID, NULL);

    exit(0);
}

void main(){
    // Kreiranje zajedničke memorije
    pravoId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0660);
    zastavicaId = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0660);
    PRAVO = (int*)shmat(pravoId, NULL, 0);
    ZASTAVICA = (int*)shmat(zastavicaId, NULL, 0);

    // Kreiranje procesa
    if(fork() == 0){
        proc(1, 0); // Prvi proces
    }
    if(fork() == 0){
        proc(0, 1); // Drugi proces
    }

    signal(SIGINT, brisi);

    wait(NULL);
    wait(NULL);

    brisi(); // Brisanje zajedničke memorije i završavanje programa
}
