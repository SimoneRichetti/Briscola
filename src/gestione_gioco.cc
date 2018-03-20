/**
 * @file
 * File contenente il modulo Gestione gioco.
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

using namespace std ;

#include "strutt_dati.h"
#include "grafica_debug.h"
#include "IA.h"

// Inizio definizioni delle funzioni del modulo Gestione gioco.

void inizializza_giocatore(giocatore &g1, const char nome_giocatore[], bool u)
{
	strcpy(g1.nome, nome_giocatore) ;
	g1.mano.num_carte = 0 ;
	if(g1.mano.carte == NULL)
		g1.mano.carte = new carta[NUM_CARTE_MANO] ;
	g1.punteggio = 0 ;
	g1.ultima_presa = false ;
	g1.utente = u ;
	g1.x_giocata = g1.utente ? X_GIOCATA_G1 : X_GIOCATA_CPU ;
	g1.y_giocata = y_centro - imagesHeight/2 ;
}

/** Inizializzazione mazzo
 *
 * @param[in] m Mazzo da inizializzare.
 * @sa ::mazzo, ::start_game per il suo utilizzo.
 */
static void inizializza_mazzo(mazzo &m)
{
	m.num_carte = NUM_CARTE_MAZZO ;
	if(m.carte!=NULL)
		m.carte = new carta[m.num_carte] ;
	int idx=0 ;

	for(int j=1; j<=10; j++) {
		for(int i=0; i<4; i++, idx++) {
			m.carte[idx].numero=j ;
			m.carte[idx].seme=(semi)i ;
			m.carte[idx].presa=false ;
			m.carte[idx].giocata=false ;
			switch(m.carte[idx].numero) {
				case 1: m.carte[idx].punteggio = 11 ; break ;
				case 3: m.carte[idx].punteggio = 10 ; break ;
				case 8: m.carte[idx].punteggio = 2 ; break ;
				case 9: m.carte[idx].punteggio = 3 ; break ;
				case 10: m.carte[idx].punteggio = 4 ; break ;
				default: m.carte[idx].punteggio = 0 ; break ;
			}
			/*Deduco nome file .bmp contenente la carta*/
			char nome[] = "data/0d.png" ;
			nome[5] = '0' + (m.carte[idx].numero % 10) ;
			switch(m.carte[idx].seme) {
				case DENARI: nome[6] = 'd' ; break ;
				case COPPE: nome[6] = 'c' ; break ;
				case BASTONI: nome[6] = 'b' ; break ;
				case SPADE: nome[6] = 's' ; break ;
			}
			m.carte[idx].img = al_load_bitmap(nome) ;
		}
	}
	D2(stampa_mazzo(m.carte, m.num_carte)) ;
}

/** Funzione per mescolare il mazzo in maniera pseudo-casuale.
 *
 * @param[in] m Mazzo da mescolare
 * 
 * La funzione crea un altro ::mazzo di dimensioni uguali a quello in ingresso,
 * dopodichè, per ogni carta del nuovo mazzo, genera casualmente un intero tra 0 
 * e NUM_CARTE_MAZZO che sarà l'indice della carta di m da copiare nel mazzo nuovo.
 * Se la carta è già stata presa, l'indice viene incrementato senza però andare 
 * fuori dal mazzo. Infine, il mazzo vecchio viene deallocato e sostituito da quello nuovo.
 */
static void mescola_mazzo(mazzo &m)
{
	assert(m.carte != NULL) ;
	
	carta* nuovo_mazzo = new carta[m.num_carte] ;
	for(int i=0; i<m.num_carte; i++) {
		int idx = rand() % m.num_carte ;
		while(m.carte[idx].presa)
			idx = (idx+1) % m.num_carte ;
		nuovo_mazzo[i] = m.carte[idx] ;
		m.carte[idx].presa = true ;
	}
	D2(stampa_mazzo(nuovo_mazzo, NUM_CARTE_MAZZO)) ;
	delete [] m.carte ;
	m.carte = nuovo_mazzo ;
}

