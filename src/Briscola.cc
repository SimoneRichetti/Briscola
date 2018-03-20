/**
 * @mainpage Briscola
 * 
 * Programma che implementa il gioco della Briscola a due giocatori. Permette
 * di giocare contro un'intelligenza artificiale.
 *
 * Vedi la documentazione della funzione ::main per maggiori dettagli.
 *
 * @author Simone Richetti 
 */
 
/**
 * @file
 * File contenente il modulo Main.
 */

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

using namespace std;

#include "strutt_dati.h"
#include "grafica_debug.h"
#include "gestione_gioco.h"

/*
 * Vedi strutt_dati.h per la documentazione relativa alle strutture dati e
 * alle variabili globali.
 */
 
// Inizio definizioni modulo Main

const int NUM_CARTE_MANO = 3 ;
const int NUM_CARTE_MAZZO = 40 ;
int carichi_giocati[4] ;

const int W_DISPLAY = 800 ;
const int H_DISPLAY = 600 ;
const int x_centro = W_DISPLAY/2 ;
const int y_centro = H_DISPLAY/2 ;
const int dx = W_DISPLAY / 6 ;
const int dy = H_DISPLAY / 10 ;
const int x_gioca_sx = x_centro - (dx/2),
	x_gioca_dx = x_centro + (dx/2),
	y_gioca_up = y_centro+dy,
	y_gioca_down = y_centro + 2*dy ;
const int x_esci_sx = x_centro - (dx/2),
	x_esci_dx = x_centro + (dx/2),
	y_esci_up = y_centro + (3*dy),
	y_esci_down = y_centro + (4*dy) ;
const int x_continua_sx = x_centro-dx,
	x_continua_dx = x_centro+dx,
	y_continua_up = y_centro+2*dy,
	y_continua_down = y_centro+3*dy ;
const int X_GIOCATA_G1 = x_centro ,
	X_GIOCATA_CPU = x_centro + 100 ;
int imagesWidth, imagesHeight ;
	  
ALLEGRO_BITMAP *img_retro ;
ALLEGRO_FONT *agentorange ;
ALLEGRO_FONT *bebasneue ;

/**
 * Inizializza la libreria Allegro 5 e alcune variabili del programma,
 * disegna la schermata iniziale e, a seconda dell'input della schermata:
 * - Avvia una partita;
 * - Termina il programma.
 *
 * Al termine della partita, il programma ritornerà nel ciclo while, ridisegnando
 * la schermata e attendendo nuovamente l'input.
 * @sa ::disegna_schermata_iniziale per la documentazione relativa alla schermata,
 * ::start_game per il ciclo di gioco e ::inizializza_giocatore per l'inizializzazione
 * di G1 e CPU
 */
int main()
{
	/* Inizializzazione Allegro */	
	if(!al_init()) return 1 ;
	if(!al_init_primitives_addon()) return 2 ;
	al_init_font_addon() ;
	if(!al_init_ttf_addon()) return 3 ;
	if(!al_install_mouse()) return 4 ;
	if(!al_init_image_addon()) return 5 ;

	/* Settaggio di variabili per il gioco */
	img_retro = al_load_bitmap("data/bg.png") ;
	imagesWidth = al_get_bitmap_width(img_retro) ;
	imagesHeight = al_get_bitmap_height(img_retro) ;
	giocatore g1, cpu ;
	g1.mano.carte = NULL ;
	cpu.mano.carte = NULL ;	
	
	/* Disegno schermata e avvio loop di input */
	ALLEGRO_DISPLAY *display = al_create_display(W_DISPLAY, H_DISPLAY) ;
	al_set_window_title(display, "BRISCOLA") ;	
	disegna_schermata_iniziale() ;
	
	
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue() ;
	al_register_event_source(event_queue, al_get_mouse_event_source()) ;
	al_register_event_source(event_queue,
		al_get_display_event_source(display)) ;
	while(true) {
		ALLEGRO_EVENT event ;
		al_wait_for_event(event_queue, &event) ;
		if(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP){
			int x = event.mouse.x ;
			int y = event.mouse.y ;
			if((x>x_gioca_sx && x<x_gioca_dx)&&(y>y_gioca_up &&
			    y<y_gioca_down)) {
				inizializza_giocatore(g1, "G1", true) ;
				inizializza_giocatore(cpu, "CPU", false) ;

				if(!start_game(g1, cpu, display)) {
					D1(cout<<"CHIUSURA FORZATA"<<endl) ;
					break ;
				}
				
				if(!al_is_event_queue_empty(event_queue))
					al_flush_event_queue(event_queue) ;
				disegna_schermata_iniziale();
			} else if ((x>x_esci_sx && x<x_esci_dx) &&
				   (y>y_esci_up && y<y_esci_down))
				break ;	
		} else if(event.type==ALLEGRO_EVENT_DISPLAY_CLOSE) {
			D1(cout<<"CHIUSURA FORZATA"<<endl) ;
			break ;
		}
	}
	
	/* Deallocazione memoria dinamica */
	if(g1.mano.carte != NULL)
		delete [] g1.mano.carte ;
	if(cpu.mano.carte != NULL)
		delete [] cpu.mano.carte ;
	if(event_queue != NULL)	
		al_destroy_event_queue(event_queue) ;
	if(display != NULL)
		al_destroy_display(display) ;
	if(img_retro != NULL)
		al_destroy_bitmap(img_retro) ;
	
	return 0 ;
}

// Fine definizioni modulo Main
