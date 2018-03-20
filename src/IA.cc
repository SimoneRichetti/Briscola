/**
 * @file
 * File contenente il modulo IA.
 */

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

using namespace std ;

#include "strutt_dati.h"
#include "grafica_debug.h"

// Inizio definizioni delle funzioni del modulo IA.

/** Determina la tipologia di carta in base a seme e punteggio.
 *
 * @param[in] c Carta da valutare
 * @param[in] briscola Seme che determina la briscola nella partita
 * @return il tipo di carta a cui appartiene c
 *
 * La funzione viene invocata nel caso in cui la CPU giochi per seconda. In base
 * al punteggio e al seme della carta passata per parametro, la funzione ritorna
 * uno dei ::tipi_carta.
 * @sa ::scegli_carta_p2 per vedere l'utilizzo della funzione. 
 */
static tipi_carta determina_tipo_carta(carta *c, const semi briscola)
{
	assert(c!=NULL) ;
	
	if(c->seme==briscola) {
		if(c->punteggio >= 10)
			return BRISCOLA_E_CARICO ;
		else
			return BRISCOLA ;
	} else {
		if(c->punteggio >= 10)
			return CARICO ;
		else
			return LISCIO ;
	}	
}

carta* confronta_valori(carta *carta_p1, carta *carta_p2)
{
	assert(carta_p1 != NULL && carta_p2 != NULL) ;
	assert(carta_p1->seme == carta_p2->seme) ;
	
	carta* vincitore ;
		
	if(carta_p1->punteggio > carta_p2->punteggio)
		vincitore=carta_p1 ;
	else if((carta_p1->punteggio == carta_p2->punteggio) &&
		(carta_p1->numero > carta_p2->numero))
		vincitore=carta_p1 ;
	else
		vincitore=carta_p2 ;
		
	return vincitore ;
}

/** Ordina i valori di un array e gli indici associati
 *
 * @param[in,out] valori Array contenente i valori da ordinare
 * @param[in,out] indici Array contenente gli indici iniziali degli elementi
 * @param[in] n_elem Lunghezza dei precedenti array
 *
 * La funzione prende in ingresso un vettore di interi e li ordina in maniera
 * crescente. Quando vengono scambiati 2 valori, però, vengono scambiati gli
 * elementi corrispondenti nell'array degli indici, di modo che a fine ordinamento si possa
 * ancora risalire alla posizione originaria nel vettore degli elementi.
 * @sa ::scegli_carta_p2 per un esempio del suo utilizzo.
 */
static void ordina_valori(int valori[], int indici[], const int n_elem) 
{
	int temp;
	for(int i=0; i<n_elem; i++) {
		for(int j=i; j<n_elem; j++) {  
			if(valori[j]<valori[i]) {
				temp=valori[j];
				valori[j]=valori[i];
				valori[i]=temp;
				
				temp=indici[j];
				indici[j]=indici[i];
				indici[i]=temp;        
			}
		}         
	} 	
}

/** Valuta il valore delle carte contro un carico a briscola
 *
 * @param[in] mano Mano della CPU
 * @param[in] c Carta giocata dall'utente precedentemente
 * @param[out] valore_giocata Array dei valori delle carte
 * @param[out] indici_valori Array che andrà a contenere la posizione iniziale dei valori
 * @param[in] briscola Seme che determina la briscola nella partita
 *
 * La funzione, per ogni carta nella mano, assegna un valore a seconda del suo seme
 * e punteggio, relativamente alla giocata dell'utente. Dopodichè ordina il
 * vettore dei valori e, nel caso ci siano più carte con lo stesso valore, va a
 * confrontarle per valutare ulteriormente la migliore tra quelle equivalenti.
 * In questo caso, l'algoritmo da il valore massimo al carico di briscola maggiore
 * a quello giocato dall'utente (se c'è), se no tenta di dare la carta con
 * punteggio più basso possibile.
 * @sa ::ordina_valori, ::scegli_carta_p2.
 */
static void valuta_giocata_carico_briscola(const mazzo &mano, carta* c,
	int valore_giocata[], int indici_valori[], const semi briscola)
{
	assert(mano.carte != NULL && c != NULL) ;
	for(int i=0; i<mano.num_carte; i++) {
		if(mano.carte[i].seme==c->seme) {
			if(mano.carte[i].punteggio >= 10)
				valore_giocata[i]=1 ;
			else
				valore_giocata[i]=3 ;
		} else {
			if(mano.carte[i].punteggio >= 10)
				valore_giocata[i]=4 ;
			else
				valore_giocata[i]=2 ;
		}
	}
	
	ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
	if(mano.num_carte>1) {
		while(valore_giocata[0]==valore_giocata[1]) {
			if(mano.carte[indici_valori[0]].punteggio>
			   mano.carte[indici_valori[1]].punteggio)
				valore_giocata[0]++ ;
			else
				valore_giocata[1]++ ;
				
			ordina_valori(valore_giocata, indici_valori,
				mano.num_carte) ;
		}
	}
}