/** Funzione per pescare dal mazzo.
 *
 * @param[in] mano Mano del giocatore che deve pescare
 * @param[in] m Mazzo da cui pescare
 */
static void pesca(mazzo &mano, mazzo &m)
{
	assert(m.carte != NULL && mano.carte != NULL) ;
	
	assert(mano.num_carte < NUM_CARTE_MANO && m.num_carte > 0) ;

	mano.carte[mano.num_carte++] = m.carte[--m.num_carte] ;
}

/** Funzione per estrarre una carta dalla mano
 *
 * @param[in] mano Mano da cui estrarre la carta
 * @param[in] indice Indice dell'array di carte corrispondente alla carta giocata
 *
 * L'operazione di estrazione della carta consiste semplicemente nello spostare
 * le carte successive una posizione indietro, riducendo di uno il numero di carte
 * nella mano.
 */
static void estrai_carta(mazzo &mano, const int indice)
{
	assert(mano.carte != NULL) ;
	
	assert(mano.num_carte > 0 && (indice >= 0 && indice < mano.num_carte)) ;
	
	for (int i=indice; i<mano.num_carte -1; i++)
		mano.carte[i] = mano.carte[i+1] ;
	mano.num_carte-- ;
}

/** Scelta della carta da parte dell'utente.
 *
 * @param[in] mano Mano dell'utente
 * @param[out] idx Indice della carta giocata dall'utente
 * @param[in] display Schermata di allegro del gioco
 * @return true se l'utente sceglie correttamente il gioco, false se l'utente
 * termina il gioco.
 *
 * La funzione viene chiamata all'interno di ::start_game per leggere la scelta
 * dell'utente. Ad ogni evento di rilascio del mouse, controlla se ciò avviene
 * sopra ad una carta. In tal caso, viene ritornato in uscita il rispettivo indice
 * dell'array carte della mano. Se invece viene premuto il pulsante di chiusura 
 * della finestra, la funzione segnala di chiudere il programma ritornando falso.
 * @sa ::start_game, ::carichi_giocati .
 */
static bool scegli_carta_p1(const mazzo &mano, int &idx, ALLEGRO_DISPLAY* display)
{
	assert(mano.carte != NULL && display != NULL) ;
	
	bool chiusura_forzata=false ;
	
	/* Preparazione lettura input da mouse e display */
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue() ;
	al_register_event_source(event_queue, al_get_mouse_event_source()) ;
	al_register_event_source(event_queue, al_get_display_event_source(display)) ;

	bool done = false ;
	while(!done) {
		ALLEGRO_EVENT event ;
		al_wait_for_event(event_queue, &event) ;
		if(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP){
			int x = event.mouse.x ;
			int y = event.mouse.y ;
			int x_img = x_centro - (mano.num_carte*imagesWidth)/2 ;
			int y_img = mano.carte[0].y_img ;
			for(int i=0; !done && i<mano.num_carte; i++) {
				/* Per ogni carta controllo se il rilascio del
				 * mouse è avvenuto all'interno della sua area
				 * disegnata sullo schermo
				 */
				if((x>x_img && x<x_img+imagesWidth)&&
				   (y>y_img && y<y_img+imagesHeight)) {
					idx = i ;
					done=true ;
				}
				x_img += imagesWidth ;
			}
		} else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			chiusura_forzata=true ;
			done=true ;
		}
	}
	
	if(!chiusura_forzata) {
		/* Nel caso sia giocato un carico, aggiorno la struttura dati
		 * carichi_giocati dell'IA.
		 */
		if(mano.carte[idx].punteggio>=10)
			carichi_giocati[(int)mano.carte[idx].seme]++ ;
			
		D1(cout<<"Carta giocata: ") ;
		D1(stampa_mazzo(&mano.carte[idx], 1)) ;
	}
	
	al_destroy_event_queue(event_queue) ;
	return !chiusura_forzata ;
}

