| &nbsp;                  | Dominator Analysis                                                                                      |
| ----------------------- | ------------------------------------------------------------------------------------------------------- |
| Domain                  | set of basic blocks                                                                                     |
| Direction               | Forward:<br/>$\text{in}[b] = \land \text{ out}[\text{pred}(b)]$<br/>$\text{out}[b] = f_b(\text{in}[b])$ |
| Transfer function       | $f_b(x) = \left\lbrace b\right\rbrace \cup \text{in}[b]$                                                           |
| Meet operator           | $\cap$                                                                                                  |
| Boundary condition      | $\text{out}[\text{entry}] = \text{entry}$                                                              |
| Initial interior points | $\text{out}[b] = \mathcal{U}$                                                                          |

# Esercizio

![](assets/dominator_analysis.jpg)

# Iterazione 1

| &nbsp; | Gen<sub>b</sub> | Kill<sub>b</sub> | in[b] | out[b] |
| ------ | --------------- | ---------------- | ----- | ------ |
| A | $\left\lbrace A\right\rbrace$ | $\emptyset$ | $\emptyset$ | $\left\lbrace A\right\rbrace$ |
| B | $\left\lbrace B\right\rbrace$ | $\emptyset$ | $\text{out}[\text{A}]$ | $\left\lbrace A, B\right\rbrace$ |
| C | $\left\lbrace C\right\rbrace$ | $\emptyset$ | $\text{out}[\text{B}]$ | $\left\lbrace A, C\right\rbrace$ |
| D | $\left\lbrace D\right\rbrace$ | $\emptyset$ | $\text{out}[\text{C}]$ | $\left\lbrace A, C, D\right\rbrace$ |
| E | $\left\lbrace E\right\rbrace$ | $\emptyset$ | $\text{out}[\text{E}]$ | $\left\lbrace A, C, E\right\rbrace$ |
| F | $\left\lbrace F\right\rbrace$ | $\emptyset$ | $\text{out}[\text{F}]$ | $\left\lbrace A, C, F\right\rbrace$ |
| G | $\left\lbrace G\right\rbrace$ | $\emptyset$ | $\text{out}[\text{G}]$ | $\left\lbrace A, G\right\rbrace$ |

Gli out sono cambiati, occorre una nuova iterazione.

# Iterazione 2

| &nbsp; | Gen<sub>b</sub> | Kill<sub>b</sub> | in[b] | out[b] |
| ------ | --------------- | ---------------- | ----- | ------ |
| A | $\left\lbrace A\right\rbrace$ | $\emptyset$ | $\emptyset$ | $\left\lbrace A\right\rbrace$ |
| B | $\left\lbrace B\right\rbrace$ | $\emptyset$ | $\text{out}[\text{A}]$ | $\left\lbrace A, B\right\rbrace$ |
| C | $\left\lbrace C\right\rbrace$ | $\emptyset$ | $\text{out}[\text{B}]$ | $\left\lbrace A, C\right\rbrace$ |
| D | $\left\lbrace D\right\rbrace$ | $\emptyset$ | $\text{out}[\text{C}]$ | $\left\lbrace A, C, D\right\rbrace$ |
| E | $\left\lbrace E\right\rbrace$ | $\emptyset$ | $\text{out}[\text{E}]$ | $\left\lbrace A, C, E\right\rbrace$ |
| F | $\left\lbrace F\right\rbrace$ | $\emptyset$ | $\text{out}[\text{F}]$ | $\left\lbrace A, C, F\right\rbrace$ |
| G | $\left\lbrace G\right\rbrace$ | $\emptyset$ | $\text{out}[\text{G}]$ | $\left\lbrace A, G\right\rbrace$ |

Gli out non sono camibati: l'algoritmo termina.
