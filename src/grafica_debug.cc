/**
 * @file
 * File contenente i moduli Debug e Grafica.
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

using namespace std ;

#include "strutt_dati.h"

#define VERDE al_map_rgb(0,150,0)
#define NERO al_map_rgb(0,0,0)
#define GIALLO al_map_rgb(255, 255, 0)

// Inizio definizioni delle funzioni del modulo Debug.

/** Stampa il seme di una carta
 *
 * @param[in] c Carta di cui stampare il seme
 * @sa ::semi, ::carta, ::stampa_mazzo.
 */
static void stampa_seme(const carta &c)
{
	switch(c.seme) {
		case DENARI: cout<<"denari" ; break ;
		case COPPE: cout<<"coppe" ; break ;
		case BASTONI: cout<<"bastoni" ; break ;
		case SPADE: cout<<"spade" ; break ;
	}
}

void stampa_mazzo(carta* m, const int n_carte)
{
	assert(m != NULL) ;
	for(int i=0; i<n_carte; i++) {
		cout<<m[i].numero<<" di " ;
		stampa_seme(m[i]) ;
		cout<<endl ;
	}
}

// Fine definizioni delle funzioni del modulo Debug.

// Inizio definizioni delle funzioni del modulo Grafica.

void disegna_tavolo_gioco(const mazzo &mano_g1, const mazzo &mano_cpu,
	const mazzo &m, const int punteggio)
{
	assert(mano_g1.carte != NULL && mano_cpu.carte != NULL && m.carte != NULL) ;
	
	al_draw_filled_rectangle(0, 0, W_DISPLAY, H_DISPLAY, VERDE) ;
	
	/* Disegna le carte per ciascun giocatore */
	for(int i=0; i<mano_cpu.num_carte; i++) {
		if(mano_cpu.carte[i].giocata)
			al_draw_bitmap(mano_cpu.carte[i].img, mano_cpu.carte[i].x_img,
				mano_cpu.carte[i].y_img, NULL) ;
		else
			al_draw_bitmap(img_retro, mano_cpu.carte[i].x_img,
				mano_cpu.carte[i].y_img, NULL) ;
	}
	
	for(int i=0; i<mano_g1.num_carte; i++)
		al_draw_bitmap(mano_g1.carte[i].img, mano_g1.carte[i].x_img,
			mano_g1.carte[i].y_img, NULL) ;

	bebasneue = al_load_font("data/BebasNeue.otf", 36, NULL) ;
	assert(bebasneue != NULL) ;
	
	/* Disegna mazzo di carte con briscola ruotata */
	if(m.num_carte > 1) {	
		al_draw_rotated_bitmap(m.carte[0].img, imagesWidth/2, imagesHeight/2,
			x_centro - imagesHeight, y_centro, 3.14/2, NULL) ;	
		al_draw_bitmap(img_retro, x_centro - 3*imagesWidth,
			y_centro - imagesHeight/2, NULL) ;
	
		/* Disegna numero carte rimaste nel mazzo */
		char stampa_num_carte[3] ;
		sprintf(stampa_num_carte, "%d", m.num_carte) ;
		al_draw_text(bebasneue, GIALLO, x_centro - 3.5*imagesWidth,
			y_centro-imagesHeight/6, ALLEGRO_ALIGN_LEFT, stampa_num_carte) ;
	}
	
	/* Crea stringa punteggio e stampa */
	char stampa_punteggio[15] = "PUNTEGGIO: " ;
	char str[4] ;
	sprintf(str, "%d", punteggio) ;
	strcat(stampa_punteggio, str) ;
	al_draw_text(bebasneue, GIALLO, W_DISPLAY-200, H_DISPLAY-50,
		ALLEGRO_ALIGN_LEFT , stampa_punteggio) ;
	
	al_flip_display() ;
	al_destroy_font(bebasneue) ;
}

void animazione_carta(giocatore &g1, giocatore &g2, const mazzo &m,
	const int punteggio, const int idx)
{
	assert(g1.mano.carte != NULL) ;
	
	carta *c = &g1.mano.carte[idx] ;
	
	/* Determina l'incremento della coordinata x e della coordinata y per
	 * ottenere un'animazione diagonale
	 */
	float dx = 4*abs(c->x_img - g1.x_giocata)/abs(c->y_img - g1.y_giocata),
	      dy = 4 ;
	while(abs(c->x_img-g1.x_giocata)>1||abs(c->y_img-g1.y_giocata)>1) {
		if(c->x_img > g1.x_giocata)
			c->x_img-=dx ;
		else if(c->x_img < g1.x_giocata)
			c->x_img+=dx ;
			
		if(c->y_img > g1.y_giocata)
			c->y_img-=dy ;
		else if(c->y_img < g1.y_giocata)
			c->y_img+=dy ;
		
		/* disegna_tavolo_gioco presuppone che vengano passati l'utente e
		 * la CPU nell'ordine, mentre questa funzione prende in ingresso
		 * prima il giocatore di cui deve animare la carta poi l'avversario.
		 * Perciò, utilizzo il booleano utente per passarli correttamente.
		 */
		disegna_tavolo_gioco(g1.utente?g1.mano:g2.mano,
			g1.utente?g2.mano:g1.mano, m, punteggio) ;
	}
}

