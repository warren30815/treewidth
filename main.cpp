#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <omp.h>

#include "common.h"
#include "Graph.h"

//Strategies and decompositions
#include "PermutationStrategy.h"
#include "DegreePermutationStrategy.h"
#include "FillInPermutationStrategy.h"
#include "DegreeFillInPermutationStrategy.h"
#include "MCSPermutationStrategy.h"
#include "TreeDecomposition.h"

//Lower Bounds
#include "LowerBound.h"
#include "LowerBoundMMD.h"
#include "Delta2D.h"
#include "CE.h"

//Meta lower bounds
#include "MetaLowerBoundHeuristic.h"
#include "LBN.h"
#include "LBNPlus.h"

using namespace std;

// region constant 
int SampleNum = 100;
// endregion

// region 變數區
unsigned long total_upper_bound = 0;
unsigned long total_lower_bound = 0;
double total_time = 0;
double total_density = 0;
unsigned long total_nodes = 0;
unsigned long total_edges = 0;
double min_density = 1;
double max_density = 0;
Graph min_density_graph;
Graph max_density_graph;
// endregion

void reset_all_parameter(){
    total_upper_bound = 0;
    total_lower_bound = 0;
    total_time = 0;
    total_density = 0;
    total_nodes = 0;
    total_edges = 0;
    min_density = 1;
    max_density = 0;
}

