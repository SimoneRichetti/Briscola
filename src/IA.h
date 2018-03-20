/**
 * @file
 * Header file contenente l'interfaccia del modulo IA.
 */

// Inizio interfaccia modulo IA.

/** Confronta due carte dello stesso seme e ritorna quella che vince la mano.
 *
 * @param[in] carta_p1 Carta giocata per prima
 * @param[in] carta_p2 Carta giocata per seconda
 * @return un puntatore alla carta che vince la mano tra le due.
 * @sa ::attribuisci_punti e ::scegli_carta_p2 per un esempio del suo utilizzo.
 */
carta* confronta_valori(carta *carta_p1, carta *carta_p2) ;

/** Implementa l'Intelligenza Artificiale dell'avversario.
 *
 * @param[in] mano Mano del giocatore CPU
 * @param[out] idx Indice della carta scelta
 * @param[in] c Carta giocata dall'utente (NULL se CPU gioca per prima)
 * @param[in] briscola Seme che determina la briscola nella partita
 *
 * L'algoritmo che implementa l'Intelligenza Artificiale si basa sul dare un valore
 * ad ogni possibile giocata, ovvero ad ogni carta, e di conseguenza giocare la
 * carta migliore (più il valore assegnato è piccolo, migliore è la giocata).
 * La funzione, dopo aver inizializzato alcune strutture dati di supporto,
 * innanzituto guarda se la CPU è la prima a giocare o la seconda:
 * se è la prima, applica sempre lo stesso criterio di valutazione, se è la seconda,
 * prima valuta la carta c giocata precedentemente dal'utente (assegnandole 
 * un ::tipi_carta), quindi in base ad essa valuta le varie giocate.
 *
 * @sa ::valuta_giocata_carico_briscola, ::valuta_giocata_carico,
 * valuta_giocata_briscola e ::valuta_giocata_liscio per le varie scelte in base
 * alla carta giocata c .
 */
void scegli_carta_p2(const mazzo &mano, int &idx, carta *c, semi briscola) ;

// Fine interfaccia modulo IA.