/** Attribuisce i punti al vincitore della mano.
 *
 * @param[in] scelta_p1 Indice della carta scelta dal giocatore che ha giocato per primo 
 * @param[in] scelta_p2 Indice della carta scelta dal giocatore che ha giocato per secondo 
 * @param[in] p1 Giocatore che ha giocato per primo
 * @param[in] p2 Giocatore che ha giocato per secondo
 * @param[in] briscola Il seme che nella partita determina la briscola 
 *
 * La funzione prende in ingresso indici e mani dei giocatori e determina
 * il vincitore in base alle regole, aggiornando la sua ultima presa e il suo
 * punteggio
 */
static void attribuisci_punti(const int scelta_p1, const int scelta_p2, 
		       giocatore *p1, giocatore *p2, const semi briscola)
{
	assert((p1 != NULL) && (p2 != NULL)) ;
	
	carta carta_p1 = p1->mano.carte[scelta_p1],
		carta_p2 = p2->mano.carte[scelta_p2] ;
	giocatore* vincitore = NULL ;
	if(carta_p1.seme == briscola) {
		if(carta_p2.seme == briscola) {
			if(&carta_p1==confronta_valori(&carta_p1, &carta_p2))
				vincitore = p1 ;
			else
				vincitore = p2 ;
		}
		else
			vincitore = p1 ;
	} else if(carta_p2.seme == briscola) {
		vincitore = p2 ;
	} else {
		if(carta_p1.seme != carta_p2.seme)
			vincitore = p1 ;
		else {
			if(&carta_p1==confronta_valori(&carta_p1, &carta_p2))
				vincitore = p1 ;
			else
				vincitore = p2 ;
		}
	}
	
	assert(vincitore != NULL) ;
	D1(cout<<vincitore->nome<<" prende!"<<endl<<endl) ;
	p1->ultima_presa = p2->ultima_presa = false ;
	vincitore->ultima_presa = true ;
	vincitore->punteggio += (carta_p1.punteggio+carta_p2.punteggio) ;
}

/** Setta le coordinate delle carte in mano ai giocatori.
 *
 * @param[in] mano_g1 Mano dell'utente
 * @param[in] mano_cpu Mano della CPU
 *
 * La funzione viene invocata all'inizio di ogni turno per settare correttamente
 * le coordinate delle carte nelle mani dei giocatori, così che vengano stampate
 * nella posizione corretta.
 * @sa ::disegna_tavolo_gioco, ::start_game.
 */
static void setta_coordinate_carte(mazzo &mano_g1, mazzo &mano_cpu)
{
	assert(mano_g1.carte != NULL && mano_cpu.carte != NULL) ;
	
	int x = x_centro - (imagesWidth*mano_cpu.num_carte)/2 ;
	int y = 0 ;
	
	for (int i=0; i<mano_cpu.num_carte; i++) {
		mano_cpu.carte[i].x_img = x ;
		mano_cpu.carte[i].y_img = y ;
		x += imagesWidth ;
	}
	
	x = x_centro - (imagesWidth*mano_g1.num_carte)/2 ;
	y = H_DISPLAY - imagesHeight ;
	
	for (int i=0; i<mano_g1.num_carte; i++) {
		mano_g1.carte[i].x_img = x ;
		mano_g1.carte[i].y_img = y ;
		x += imagesWidth ;
	}
}