// 字串切割函數
vector<string> string_split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;
     
    while(i != s.size()){
        //找到字串中首個不等於分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x){
                if(s[i] == seperator[x]){
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }
         
        //找到又一個分隔符，將兩個分隔符之間的字串取出；
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x){
                if(s[j] == seperator[x]){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0) ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

double compute_undirected_density(Graph G){
    unsigned long node_num = G.number_nodes(); 
    unsigned long edge_num = G.number_edges();
    return (double)(2 * edge_num) / (double)(node_num * (node_num - 1));
}

// double compute_directed_density(Graph G){
//     unsigned long node_num = G.number_nodes(); 
//     unsigned long edge_num = G.number_edges();
//     return (double)edge_num / (double)(node_num * (node_num - 1));
// }

void add_total_node_and_edge_nums(Graph G){
    cout << "add_total_node_and_edge_nums" << endl;
    total_nodes += G.number_nodes();
    total_edges += G.number_edges();
}

void add_total_density(Graph G){
    cout << "add total density" << endl;
    total_density += compute_undirected_density(G);
}

void update_min_max_density(Graph G){
    cout << "update_min_max_density" << endl;
    double current_density;
    current_density = compute_undirected_density(G);

    if (current_density > max_density){
        max_density = current_density;
        max_density_graph = G;
    }
    else if (current_density < min_density){
        min_density = current_density;
        min_density_graph = G;
    }
}

unsigned long lower(Graph graph, int graph_index){
    // string file_name_graph(argv[2]);
    // stringstream ss, ss_stat;
    timestamp_t t0, t1;
    double time_sec;
    int meta = 0;
    int lb = 1;
    // if(argc>3)
    //     lb = atoi(argv[3]);
    // if(argc>4)
    //     meta = atoi(argv[4]);

    // ss << file_name_graph << "." << meta << "." << lb << "." << ".lb";
    //strategy
    unique_ptr<PermutationStrategy> strategy(new DegreePermutationStrategy());
    //lower bounds estimators
    vector<unique_ptr<LowerBound>> bounds;
    bounds.push_back(unique_ptr<LowerBound>\
           (new LowerBoundMMD(graph, *strategy)));
    bounds.push_back(unique_ptr<LowerBound>\
           (new LowerBoundMMDPlus(graph, *strategy)));
    bounds.push_back(unique_ptr<LowerBound>\
           (new Delta2D(graph, *strategy)));
    //meta lower bound estimators
    vector<unique_ptr<MetaLowerBoundHeuristic>> metas;
    metas.push_back(unique_ptr<MetaLowerBoundHeuristic>\
         (new LBN(graph, *bounds[lb])));
    metas.push_back(unique_ptr<MetaLowerBoundHeuristic>\
          (new LBNPlus(graph, *bounds[lb])));

    cout << "loading graph " << graph_index << "..." << flush;
    cout << "graph: " << to_string(graph.number_nodes()) << " nodes, " << to_string(graph.number_edges()) << " edges" << endl;
    add_total_node_and_edge_nums(graph);
    add_total_density(graph);
    update_min_max_density(graph);
    // ofstream filestat(ss.str());
    cout << "lower bound..."  << flush;
    t0 = get_timestamp();
    unsigned long lower = 0;
    if(meta==0){
        lower = bounds[lb]->estimate();
    }
    else{
        lower = metas[meta-1]->estimate();
    }
    t1 = get_timestamp();
    time_sec = (t1-t0)/(1000.0L*1000.0L);
    cout << " done in " << time_sec << " sec."<< endl;
    cout << "lower bound " << lower << endl;
    // filestat << lower << "\t" << time_sec << endl;
    // filestat.close();
    return lower;
}

unsigned long upper(Graph graph, int graph_index, int strategy_index, int partial_degree){
    // stringstream ss, ss_stat;
    timestamp_t t0, t1;
    double time_sec;
    // ss << file_name_graph << "." << strategy_index << "." << partial_degree << ".dec";
    // ss_stat << file_name_graph << "." << strategy_index << "." << partial_degree << ".stat";
    vector<unique_ptr<PermutationStrategy>> strategies;
    strategies.push_back(unique_ptr<PermutationStrategy>
             (new DegreePermutationStrategy()));
    strategies.push_back(unique_ptr<PermutationStrategy>
             (new FillInPermutationStrategy()));
    strategies.push_back(unique_ptr<PermutationStrategy>
             (new DegreeFillInPermutationStrategy()));
    strategies.push_back(unique_ptr<PermutationStrategy>
             (new MCSPermutationStrategy()));
    // unsigned long src, tgt;
    // ofstream filestat(ss_stat.str());
    cout << "loading graph " << graph_index << "..." << flush;
    cout << "graph: " << to_string(graph.number_nodes()) << " nodes, " << to_string(graph.number_edges()) << " edges" << endl;
    add_total_node_and_edge_nums(graph);
    add_total_density(graph);
    update_min_max_density(graph);
    TreeDecomposition decomposition(graph, *strategies[strategy_index]);
    cout << "upper bound..."  << flush;
    t0 = get_timestamp();
    unsigned long upper = decomposition.decompose(partial_degree);
    t1 = get_timestamp();
    time_sec = (t1-t0)/(1000.0L*1000.0L);
    cout << " done in " << time_sec << " sec."<< endl;
    cout << "upper bound " << upper << endl;
    // filestat << upper << "\t";
    // filestat << time_sec << "\t";
    cout << "building tree..."  << flush;
    t0 = get_timestamp();
    decomposition.build_tree();
    t1 = get_timestamp();
    time_sec = (t1-t0)/(1000.0L*1000.0L);
    cout << " done in " << time_sec << " sec."<< endl;
    // filestat << time_sec << endl;
    // cout << "writing decomposition..." << flush;
    // t0 = get_timestamp();
    // ofstream filedec(ss.str());
    // filedec << decomposition;
    // filedec.close();
    // filestat << decomposition.get_stat();
    // filestat.close();
    // t1 = get_timestamp();
    // time_sec = (t1-t0)/(1000.0L*1000.0L);
    // cout << " done in " << time_sec << " sec."<< endl;
    return upper;
}

int main(int argc, const char * argv[]) {

    if(argc != 3)
    {
        cout << "Input format is: " << argv[0] << " [(only upper need) strategy_index] [(only upper need) partial_degree]" << endl; 
        return -1;
    }

    fstream oFile;
    oFile.open("result/upper_tw.csv", ios::out | ios::app);
    // oFile << "Data" << "," << "Hop" << "," << "upper treewidth" << "," << "node nums" << "," << "edge nums" << "," << "density" << endl;

    vector<string> vect{"version2_Enron", "version2_facebook_combined", "version2_undirected_soc-Epinions1", "version2_undirected_twitter_combined", "version2_undirected_soc-pokec-relationships", "version2_undirected_soc-LiveJournal1"};
    // vector<string> vect{"version2_undirected_twitter_combined", "version2_undirected_soc-pokec-relationships"};
    // vector<string> vect{"version2_undirected_soc-pokec-relationships", "version2_undirected_soc-LiveJournal1"};

    for (string filename : vect){
        string datapath = "python_graph_tools/data/" + filename;
        for (int hop_index = 1; hop_index < 4; hop_index++){
            string path = datapath + "_" + to_string(hop_index) + "Hop.txt";
            ifstream file(path);  // ifstream: Stream class to read from files
            vector<Graph> graphs;
            timestamp_t start_time, end_time;
            
            // omp_set_num_threads(1000);

            cout << "Filename: " << filename << "_Hop" << to_string(hop_index) << endl;

            cout << "Preprocessing..." << endl;
            if (file.is_open()) {
                string line;
                Graph graph;
                while (getline(file, line)) {
                    // 以'-'來分隔一組Graph data
                    if (line[0] != '-'){
                        vector<string> node_pair = string_split(line, "\t");
                        unsigned long src = stoul(node_pair[0]);  // stoul : Convert string to unsigned long
                        unsigned long target = stoul(node_pair[1]);  // stoull : Convert string to unsigned long long
                        if(src != target) graph.add_edge(src, target);
                    }
                    else{
                        if (graph.number_nodes() > 0)
                        {
                            graphs.push_back(graph);
                            graph.clear();
                        }
                    }
                }
                file.close(); 
            }

            cout << "Preprocessing Done" << endl;

            cout << "Check Sample Num..." << endl;
            if (SampleNum != (int)graphs.size())
            {
                cout << "Failed: " << (int)graphs.size() << "/100" << endl;
                exit(-1);
            }
            else
            {
                cout << "OK" << endl;
            }

            // region upper
            // start_time = get_timestamp();

            int strategy_index = atoi(argv[1]);
            int partial_degree = atoi(argv[2]);
            // #pragma omp parallel for 
            for(int i = 0; i < SampleNum; i++){
                // total_upper_bound += upper(graphs[i], i, strategy_index, partial_degree);
                Graph G = graphs[i];
                int tw = upper(G, i, strategy_index, partial_degree);
                unsigned long node_num = G.number_nodes(); 
                unsigned long edge_num = G.number_edges();
                double density = compute_undirected_density(G);
                oFile << filename << "," << to_string(hop_index) << "," << tw << "," << node_num << "," << edge_num << "," << density << endl;
            }
            // cout << "Average treewidth 'upper' bound: " << total_upper_bound / SampleNum << endl;
            // end_time = get_timestamp();
            // cout << "Total elapsed time (not including preprocessing time): " << (end_time - start_time)/(1000.0L*1000.0L) << " sec."<< endl;
            // oFile << total_upper_bound / SampleNum << "," << (end_time - start_time)/(1000.0L*1000.0L) << ",";

            // reset_all_parameter();
            // // endregion

            // // region lower
            // start_time = get_timestamp();

            // #pragma omp parallel for 
            // for(int i = 0; i < SampleNum; i++){
            //     total_lower_bound += lower(graphs[i], i);
            // }
            // cout << "Average treewidth 'lower' bound: " << total_lower_bound / SampleNum << endl;
            // end_time = get_timestamp();
            // cout << "Total elapsed time (not including preprocessing time): " << (end_time - start_time)/(1000.0L*1000.0L) << " sec."<< endl;
            // oFile << total_lower_bound / SampleNum << "," << (end_time - start_time)/(1000.0L*1000.0L) << ",";

            // // endregion

            // cout << "Average node numbers: " << total_nodes / SampleNum << endl;
            // cout << "Average edge numbers: " << total_edges / SampleNum << endl;
            // cout << "Average density: " << total_density / SampleNum << endl;
            // cout << "Max density graph: " << to_string(max_density_graph.number_nodes()) << " nodes, " << to_string(max_density_graph.number_edges()) << " edges" << endl;
            // cout << "Max density = " << max_density << endl;
            // cout << "Min density graph: " << to_string(min_density_graph.number_nodes()) << " nodes, " << to_string(min_density_graph.number_edges()) << " edges" << endl;
            // cout << "Min density = " << min_density << endl;

            // oFile << total_nodes / SampleNum << "," << total_edges / SampleNum << "," << total_density / SampleNum << "," <<\
            // max_density_graph.number_nodes() << "," << max_density_graph.number_edges() << "," <<\
            // max_density << "," << min_density_graph.number_nodes() << "," << min_density_graph.number_edges() << "," <<\
            // min_density << endl;

            graphs.clear();
        }
    }
    return 0;
}
