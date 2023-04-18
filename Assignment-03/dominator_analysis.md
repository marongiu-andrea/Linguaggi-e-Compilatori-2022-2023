| &nbsp;                  | Dominator Analysis                                                                                      |
| ----------------------- | ------------------------------------------------------------------------------------------------------- |
| Domain                  | set of basic blocks                                                                                     |
| Direction               | Forward:<br/>$\text{in}[b] = \land \text{ out}[\text{pred}(b)]$<br/>$\text{out}[b] = f_b(\text{in}[b])$ |
| Transfer function       | $f_b(x) = \left\{b\right\} \cup \text{in}[b]$                                                           |
| Meet operator           | $\cap$                                                                                                  |
| Boundary condition      | $\text{out}[\text{entry}] = \text{entry}                                                                |
| Initial interior points | $\text{out}[b] = $\mathcal{U}$                                                                          |

# Esercizio

![](assets/dominator_analysis.jpg)

# Iterazione 1

| &nbsp; | Gen<sub>b</sub> | Kill<sub>b</sub> | in[b] | out[b] |
| ------ | --------------- | ---------------- | ----- | ------ |
| A | $\left\{A\rigth} | $\emptyset$ | $\emptyset$ | $\left\{A\rigth} |
| B | $\left\{B\rigth} | $\emptyset$ | $\text{out}[\text{A}]$ | $\left\{A, B\rigth} |
| C | $\left\{C\rigth} | $\emptyset$ | $\text{out}[\text{B}]$ | $\left\{A, C\rigth} |
| D | $\left\{D\rigth} | $\emptyset$ | $\text{out}[\text{C}]$ | $\left\{A, C, D\rigth} |
| E | $\left\{E\rigth} | $\emptyset$ | $\text{out}[\text{E}]$ | $\left\{A, C, E\rigth} |
| F | $\left\{F\rigth} | $\emptyset$ | $\text{out}[\text{F}]$ | $\left\{A, C, F\rigth} |
| G | $\left\{G\rigth} | $\emptyset$ | $\text{out}[\text{G}]$ | $\left\{A, G\rigth} |

Gli out sono cambiati, occorre una nuova iterazione.

# Iterazione 2

| &nbsp; | Gen<sub>b</sub> | Kill<sub>b</sub> | in[b] | out[b] |
| ------ | --------------- | ---------------- | ----- | ------ |
| A | $\left\{A\rigth} | $\emptyset$ | $\emptyset$ | $\left\{A\rigth} |
| B | $\left\{B\rigth} | $\emptyset$ | $\text{out}[\text{A}]$ | $\left\{A, B\rigth} |
| C | $\left\{C\rigth} | $\emptyset$ | $\text{out}[\text{B}]$ | $\left\{A, C\rigth} |
| D | $\left\{D\rigth} | $\emptyset$ | $\text{out}[\text{C}]$ | $\left\{A, C, D\rigth} |
| E | $\left\{E\rigth} | $\emptyset$ | $\text{out}[\text{E}]$ | $\left\{A, C, E\rigth} |
| F | $\left\{F\rigth} | $\emptyset$ | $\text{out}[\text{F}]$ | $\left\{A, C, F\rigth} |
| G | $\left\{G\rigth} | $\emptyset$ | $\text{out}[\text{G}]$ | $\left\{A, G\rigth} |

Gli out non sono camibati: l'algoritmo termina.
