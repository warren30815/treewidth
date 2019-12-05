import time

import networkx as nx
from networkx.algorithms import approximation
from dwave_networkx.algorithms.elimination_ordering import *
import numpy as np

def Read_Graph_from_textFile(inputPath):
    graphs = []
    G = nx.Graph()
    with open(inputPath, 'r') as file:
        line = file.readline()
        while line is not None and line != "":
            if (line[0] != '-'):
                nodes = line.split("\t")
                G.add_edge(int(nodes[0]), int(nodes[1]))
            else:
                graphs.append(G)
                G = nx.Graph()
            line = file.readline()

    if(len(graphs) == 100):
        print("Num Check: OK")
        return graphs
    else:
        print("Num Check: Error")
        exit(-1)

def main():
    hop = 3
    datasets = ["version2_Enron", "version2_facebook_combined", "version2_undirected_twitter_combined", "version2_undirected_soc-Epinions1", "version2_undirected_soc-pokec-relationships", "version2_undirected_soc-LiveJournal1"]
    # datasets = ["version2_undirected_soc-LiveJournal1"]
    # make some graphs
    graphs = []
    for filename in datasets:
        datapath = "python_graph_tools/data/" + filename
        for i in range(hop):
            path = datapath + "_" + str(i + 1) + "Hop.txt";
            graphs = Read_Graph_from_textFile(path)
            tw_sum = 0
            start_time = time.time()
            for graph_idx, eval_graph in enumerate(graphs):
                # exact computation
                tw = minor_min_width(eval_graph)
                # print(tw)
                tw_sum += tw
                print("Process: " + str(path) + ", " + str(graph_idx) + "/100")
                num_node = eval_graph.number_of_nodes()
                num_edge = eval_graph.number_of_edges()
                density = float(2 * num_edge) / float(num_node * (num_node - 1))
                with open("result/lower_tw.csv", 'ab') as file:
                    outputlog = filename + "," + str(i + 1) + "," + str(tw) + "," + str(num_node) + "," + str(num_edge) + "," + str(density) + "\n"
                    file.write(outputlog.encode())
            end_time = time.time()
            log = path + "Average tw: " + "," + str(tw_sum / 100) + "," + "Time: " + "," + str(end_time - start_time) + "\n"
            print(log)

def Output_Graph_textFile(G, outputPath, selected_node, node_degree):
    with open(outputPath, 'ab') as file:
        separtion_log = "----- Node: " + str(selected_node) + " Degree: " + str(node_degree) + "\n"
        file.write(separtion_log.encode())

if __name__ == '__main__':
    main()