bool disegna_schermata_finale(esiti_partita esito, const int punteggio_g1,
	ALLEGRO_DISPLAY* display)
{
	assert(display != NULL) ;
	
	char scritta_esito[10] ;
	ALLEGRO_FONT *font_scritta=NULL ;
	bebasneue = al_load_font("data/BebasNeue.otf", 40, NULL) ;
	
	/* A seconda dell'esito, determino la scritta da stampare e il font utilizzato */
	switch(esito) {
		case VITTORIA:
			strcpy(scritta_esito, "VITTORIA") ;
			font_scritta=al_load_font("data/BD_Cartoon_Shout.ttf",
				100, NULL) ;
			break ;
		case PAREGGIO:
			strcpy(scritta_esito, "PAREGGIO") ;
			font_scritta=al_load_font("data/Snaps Taste.otf",
				150, NULL) ;
			break ;
		case SCONFITTA:
			strcpy(scritta_esito, "SCONFITTA") ;
			font_scritta=al_load_font("data/Kraash Black.ttf",
				75, NULL) ;
			break ;
		default:
			exit(7) ;
	}
	
	/* Crea stringa punteggio */
	int punteggio_cpu = 120 - punteggio_g1 ;
	char str_punt_g1[4], str_mezzo[]=" - ", str_punt_g2[4],
		str_punt_finale[10]="" ;
	sprintf(str_punt_g1, "%d", punteggio_g1) ;
	strcat(str_punt_finale, str_punt_g1) ;
	strcat(str_punt_finale, str_mezzo) ;
	sprintf(str_punt_g2, "%d", punteggio_cpu) ;
	strcat(str_punt_finale, str_punt_g2) ;
	
	/* Disegno esito, punteggi finali e bottone continua */
	al_draw_filled_rectangle(0, 0, W_DISPLAY, H_DISPLAY, VERDE) ;
	al_draw_text(font_scritta, GIALLO, x_centro, y_centro - 2*dy,
		     ALLEGRO_ALIGN_CENTRE, scritta_esito) ;
	al_draw_text(bebasneue, GIALLO, x_centro, y_centro+0.5*dy,
		ALLEGRO_ALIGN_CENTRE, str_punt_finale) ;
	al_draw_filled_rectangle(x_continua_sx, y_continua_up, x_continua_dx,
		y_continua_down, GIALLO) ;
	al_draw_rectangle(x_continua_sx, y_continua_up, x_continua_dx,
		y_continua_down, NERO, 2.0) ;
	al_draw_text(bebasneue, NERO, x_centro, y_continua_up+dy/6,
		ALLEGRO_ALIGN_CENTRE, "CONTINUA") ;
	al_flip_display() ;
	
	/* Leggo input utente */
	ALLEGRO_EVENT_QUEUE *event_queue=al_create_event_queue() ;
	al_register_event_source(event_queue, al_get_mouse_event_source()) ;
	al_register_event_source(event_queue, al_get_display_event_source(display)) ;
	bool chiusura_forzata=false ;
	while(true) {
		ALLEGRO_EVENT event ;
		al_wait_for_event(event_queue, &event) ;
		if(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			int x = event.mouse.x ;
			int y = event.mouse.y ;
			if((x>x_continua_sx && x<x_continua_dx)&&
			   (y>y_continua_up && y<y_continua_down))
				break ;
		} else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			chiusura_forzata=true ;
			break ;
		}
	}
	
	al_destroy_font(bebasneue) ;
	al_destroy_font(font_scritta) ;
	al_destroy_event_queue(event_queue) ;
	
	return !chiusura_forzata ;	
}

void disegna_schermata_iniziale()
{
	agentorange = al_load_font("data/AGENTORANGE.TTF", 100, NULL) ;
	bebasneue = al_load_font("data/BebasNeue.otf", 36, NULL) ;
	
	al_draw_filled_rectangle(0, 0, W_DISPLAY, H_DISPLAY, VERDE) ;
	
	al_draw_filled_rectangle(x_gioca_sx, y_gioca_up, x_gioca_dx, y_gioca_down,
				 GIALLO) ;
	al_draw_rectangle(x_gioca_sx, y_gioca_up, x_gioca_dx, y_gioca_down,
			  NERO, 2.0) ;
	al_draw_filled_rectangle(x_esci_sx, y_esci_up, x_esci_dx, y_esci_down,
				 GIALLO) ;
	al_draw_rectangle(x_esci_sx, y_esci_up, x_esci_dx, y_esci_down, NERO,
			  2.0) ;
	al_draw_text(agentorange, GIALLO, x_centro, y_centro - 2.5*dy,
		     ALLEGRO_ALIGN_CENTRE, "BRISCOLA") ;
	al_draw_text(bebasneue, NERO, x_centro, y_gioca_up + dy/6,
		     ALLEGRO_ALIGN_CENTRE, "GIOCA") ;
	al_draw_text(bebasneue, NERO, x_centro, y_esci_up + dy/6,
		     ALLEGRO_ALIGN_CENTRE, "ESCI") ;
	
	al_flip_display() ;
	
	al_destroy_font(agentorange) ;
	al_destroy_font(bebasneue) ;
}

// Fine definizioni delle funzioni del modulo Grafica.
