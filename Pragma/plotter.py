import numpy as np
import matplotlib.pyplot as plt
with open("for_output_1.txt", "r") as f1:
    with open("for_output_2.txt", "r") as f2:
        raw_data_1 = f1.readlines()
        raw_data_2 = f2.readlines()
        y_1 = [float(d.replace("\n", "")) for d in raw_data_1]
        y_2 = [float(d.replace("\n", "")) for d in raw_data_2]
        x = [int(i) for i in np.linspace(1,10,10)]
        print(y_1)
        print(x)
        plt.plot(x, y_1, label="dynamic,chunk = 1", marker='^')
        plt.plot(x,y_2, label="dynamic, chunk = 4", marker='^')
        plt.legend(loc='lower right')
        plt.show()
