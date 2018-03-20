/**
 * @file
 * Header file contenente le dichiarazioni della struttura dati comune.
 */

#ifdef DEBUG_MODE

/**
 * Se definita la macro DEBUG_MODE, attivo i livelli di debug a seconda della
 * maschera che mi viene passata in fase di compialzione (anch'essa attraverso 
 * una macro).
 */
#define DBG(A, B) { if((A) & MASK) { B; } }

#else

#define NDEBUG
#define DBG(A, B)

#endif

#define D1(a) DBG(1, a)
#define D2(a) DBG(2, a)

#include <cassert>

/* Inizio header del modulo Struttura Dati */

/**
 * Tipo seme delle carte.
 * Ogni carta ha un seme. @sa ::carta per maggiori dettagli implementativi.
 */
enum semi { DENARI=0, COPPE, BASTONI, SPADE } ;

/**
 * Tipologia di carta.
 *
 * Ad ogni carta, a seconda del suo seme e punteggio, viene attribuita una di
 * queste quattro categorie dall'Intelligenza Artificiale per valutare la
 * giocata migliore da fare contro di essa. @sa ::determina_tipo_carta per
 * maggiori dettagli e ::scegli_carta_p2 per il funzionamento dell'IA.
 */
enum tipi_carta { BRISCOLA, CARICO, BRISCOLA_E_CARICO, LISCIO } ;

/** Esito della partita. Utilizzato per disegnare la schermata finale da
 * ::disegna_schermata_finale
 */
enum esiti_partita { VITTORIA, PAREGGIO, SCONFITTA } ;

/**
 * Struttura della singola carta.
 * 
 * Ogni carta è caratterizzata in particolare da un seme e da un numero, dal
 * quale viene determinato il suo punteggio. @sa ::inizializza_mazzo per
 * l'inizializzazione di tutte le carte.
 */
struct carta {
	semi seme ; /**< Seme della carta. @sa ::semi. */
	int numero ; /**< Numero della carta, da 1 a 10. */
	int punteggio ; /**< Punteggio della carta. */
	bool presa ; /**< Booleano di supporto alla funzione ::mescola_mazzo */
	bool giocata ; /**< Booleano di supporto alla funzione
			* ::disegna_tavolo_gioco
			*/
	float x_img, y_img ; /**< Coordinate della carta nel tavolo da gioco.
			      * @sa ::animazione_carta per maggiori dettagli.
			      */
	ALLEGRO_BITMAP *img ; /**< Campo contenente l'immagine della carta. */
} ;

/**
 * Descrittore del mazzo.
 *
 * Ogni mazzo di carte è implementato attraverso un array dinamico di carte. @sa
 * ::inizializza_mazzo per maggiori dettagli.
 */
struct mazzo {
	int num_carte ; /**< Numero massimo di carte nel mazzo. */
	carta* carte ; /**< Puntatore all'array dinamico di carte. */
} ;

/**
 * Struttura per descrivere un giocatore.
 *
 * In generale, un giocatore è caratterizzato dal nome, dalla mano (implementata)
 * attraverso un ::mazzo ), e il punteggio all'interno della partita. @sa
 * ::inizializza_giocatore per ulteriori informazioni.
 */
struct giocatore {
	char nome[4] ;
	mazzo mano ;
	int punteggio ;
	bool ultima_presa ; /**< Booleano di supporto alla funizione ::start_game,
			     * indica se il giocatore ha vinto l'ultima mano.
			     */
	bool utente ; /**< Indica se il giocatore è comandato dall'utente o
		       * dall'IA.
		       */
	int x_giocata, y_giocata ; /**< Coordinate x e y del punto del campo
				    * di gioco in cui le carte del giocatore
				    * devono essere giocate. @sa ::animazione_carta
				    * per maggiori dettagli.
				    */
} ;

extern const int NUM_CARTE_MANO ; /**< Numero massimo delle carte nella mano
				   * di un giocatore.
				   */
extern const int NUM_CARTE_MAZZO ; /**< Numero massimo delle carte nel mazzo. */
extern int carichi_giocati[4] ; /**< Array di supporto per l'IA, indica quanti
				 * "carichi" sono già stati giocati nella partita
				 * per ogni seme. @sa ::scegli_carta_p2.
				 */

extern const int W_DISPLAY ; /**< Larghezza display in pixel. */
extern const int H_DISPLAY ; /**< Altezza display in pixel. */
extern const int x_centro ; /**< Coordinata x del centro della schermata. */
extern const int y_centro ; /**< Coordinata y del centro della schermata. */
extern const int dx, dy ; /**< Frazioni di schermo utilizzate per determinare la grandezza
			   * dei bottoni e le spaziature tra di essi. @sa Briscola.cc per
			   * le definizioni delle coordinate dei bottoni e ::disegna_schermata_iniziale
			   * per il disegno della schermata.
			   */
extern const int x_gioca_dx, x_gioca_sx, /**< Coordinate x del pulsante "GIOCA" 
					  * nella schermata principale. @sa
					  * ::disegna_schermata_iniziale.
					  */
		 y_gioca_down, y_gioca_up ; /**< Coordinate y del pulsante "GIOCA" 
					     * nella schermata principale. @sa
					     * ::disegna_schermata_iniziale.
					     */
extern const int x_esci_dx, x_esci_sx, /**< Coordinate x del pulsante "ESCI" 
					* nella schermata principale. @sa
					* ::disegna_schermata_iniziale.
					*/
		 y_esci_down, y_esci_up ; /**< Coordinate y del pulsante "ESCI" 
					   * nella schermata principale. @sa
					   * ::disegna_schermata_iniziale.
					   */
extern const int x_continua_dx, x_continua_sx, /**< Coordinate x del pulsante
						* "CONTINUA" nella schermata finale. @sa
						* ::disegna_schermata_finale.
						*/
		 y_continua_down, y_continua_up ; /**< Coordinate y del pulsante
						   * "CONTINUA" nella schermata finale. @sa
						   * ::disegna_schermata_finale.
						   */
extern const int X_GIOCATA_CPU, X_GIOCATA_G1 ; /**< Coordinate x delle posizioni in cui verranno
						* giocate le carte dell'utente e della CPU.
						*/
extern int imagesWidth,   /**< Larghezza in pixel dei bitmap caricati per le immagini delle carte. */
	   imagesHeight ; /**< Altezza in pixel dei bitmap caricati per le immagini delle carte. */
	  
extern ALLEGRO_BITMAP *img_retro ; /**< Bitmap contenente il retro delle carte. */
extern ALLEGRO_FONT *agentorange ; /**< Font utilizzato nella schermata principale. */
extern ALLEGRO_FONT *bebasneue ; /**< Font utilizzato nelle scritte all'interno
				  * dei pulsanti
				  */

/* Fine header modulo Struttura Dati */
