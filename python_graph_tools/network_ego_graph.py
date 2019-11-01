from operator import itemgetter
import matplotlib.pyplot as plt
import networkx as netx
from random import *
import os

# region 變數區
FileName = "Enron.txt"
Graphtype = netx.Graph()   # use net.DiGraph() for directed graph
SampleNum = 100
Hop = 1
OutputFileDir = str(SampleNum) + "_Sample_Data" 
OutputFileName = str(Hop) + "Hop.txt"
OutputFilePath = OutputFileDir + "/" + OutputFileName
Mode = "Dev"  # Dev or Test 
# endregion

def Test_ego_graph_function(hop=1):
    # Create a BA model graph
    n = 10
    m = 2
    TestG = netx.generators.barabasi_albert_graph(n, m)
    # find node with largest degree
    node_and_degree = TestG.degree()
    (largest_hub, degree) = sorted(node_and_degree, key=itemgetter(1))[-1]
    # Create ego graph of main hub
    custom_ego = netx.ego_graph(TestG, largest_hub, hop)
    Draw_Graph(TestG, largest_hub)
    Draw_Graph(custom_ego, largest_hub)
    exit()

def Build_Custom_hop_Ego_Network(G, hop, selected_node):
    custom_ego = netx.ego_graph(G, selected_node, hop)
    return custom_ego
   
def Calculate_largest_degree_node(G):
    node_and_degree = G.degree()
    (largest_node, degree) = sorted(node_and_degree, key=itemgetter(1))[-1]
    return (largest_node, degree)

def Draw_Graph(G, highlight_nodes):
    pos = netx.spring_layout(G)
    netx.draw(G, pos, node_color='b', node_size=50, with_labels=False)
    netx.draw_networkx_nodes(G, pos, nodelist=[highlight_nodes], node_size=300, node_color='r')
    plt.show()

def Output_Graph_textFile(G, outputPath, selected_node, node_degree):
    with open(outputPath, 'ab') as file:
        separtion_log = "----- Node: " + str(selected_node) + " Degree: " + str(node_degree) + "\n"
        file.write(separtion_log.encode())
        netx.write_edgelist(G, file, data=False, delimiter='\t')

if __name__ == '__main__':

    if Mode == "Dev":
        # How to read from a file. Note: if your egde weights are int, change float to int.
        G = netx.read_edgelist(FileName, create_using=Graphtype, nodetype=int)

        if not os.path.isdir(OutputFileDir):
            os.mkdir(OutputFileDir)
        print("Output Dir is: " + OutputFileDir)
        # Find the total number of degree for each node
        # for x in G.nodes():
        #     print("Node: ", x, " has total #degree: ",G.degree(x))

        for i in range(SampleNum):
            selected_node = sample(G.nodes(), 1)[0]
            print("Node: ", selected_node, " has total #degree: ",G.degree(selected_node))
            custom_ego = Build_Custom_hop_Ego_Network(G, hop=Hop, selected_node=selected_node)
            Output_Graph_textFile(custom_ego, OutputFilePath, selected_node, G.degree(selected_node))
        # # full graph
        # Show_Full_Graph(G)
        # Draw_Graph(custom_ego, selected_node)
    elif Mode == "Test":
        Test_ego_graph_function()