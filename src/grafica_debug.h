/**
 * @file
 * Header file contenente l'interfaccia dei moduli Debug e Grafica.
 */

// Inizio interfaccia modulo Debug.

/** Stampa informazioni di tutte le carte del mazzo
 *
 * @param[in] m Array di struct ::carta
 * @param[in] n_carte Numero di carte da stampare
 * @sa ::stampa_seme.
 */
void stampa_mazzo(carta* m, const int n_carte) ;

// Fine interfaccia modulo Debug.

// Inizio interfaccia modulo Grafica.

/** Stampa il tavolo da gioco.
 *
 * @param[in] mano_g1 Mano dell'utente
 * @param[in] mano_cpu Mano dell'IA
 * @param[in] m Mazzo di gioco
 * @param[in] punteggio Punteggio dell'utente
 *
 * La funzione si occupa di stampare a schermo il mazzo di carte con il numero
 * di carte rimaste in esso, la briscola ruotata, il punteggio del giocatore, 
 * e infine tutte le carte della CPU e dell'utente, utilizzando le loro coordinate 
 * (vedi la struct ::carta). Se la CPU sta giocando una carta allora stamperà la 
 * sua immagine, se no verrà stampato il retro delle carte.
 * @sa ::start_game e ::animazione_carta per esempi del suo utilizzo.
 */
void disegna_tavolo_gioco(const mazzo &mano_g1, const mazzo &mano_cpu,
	const mazzo &m, const int punteggio) ;

/** Gestisce le animazioni legate alle carte.
 *
 * @param[in] mano_g1 Mano dell'utente
 * @param[in] mano_cpu Mano dell'IA
 * @param[in] m Mazzo di gioco
 * @param[in] punteggio Punteggio dell'utente
 *
 * La funzione gestisce l'animazione legata alla giocata di una carta. Considera
 * la carta data dalla mano passata come mano_g1 e dall'indice, incrementando o
 * decrementando ciclicamente le sue coordinate fino a raggiungere le coordinate
 * relative al giocatore che l'ha giocata (vedi l'implementazione di ::giocatore).
 * Ad ogni ciclo viene chiamata la funzione ::disegna_tavolo_gioco per
 * aggiornare la schermata.
 */
void animazione_carta(giocatore &g1, giocatore &g2, const mazzo &m,
	const int punteggio, const int idx) ;

/** Disegna la schermata con il risultato finale
 *
 * @param[in] esito L'esito finale della partita
 * @param[in] punteggio_g1 Punteggio finale dell'utente
 * @param[in] display Schermata del gioco
 *
 * La funzione prende in ingresso l'esito della partita e, in relazione ad esso,
 * carica il font e stampa il risultato finale. Dopodichè attende l'input dell'utente,
 * che può tornare al menu principale o chiudere il programma.
 * @sa ::esiti_partita, ::main, ::start_game
 */
bool disegna_schermata_finale(esiti_partita esito, const int punteggio_g1,
	ALLEGRO_DISPLAY* display) ;

/** Disegna la schermata con il menu iniziale.
 *
 * La funzione disegna la schermata iniziale contenente il titolo e il menu, il
 * quale permette di:
 * - cominciare una nuova partita
 * - terminare il programma
 * @sa ::main.
 */
void disegna_schermata_iniziale() ;

//Fine interfaccia modulo Grafica.
