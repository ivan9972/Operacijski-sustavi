#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int *ULAZ, *BROJ, brSt, brDr, *REZ;

// Pronalaženje maksimalne vrijednosti u nizu BROJ
int max(){
	int najv = BROJ[0];
	for(int i = 0; i < brSt; i++){
		if(BROJ[i] > najv){
			najv = BROJ[i];
		}
	}
	return najv;
}

// Provjerava jesu li svi stolovi zauzeti
int sve_zauzeto(){
	for(int i = 0; i < brSt; i++){
		if(REZ[i] == -1){	// Ako je bilo koji stol slobodan vraća 0
			return 0; 
		}
	}
	return 1;
}

void udji_u_kriticni_odsjecak(int i){
	ULAZ[i] = 1;
	BROJ[i] = max() + 1;
	ULAZ[i] = 0;

	for(int j = 0; j < brSt - 1; j++){
		while(ULAZ[j] != 0){}	// Čeka dok dretva j ne izađe iz kritičnog odsječka
		while(BROJ[j] != 0 && (BROJ[j] < BROJ[i] || (BROJ[j] == BROJ[i] && j < i))){}
	}
}

void izadji_iz_kriticnog_odsjecka(int i){
	BROJ[i] = 0;
}

// Funkcija koja provjerava stanje stola i pokušava rezervirati stol za dretvu
void *provjeri_stol(void *pdr){
	if(sve_zauzeto()){
		pthread_exit("svi su stolovi zauzeti"); // Ako su svi stolovi zauzeti, završi dretvu
	}
	
	int dr = *((int *)pdr);
	int rand_tbl = rand() % brSt;
	printf("Dretva %d: pokusavam rez stol %d\n", dr + 1, rand_tbl + 1); // Pokušaj rezervacije
	udji_u_kriticni_odsjecak(rand_tbl);
	
	char stanje[brSt];
    for(int i = 0; i < brSt; i++){
        stanje[i] = (REZ[i] == -1) ? '-' : REZ[i] + 1 + '0';
    }
    stanje[brSt] = '\0';

	if(REZ[rand_tbl] == -1){
		REZ[rand_tbl] = dr;	// Ako je stol slobodan, rezerviraj ga 
		for(int i = 0; i < brSt; i++){
            stanje[i] = (REZ[i] == -1) ? '-' : REZ[i] + 1 + '0'; // Ažuriraj stanje stolova
        }
        stanje[brSt] = '\0';
		printf("Dretva %d: rezerviram stol %d, stanje: %s\n", dr + 1, rand_tbl + 1, stanje); // Ispis uspješne rezervacije
	} else {
		printf("Dretva %d: neuspjela rezervacija stola %d, stanje: %s\n", dr + 1, rand_tbl + 1, stanje);
	}
	
	izadji_iz_kriticnog_odsjecka(rand_tbl);
}

void main(){
	printf("br dretvi: ");
	scanf("%d", &brDr);
	printf("br st: ");
	scanf("%d", &brSt);
	fflush(stdin);

	pthread_t thread_id;

	REZ = (int*)malloc(sizeof(int) * brSt);
    BROJ = (int*)malloc(sizeof(int) * brSt);
    ULAZ = (int*)malloc(sizeof(int) * brSt);
	
	for(int i = 0; i < brSt; i++){
		REZ[i] = -1;
		BROJ[i] = 0;
		ULAZ[i] = 0;
	}

// Dok nisu zauzeti stolovi
	while(!sve_zauzeto()){ 
		for(int i = 0; i < brDr; i++){
			int *pdr = (int *)malloc(sizeof(int));
			*pdr = i;
			pthread_create(&thread_id, NULL, provjeri_stol, pdr); // Nova dretva
		}
	}
	
	for(int i = 0; i < brDr; i++){
		pthread_join(thread_id, NULL); // Čekanje dok svaka od dretvi ne završi
	}
	
	free(REZ);
	free(BROJ);
	free(ULAZ);
}
