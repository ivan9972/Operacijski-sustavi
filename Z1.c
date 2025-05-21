#include <math.h> 
#include <stdio.h>  
#include <sys/types.h>  
#include <signal.h> 
#include <stdlib.h>  
#include <unistd.h>  

int zavrsi = 0;
int statusBr;

// Funkcija za SIGINT signal
void sigint_funkcija (int sig) {
    printf("\nOdaberite :\n1) Prekid programa bez dovrsavanja radnje\n2) Zavrsi program\n3) Ispisi trenutni status\n4) Nastavi\n");
    int input;
    scanf("%d", &input);
    switch(input) {
        case 1:
            printf("\nPrekid programa\n");
            exit(0);
            break;
        case 2:
            kill(getpid(), SIGTERM); 
            break;
        case 3:
            kill(getpid(), SIGUSR1); 
            break;
        default:
            break;
    }
}

// Funkcija za SIGTERM signalom
void sigterm_funkcija (int sig) {
    printf("\nSignal primljen, program uspjesno zavrsen\n");
    zavrsi = 1;
    exit(0);
}

// Funkcija za SIGUSR1 signalom
void sigusr1_funkcija (int sig) {
    printf("\nTrenutni status je: %d\n", statusBr); 
}

// Obrada datoteka
void proces(FILE *status, FILE *obrada) {
    printf("\n%d\n", statusBr);  // Ispisuje trenutni status

    if (statusBr == 0) {
        int obradaBr;
        while(!feof(obrada)) {
            fscanf(obrada, "%d", &obradaBr);  // Čita broj iz obrade
        }
        statusBr = round(sqrt(obradaBr));  // Ažurira status
        printf("\n%d\n", statusBr);
    }

    fseek(status, 0, SEEK_SET);
    fprintf(status, "%d", 0);
    fflush(status); 
    fflush(obrada);
    sleep(1);
    fseek(status, 0, SEEK_SET);
    fseek(obrada, 0, SEEK_END);
    statusBr++;
    fprintf(status, "%d", statusBr);  // Piše novi status u status.txt
    fprintf(obrada, "%d\n", statusBr*statusBr);  // Piše kvadrat novog statusa u obrada.txt
    printf("\nUpisan broj\n");

    fflush(status); 
    fflush(obrada); 
    sleep(1);
}

void main() {
    FILE  *status, *obrada;
    status = fopen("status.txt", "r");
    obrada = fopen("obrada.txt", "r+");

    fscanf(status, "%d", &statusBr);  // Čita početni status iz datoteke
    signal(SIGTERM, sigterm_funkcija); 
    signal(SIGINT, sigint_funkcija);
    signal(SIGUSR1, sigusr1_funkcija);

    while(zavrsi == 0) {  // Glavna petlja programa
        status = freopen("status.txt", "w+", status);
        proces(status, obrada);  // Poziva funkciju za obradu datoteka
    }
    fclose(status);
    fclose(obrada);
}