bool start_game(giocatore &g1, giocatore &cpu, ALLEGRO_DISPLAY *display)
{
	assert(display != NULL && g1.mano.carte != NULL 
		&& cpu.mano.carte != NULL)  ;
	
	/* Inizializza struttura dati di appoggio al gioco */
	bool chiusura_forzata = false ;
	for(int i=0; i<4; i++)
		carichi_giocati[i]=0 ;

	mazzo m ;
	inizializza_mazzo(m) ;
	mescola_mazzo(m) ;

	const semi briscola = m.carte[0].seme ;
	D1(cout<<"L'ultima carta è il " ) ;
	D1(stampa_mazzo(&m.carte[0], 1)) ;

	for(int i=0; i<2; i++) {
		pesca(g1.mano, m) ;
		pesca(cpu.mano, m) ;
	}

	int turno=1 ;
	g1.ultima_presa = true ;
	
	/* Loop di gioco */
	while(g1.mano.num_carte > 0 || cpu.mano.num_carte > 0) {
		D1(cout<<"Turno "<<turno<<endl<<endl) ;
		/* Il giocatore che ha vinto l'ultima mano pesca per primo */
		if(m.num_carte > 0) {
			if(g1.ultima_presa) {
				pesca(g1.mano, m) ;
				pesca(cpu.mano, m) ;
				D1(cout<<"Gioca prima "<<g1.nome<<endl) ;
			} else {
				pesca(cpu.mano, m) ;
				pesca(g1.mano, m) ;
				D1(cout<<"Gioca prima "<<cpu.nome<<endl) ;
			}
		}
		
		setta_coordinate_carte(g1.mano, cpu.mano) ;
		disegna_tavolo_gioco(g1.mano, cpu.mano, m, g1.punteggio) ;

		/* Stampe di debug se quest'ultimo è attivo */
		D1(cout<<"Mano:"<<endl) ;
		D1(stampa_mazzo(g1.mano.carte, g1.mano.num_carte)) ;
		D1(cout<<"Punteggio: "<<g1.punteggio<<endl) ;
		D1(cout<<"Briscola:\t") ;
		D1(stampa_mazzo(&m.carte[0], 1)) ;
		D1(cout<<"Carte nel mazzo: "<<m.num_carte<<endl<<endl) ;
		int scelta_p1, scelta_p2 ;
		
		/* Scelta carte giocatori */
		if(g1.ultima_presa) {
			if(!scegli_carta_p1(g1.mano, scelta_p1, display)) {
				chiusura_forzata = true ;
				break ;
			}
			animazione_carta(g1, cpu, m , g1.punteggio, scelta_p1) ;
			
			scegli_carta_p2(cpu.mano, scelta_p2, 
				&g1.mano.carte[scelta_p1], briscola) ;
			animazione_carta(cpu, g1, m , g1.punteggio, scelta_p2) ;
			
			attribuisci_punti(scelta_p1, scelta_p2, &g1, 
				&cpu, briscola) ;
				
			estrai_carta(g1.mano, scelta_p1) ;
			estrai_carta(cpu.mano, scelta_p2) ;
		} else {
			scegli_carta_p2(cpu.mano, scelta_p2, NULL, briscola) ;
			animazione_carta(cpu, g1, m , g1.punteggio, scelta_p2) ;
			
			if(!scegli_carta_p1(g1.mano, scelta_p1, display)) {
				chiusura_forzata = true ;
				break ;
			}
			animazione_carta(g1, cpu, m , g1.punteggio, scelta_p1) ;
			
			attribuisci_punti(scelta_p2, scelta_p1, &cpu,
				&g1, briscola) ;
				
			estrai_carta(cpu.mano, scelta_p2) ;
			estrai_carta(g1.mano, scelta_p1) ;
		}
		al_rest(1.0) ;
		turno++ ;		
	}
	
	/* Stampa schermata finale */
	if(!chiusura_forzata) {
		if(g1.punteggio > cpu.punteggio)
			chiusura_forzata=(!disegna_schermata_finale(VITTORIA, 
				g1.punteggio, display)) ;
		else if(g1.punteggio == cpu.punteggio)
			chiusura_forzata=(!disegna_schermata_finale(PAREGGIO,
				g1.punteggio, display)) ;
		else
			chiusura_forzata=(!disegna_schermata_finale(SCONFITTA,
				g1.punteggio, display)) ;
	}

	for(int i=0; i<NUM_CARTE_MAZZO; i++)
		al_destroy_bitmap(m.carte[i].img) ;
	delete [] m.carte ;
	return !chiusura_forzata ;
}

// Fine definizioni delle funzioni del modulo Gestione gioco.
