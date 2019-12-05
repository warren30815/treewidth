import matplotlib.pyplot as plt
import matplotlib.cm as cm
import csv
import numpy as np

total_hop = 3
total_dataset_num = 5
sample_num = 100
upper_tw_list = []
upper_density_list = []
lower_tw_list = []
lower_density_list = []
colors = ["red", "green", "blue"]
marker_dict = ['o','x','*']
label_dict = ["1Hop", "2Hop", "3Hop"]

plt.figure(figsize=(10,5))

data = np.loadtxt('../result/upper_tw.csv',dtype=np.str , delimiter=',')

tmp_hop1_tw = []
tmp_hop2_tw = []
tmp_hop3_tw = []
tmp_hop1_density = []
tmp_hop2_density = []
tmp_hop3_density = []

for i in range(len(data)):
    hop_num = int(data[i][1])
    upper_tw = int(data[i][2])
    density = float(data[i][5])
    if density <= 1:
	    if hop_num == 1:
	    	tmp_hop1_tw.append(upper_tw)
	    	tmp_hop1_density.append(density)
	    elif hop_num == 2:
	    	tmp_hop2_tw.append(upper_tw)
	    	tmp_hop2_density.append(density)
	    elif hop_num == 3:
	    	tmp_hop3_tw.append(upper_tw)
	    	tmp_hop3_density.append(density)
    
upper_tw_list.append(tmp_hop1_tw)
upper_tw_list.append(tmp_hop2_tw)
upper_tw_list.append(tmp_hop3_tw)
upper_density_list.append(tmp_hop1_density)
upper_density_list.append(tmp_hop2_density)
upper_density_list.append(tmp_hop3_density)

for hop in range(total_hop):
	plt.scatter(upper_density_list[hop][:], upper_tw_list[hop][:], s=30, c=colors[hop], marker=marker_dict[hop], alpha=0.5, label=label_dict[hop])

plt.xlabel('density')
# plt.xticks(np.arange(1,3.1,1))
plt.ylabel('treewidth')
plt.yscale('log')
plt.title('density_to_upper_width')
plt.legend(loc='upper right')
plt.savefig("../result/density_to_upper_width.png")
plt.show()

plt.figure(figsize=(10,5))

data = np.loadtxt('../result/lower_tw.csv',dtype=np.str , delimiter=',')

tmp_hop1_tw = []
tmp_hop2_tw = []
tmp_hop3_tw = []
tmp_hop1_density = []
tmp_hop2_density = []
tmp_hop3_density = []

for i in range(len(data)):
    hop_num = int(data[i][1])
    lower_tw = int(data[i][2])
    density = float(data[i][5])
    if density <= 1:
	    if hop_num == 1:
	    	tmp_hop1_tw.append(lower_tw)
	    	tmp_hop1_density.append(density)
	    elif hop_num == 2:
	    	tmp_hop2_tw.append(lower_tw)
	    	tmp_hop2_density.append(density)
	    elif hop_num == 3:
	    	tmp_hop3_tw.append(lower_tw)
	    	tmp_hop3_density.append(density)
    
lower_tw_list.append(tmp_hop1_tw)
lower_tw_list.append(tmp_hop2_tw)
lower_tw_list.append(tmp_hop3_tw)
lower_density_list.append(tmp_hop1_density)
lower_density_list.append(tmp_hop2_density)
lower_density_list.append(tmp_hop3_density)

for hop in range(total_hop):
	plt.scatter(lower_density_list[hop][:], lower_tw_list[hop][:], s=30, c=colors[hop], marker=marker_dict[hop], alpha=0.5, label=label_dict[hop])

plt.xlabel('density')
# plt.xticks(np.arange(1,3.1,1))
plt.ylabel('treewidth')
plt.yscale('log')
plt.title('density_to_lower_width')
plt.legend(loc='upper right')
plt.savefig("../result/density_to_lower_width.png")
plt.show()