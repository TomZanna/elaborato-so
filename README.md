# Connect 4 ![main workflow](https://github.com/TomZanna/elaborato-so/actions/workflows/ubuntu.yml/badge.svg)

## Introduzione
Implementazione del gioco [Forza 4](https://en.wikipedia.org/wiki/Connect_Four) utilizzando le [system call System V](https://man7.org/linux/man-pages/man7/sysvipc.7.html) per l'elaborato di sistemi operativi (2022-23).

## Directory
* `.vscode`: contiene i file di configurazione per l'editor Visual Studio Code
* `subprojects`
  * `client`: sorgenti dell'eseguibile che si interfaccia con l'utente
  * `server`: sorgenti dell'eseguibile che instanzia il campo di gioco e coordina i due giocatori
  * `libconnect4`: libreria con il codice necessario allo scambio di dati tra i due componenti
* `tools`: contiene il binario `muon` utilizzabile per compilare il progetto

Il progetto usa [meson](https://mesonbuild.com/) come build system. I file `meson.build` contengono le direttive per compilare i binari.

## Compilazione
1. Installare un compilatore per il linguaggio C (es. `gcc`), il build system `meson` ed il backend `ninja`
2. Creare una directory per la compilazione: `meson setup builddir`
3. Compilare il progetto: `meson compile -C builddir`
4. Eseguire i binari `connect4-client` e `connect4-server` presenti nella directory `builddir` in terminali diversi

> Anzichè installare `meson` e `ninja` è possibile utilizzare il binario `muon`. Per ulteriori informazioni si rimanda alla [documentazione](https://muon.build/) di quest'ultimo.

## Utilizzo
### Server
`connect4-server [-t TIMEOUT] GRIDHEIGHT GRIDWIDTH P1TOKEN P2TOKEN`
- `-t TIMEOUT`: parametro opzionale per indicare i secondi entro cui ogni giocatore deve fare la propria mossa. Se 0 indica che non c'è timeout (default 0)
- `GRIDHEIGHT`: l'altezza della griglia di gioco (minimo 5)
- `GRIDWIDTH`: la larghezza della griglia di gioco (minimo 5)
- `P1TOKEN`: un carattere che rappresenta il simbolo assegnato al giocatore 1
- `P2TOKEN`: un carattere che rappresenta il simbolo assegnato al giocatore 2

### Client
`connect4-client USERNAME GAME_ID`
- `GAME_ID`: id generato dal server che identifica la partita
- `USERNAME`: username con cui sarà identificato il giocatore

## Architettura
I processi client e server comunicano attraverso una coda di messaggi, alcuni semafori ed i segnali SIGUSR1 e SIGUSR2.
Inoltre essi condividono una zona di memoria in cui é memorizzata la griglia di gioco.

In particolare sulla coda vengono scambiati i seguenti messaggi:
- messaggi di configurazione, servono a comunicare ai client l'ID della memoria condivisa e dei semafori, le dimensioni della griglia di gioco, il token assegnato al giocatore, il PID del processo server e l'eventuale timeout.
- messaggi di stato, inviati dal server verso i client, contengono il risultato della partita in corso (PERSA, PAREGGIO, VINTA o VINTA A TAVOLINO)
- messaggi di feedback, inviati dal client verso il server. Contengono il PID del client e servono a comunicare quando il client si è connesso o sta abbandonando la partita

I semafori utilizzati invece sono i seguenti:
- connessione client (intero): viene incrementato dai processi client durante la connessione.
- turno: (binario, uno per client) viene incrementato dal server per sbloccare il rispettivo giocatore
- mossa (binario, uno per client): viene incrementato dal client per indicare che é stata eseguita la mossa

Per quanto riguarda i segnali abbiamo il server che utilizza:
- `SIGUSR1`: per capire quando un nuovo messaggio viene posto nella coda
- `SIGINT`: per terminare la partita per entrambi i giocatori

I processi client invece reagiscono ai seguenti segnali:
- `SIGUSR1`: per capire quando viene posto un nuovo messaggio nella coda
- `SIGUSR2`: per capire quando il server ordina la chiusura unilaterale della partita
- `SIGINT`: per permettere al giocatore di abbandonare la partita, perdendo a tavolino
- `SIGALRM`: utilizzato quando la partita prevede timeout
