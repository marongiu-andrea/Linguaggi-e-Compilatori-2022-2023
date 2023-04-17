# Linguaggi-e-Compilatori-2022-2023

Questo repository contiene i materiali per i laboratori del corso "Linguaggi e Compilatori" dell'Università di Modena e Reggio Emilia

<h1>Ottimizzazioni</h1>

<h2>Identità algebrica</h2>

Se si trova un'identità algebrica allora conviene semplificarla in quanto è un'istruzione inutile. <br>
Nel caso di addizione e sottrazione si trova come
$$x = x + 0$$
$$x = x - 0$$
Nel caso di motliplicazione e divisione invece avremo
$$x = x * 1$$
$$x = x / 1$$
Nel caso trovassimo istruzioni del genere le elimineremo.

<h2> Strength Reduction </h2>

La strength Reduction aiuta a semplificare le operazioni di moltiplicazione e divsione, trasformandole in shift con insieme delle sottrazioni o addizioni in caso l'operatore costante non sia una potenza del 2.

$$ 15 × 𝑥 = 𝑥 × 15 ⇒ (𝑥 ≪ 4) – x $$
$$ y = x / 8 ⇒ y = x >> 3 $$

<h2>Multi-Instruction Optimization</h2>
L'ottimizzazione su più istruzioni prende in considerazione istruzioni consecutive che hanno un opcode, ovvero un codice che identifica l'operazione fatta, opposto. Ad esempio se l'opreazione seguente a un'addizione è una sottrazione usante la variabile appena valutata, l'ottimizzazione partirà. Dopo aver calcolato il risultato fra le due costanti delle due operazioni, sostituirà la seconda istruzione con una nuova che userà l'espressione della prima con la somma algebrica dell'espressioni. Qui sotto un esempio che fa capire bene il procedimento. <br> L'ottimizzazione è stata implementata sia con addizione e sottrazione, sia con numeri interi e numeri float in modo da renderla il più generale possibile.

$$ 𝑎 = 𝑏 + 2, 𝑐 = 𝑎 − 1 ⇒𝑎 = 𝑏 + 1, 𝑐 = 𝑏 +1 $$

<h3> Testing </h3>
Ogni ottimizzazione sopra elencata è stata testata con diversi ll creati appositamente per mettere in difficoltà e vedere l'efficienza di esse. Ogni file.ll che viene consegnato ha anche il codice, per dimostrare l'efficacia delle ottimizzazioni, commentato.
