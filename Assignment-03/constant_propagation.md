| &nbsp;                  | Constant Propagation                                                                                    |
| ----------------------- | ------------------------------------------------------------------------------------------------------- |
| Domain                  | set of pairs $\left(\text{variable}, \text{ constant value}\right)$                                     |
| Direction               | Forward:<br/>$\text{in}[b] = \land \text{ out}[\text{pred}(b)]$<br/>$\text{out}[b] = f_b(\text{in}[b])$ |
| Transfer function       | $f_b = \text{Gen}_b \cup \left(x - \text{Kill}_b\right)$                                                |
| Meet operator ($\land$) | $\cap$                                                                                                  |
| Boundary condition      | $\text{out}[\text{entry}] = \emptyset$                                                                  |
| Initial interior points | $\text{out}[b] = \mathcal{U}$                                                                           |