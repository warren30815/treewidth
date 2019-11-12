import matplotlib.pyplot as plt
import matplotlib.cm as cm
import csv
import numpy as np

total_dataset_num = 6
hop = 3
x = []
y_upper = []
y_lower = []
colors = ["red", "green", "blue", "coral", "lightblue", "yellow"]
marker_dict = ['o','x','*','>','<','^','v']
label_dict = ["Enron", "facebook", "soc-Epinions1", "soc-LiveJournal1", "soc-pokec-relationships", "twitter"]

data = np.loadtxt('../result.csv',dtype=np.str , delimiter=',')
for i in range(1, len(data)):
    x.append(int(data[i][1]))
    y_upper.append(int(data[i][2]))
    y_lower.append(int(data[i][4]))

plt.figure(figsize=(8,6))

for i in range(total_dataset_num):
	plt.scatter(x[3 * i : 3 * i + 3], y_upper[3 * i : 3 * i + 3], s=30, c=colors[i], marker=marker_dict[i], alpha=0.5, label=label_dict[i])
	plt.scatter(x[3 * i : 3 * i + 3], y_lower[3 * i : 3 * i + 3], s=30, c=colors[i], marker=marker_dict[i], alpha=0.5, label=label_dict[i])
plt.xlabel('treewidthhop num')
plt.ylabel('treewidth')
plt.yscale('log')
plt.title('Estimation upper:degree(100), lower:MMD')
plt.legend(loc='upper left')
plt.savefig("result.png")
plt.show()