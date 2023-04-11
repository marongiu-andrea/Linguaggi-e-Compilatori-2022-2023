| &nbsp;                  | Very Busy Expressions                                                                                    |
| ----------------------- | -------------------------------------------------------------------------------------------------------- |
| Domain                  | set of expressions                                                                                       |
| Direction               | Backward:<br/>$\text{in}[b] = f_b(\text{out}[b])$<br/>$\text{out}[b] = \land \text{ in}[\text{succ}(b)]$ |
| Transfer function       | $f_b(x) = \text{Gen}_b \cup \left(\text{out}[b] - \text{Kill}_b\right)$                                  |
| Meet operator ($\land$) | $\cap$                                                                                                   |
| Boundary condition      | $\text{in}[\text{exit}] = \emptyset$                                                                     |
| Initial interior points | $\text{in}[b] = \mathcal{U}$                                                                             |
