/**
 * @file
 * Header file contenente l'interfaccia del modulo Gestione gioco.
 */

// Inizio interfaccia modulo Gestione gioco.

/** Inizializzazione della struttura giocatore
 *
 * @param[in] g1 Giocatore da inizializzare
 * @param[in] nome_giocatore Nome del giocatore da assegnare a g1
 * @param[in] u Mi dice se g1 è l'utente o la cpu.
 *
 * Viene invocata nel main per inizializzare i 2 giocatori (utente e CPU) prima
 * dell'avvio del gioco.
 *
 * @sa ::main per vedere il codice in cui viene invocata, ::giocatore per vedere
 * i campi della struttura.
 */
void inizializza_giocatore(giocatore &g1, const char nome_giocatore[], bool u) ;

/** Implementazione del ciclo di gioco.
 *
 * @param[in] g1 Struct giocatore dell'utente
 * @param[in] cpu Struct giocatore dell'IA
 * @param[in] display Display corrispondente alla schermata del gioco
 * @return true nel caso il gioco sia finito correttamente, false se l'utente
 * chiude la finestra e non termina la partita
 *
 * La funzione è utilizzata per gestire l'intero ciclo di gioco. Inizializza il
 * mazzo, le mani dei giocatori e la struttura dati di appoggio e gestisce il loop
 * di gioco, il quale consiste nel:
 * - Pescare
 * - Ricevere l'input dell'utente per la carta da giocare
 * - Elaborare la scelta dell'IA
 * - Valutare il vincitore
 * 
 * @sa ::inizializza_mazzo e ::mescola_mazzo per la gestione del mazzo,
 * ::setta_coordinate_carte , ::disegna_tavolo_gioco e ::animazione_carta per la
 * grafica del tavolo, ::scegli_carta_p1 per la lettura dell'input dell'utente,
 * ::scegli_carta_p2 per l'IA e ::disegna_schermata_finale per la grafica del
 * risultato della partita. 
 */
bool start_game(giocatore &g1, giocatore &cpu, ALLEGRO_DISPLAY *display) ;

// Fine interfaccia modulo Gestione gioco.
