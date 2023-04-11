| &nbsp;                  | Dominator Analysis                                                                                      |
| ----------------------- | ------------------------------------------------------------------------------------------------------- |
| Domain                  | set of basic blocks                                                                                     |
| Direction               | Forward:<br/>$\text{in}[b] = \land \text{ out}[\text{pred}(b)]$<br/>$\text{out}[b] = f_b(\text{in}[b])$ |
| Transfer function       | $f_b(x) = \left\{b\right\} \cup \text{in}[b]$                                                           |
| Meet operator           | $\cap$                                                                                                  |
| Boundary condition      | $\text{out}[\text{entry}] = \emptyset$                                                                  |
| Initial interior points | $\text{out}[b] = $\mathcal{U}$                                                                          |