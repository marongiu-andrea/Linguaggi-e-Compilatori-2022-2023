import numpy as np
import matplotlib.pyplot as plt
with open("for_output_1.txt", "r") as f1:
    with open("for_output_2.txt", "r") as f2:
        with open("for_output_3.txt", "r") as f3:        
            raw_data_1 = f1.readlines()
            raw_data_2 = f2.readlines()
            raw_data_3 = f3.readlines()
            y_1 = [float(d.replace("\n", "")) for d in raw_data_1]
            y_2 = [float(d.replace("\n", "")) for d in raw_data_2]
            y_3 = [float(d.replace("\n", "")) for d in raw_data_3]
            x = [int(i) for i in np.linspace(1,15,15)]
            plt.plot(x,y_1, label="static", marker='^', color="blue")
            plt.plot(x,y_2, label="dynamic, chunk = 1", marker='^', color="orange")
            plt.plot(x,y_3, label="dynamic, chunk = 4", marker='^', color="red")
            plt.xlabel("Function calls")
            plt.ylabel("Time(ms)")
            plt.legend(loc='lower right')
            plt.show()