/** Valuta il valore delle carte contro una briscola.
 *
 * @param[in] mano Mano della CPU
 * @param[in] c Carta giocata dall'utente precedentemente
 * @param[out] valore_giocata Array dei valori delle carte
 * @param[out] indici_valori Array che andrà a contenere la posizione iniziale dei valori
 * @param[in] briscola Seme che determina la briscola nella partita
 *
 * Vedi la documentazione di ::valuta_giocata_carico_briscola per il funzionamento
 * generale dell'algoritmo. In particolare, questa funzione tenta di dare meno
 * punti possibili all'avversario o, in alternativa, prendere con la briscola più piccola
 * possibile.
 * @sa ::valuta_giocata_carico_briscola, ::ordina_valori, ::scegli_carta_p2
 */
static void valuta_giocata_briscola(const mazzo &mano, carta* c,
	int valore_giocata[], int indici_valori[], const semi briscola)
{
	assert(mano.carte != NULL && c != NULL) ;
	
	for(int i=0; i<mano.num_carte; i++) {
		if(mano.carte[i].seme==c->seme) {
			if(&mano.carte[i]==confronta_valori(&mano.carte[i], c)) {
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=3 ;
				else
					valore_giocata[i]=2 ;						
			} else
				valore_giocata[i]=4 ;						
		} else {
			if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=5 ;
				else
					valore_giocata[i]=1 ;
		}
	}
	ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
	if(mano.num_carte>1) {
		while(valore_giocata[0]==valore_giocata[1]) {
			if(mano.carte[indici_valori[0]].punteggio>
			   mano.carte[indici_valori[1]].punteggio)
				valore_giocata[0]++ ;
			else
				valore_giocata[1]++ ;
				
			ordina_valori(valore_giocata, indici_valori,
				mano.num_carte) ;
		}
	}
}

/** Valuta il valore delle carte contro un carico.
 *
 * @param[in] mano Mano della CPU
 * @param[in] c Carta giocata dall'utente precedentemente
 * @param[out] valore_giocata Array dei valori delle carte
 * @param[out] indici_valori Array che andrà a contenere la posizione iniziale dei valori
 * @param[in] briscola Seme che determina la briscola nella partita
 *
 * Vedi la documentazione di ::valuta_giocata_carico_briscola per il funzionamento
 * generale dell'algoritmo. In particolare, questa funzione tenta di giocare il carico
 * più grande dello stesso seme (se possibile) o, in alternativa, prendere con la
 * briscola più piccola possibile.
 * @sa ::valuta_giocata_carico_briscola, ::ordina_valori, ::scegli_carta_p2
 */
static void valuta_giocata_carico(const mazzo &mano, carta* c,
	int valore_giocata[], int indici_valori[], const semi briscola)
{
	assert(mano.carte != NULL && c != NULL) ;
	
	for(int i=0; i<mano.num_carte; i++) {
		if(mano.carte[i].seme==c->seme) {
			if(&mano.carte[i]==confronta_valori(&mano.carte[i], c)) {
				valore_giocata[i]=1 ;						
			} else
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=5 ;
				else
					valore_giocata[i]=4 ;							
		} else {
			if(mano.carte[i].seme==briscola){
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=3 ;
				else
					valore_giocata[i]=2 ;
			} else {
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=5 ;
				else
					valore_giocata[i]=4 ;
			}
		}
	}
	
	ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
	if(mano.num_carte>1) {
		while(valore_giocata[0]==valore_giocata[1]) {
			if(mano.carte[indici_valori[0]].punteggio>
			   mano.carte[indici_valori[1]].punteggio)
				valore_giocata[0]++ ;
			else
				valore_giocata[1]++ ;
				
			ordina_valori(valore_giocata, indici_valori,
				mano.num_carte) ;
		}
	}
}

/** Valuta il valore delle carte contro una carta con un basso punteggio e un seme
 * diverso dalla briscola.
 *
 * @param[in] mano Mano della CPU
 * @param[in] c Carta giocata dall'utente precedentemente
 * @param[out] valore_giocata Array dei valori delle carte
 * @param[out] indici_valori Array che andrà a contenere la posizione iniziale dei valori
 * @param[in] briscola Seme che determina la briscola nella partita
 *
 * Vedi la documentazione di ::valuta_giocata_carico_briscola per il funzionamento
 * generale dell'algoritmo. In particolare, questa funzione tenta di "strozzare"
 * giocando un carico dello stesso seme o, in alternativa, dare il minor numero
 * di punti possibili all'avversario.
 * @sa ::valuta_giocata_carico_briscola, ::ordina_valori, ::scegli_carta_p2.
 */
