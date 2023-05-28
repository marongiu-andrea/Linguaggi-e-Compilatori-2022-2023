# Linguaggi-e-Compilatori-2022-2023
Questo repository contiene i materiali per i laboratori del corso "Linguaggi e Compilatori" dell'Università di Modena e Reggio Emilia



## esercizi eseguiti da Simone Pellacani


## Note su Loop Fusion e test


### compilazione

- per compilare spostarsi nella cartella LoopFusion
- Usare il makefile con `make`, crea l'ottimizzatore 
- `make opty` crea i file ottimizzato
- `make loop` prende il loop in testPopulate e ne estrae il codice intermedio
- `make compileobc` permette di avere 2 eseguibili

### test
- prima esecuzione non ottimizzata
```sh
./mainOutnonOpt
Using populate function, it took: 29.978865 seconds
```
- seconda esecuzione con libreria ottimizzata
```sh
./mainOutOpt
Using populate function, it took: 5.907987 seconds
```
Le esecuzioni sono state ripetute più volte, non si sdicostano da queste rilevazioni puntuali.<br>
Sembra che l'ottimizzatore faccia un lavoro di alleggerimento del carico superiore al 50%. <br>
Possiamo solo ipotizzare cause, per ora restano ignote.