static void valuta_giocata_liscio(const mazzo &mano, carta* c,
	int valore_giocata[], int indici_valori[], const semi briscola)
{
	assert(mano.carte != NULL && c != NULL) ;
	
	for(int i=0; i<mano.num_carte; i++) {
		if(mano.carte[i].seme==c->seme) {
			if(&mano.carte[i]==confronta_valori(&mano.carte[i], c)) {
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=1 ;
				else {
					if(mano.carte[i].punteggio > 0)	
						valore_giocata[i]=2 ;
					else
						valore_giocata[i]=4 ;
				}					
			} else
				valore_giocata[i]=2 ;						
		} else {
			if(mano.carte[i].seme==briscola) {
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=6 ;
				else
					valore_giocata[i]=5 ;
			} else {
				if(mano.carte[i].punteggio >= 10)
					valore_giocata[i]=7 ;
				else
					valore_giocata[i]=3 ;
			}
		}
	}
				
	ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
	if(mano.num_carte>1) {
		while(valore_giocata[0]==valore_giocata[1]) {
			if(valore_giocata[0]==1 || valore_giocata[0]==2) {
				if(mano.carte[indici_valori[0]].punteggio>
				   mano.carte[indici_valori[1]].punteggio)
					valore_giocata[1]++ ;
				else
					valore_giocata[0]++ ;
			} else {
				if(mano.carte[indici_valori[0]].punteggio>
				   mano.carte[indici_valori[1]].punteggio)
					valore_giocata[0]++ ;
				else
					valore_giocata[1]++ ;
			}
			ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
		}
	}
}

void scegli_carta_p2(const mazzo &mano, int &idx, carta *c, semi briscola)
{
	assert(mano.carte != NULL) ;
	
	for(int i=0; i<mano.num_carte; i++) {
		D2(stampa_mazzo(&mano.carte[i], 1)) ;
	}
	D2(cout<<"CARICHI GIOCATI"<<endl) ;
	for(int i=0; i<4; i++)
		D2(cout<<carichi_giocati[i]<<" ") ;
	D2(cout<<endl) ;
	
	/* Inizializzazione vettore valori giocata */
	int valore_giocata[3] = { 1,1,1 } ;
	int indici_valori[3] = { 0,1,2 } ;
	if(c == NULL) {
		/* Se l'IA gioca per prima, applica sempre lo stesso criterio
		 * di valutazione delle carte.
		 */
		for(int i=0; i<mano.num_carte; i++) {
			if(carichi_giocati[(int)mano.carte[i].seme]<2)
				valore_giocata[i]++ ;
			if(mano.carte[i].seme==briscola)
				valore_giocata[i]+=2 ;
			if(mano.carte[i].punteggio>=10)
				valore_giocata[i]+=3 ;
		}
		ordina_valori(valore_giocata, indici_valori, mano.num_carte) ;
		if(mano.num_carte>1) {
			/* Finchè ci sono valori a parimerito, confronto le carte
			 * equivalenti e valuto la migliore
			 */
			while(valore_giocata[0]==valore_giocata[1]) {
				if(mano.carte[indici_valori[0]].punteggio>
				   mano.carte[indici_valori[1]].punteggio)
					valore_giocata[0]++ ;
				else
					valore_giocata[1]++ ;
			}
			ordina_valori(valore_giocata, indici_valori,
				mano.num_carte) ;
		}
	} else {
		/* Se l'IA gioca per seconda, valuto la carta giocata dall'utente
		 * e chiamo la funzione corrispondente
		 */
		tipi_carta tipo_c=determina_tipo_carta(c, briscola) ;
		switch(tipo_c) {
			case BRISCOLA_E_CARICO:
				valuta_giocata_carico_briscola(mano, c,
					valore_giocata, indici_valori, briscola) ;
				break ;
			case BRISCOLA:
				valuta_giocata_briscola(mano, c, valore_giocata,
					indici_valori, briscola) ;
				break ;
			case CARICO:
				valuta_giocata_carico(mano, c, valore_giocata,
					indici_valori, briscola) ;
				break ;
			case LISCIO:
				valuta_giocata_liscio(mano, c, valore_giocata,
					indici_valori, briscola) ;
				break ;
			default:
				exit(6) ;
		}
	}
	
	D2(cout<<"Valori giocate:"<<endl) ;
	for(int i=0; i<mano.num_carte; i++)
		D2(cout<<valore_giocata[i]<<" ") ;
	D2(cout<<endl) ;
	
	for(int i=0; i<mano.num_carte; i++)
		D2(cout<<indici_valori[i]<<" ") ;
	D2(cout<<endl) ;
	
	idx=indici_valori[0] ;
	
	D1(cout<<"Carta giocata: ") ;
	D1(stampa_mazzo(&mano.carte[idx], 1)) ;
	mano.carte[idx].giocata=true ;
	if(mano.carte[idx].punteggio>=10)
		carichi_giocati[(int)mano.carte[idx].seme]++ ;
}

// Fine definizioni delle funzioni del modulo IA.
