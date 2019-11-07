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

// region 常數區
int SampleNum = 100;
// endregion

// region 變數區
string isDirected;  // set by argv (f or t)
unsigned long total_upper_bound = 0;
unsigned long total_lower_bound = 0;
double total_time = 0;
double total_density = 0;
double min_density = 0;
double max_density = 0;
Graph min_density_graph;
Graph max_density_graph;
// endregion

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
    return (2 * edge_num) / (node_num * (node_num - 1));
}

double compute_directed_density(Graph G){
    unsigned long node_num = G.number_nodes(); 
    unsigned long edge_num = G.number_edges();
    return edge_num / (node_num * (node_num - 1));
}

void add_total_density(Graph G){
    cout << "add total density" << endl;
    if (isDirected == "f") total_density += compute_undirected_density(G);
    else if (isDirected == "t") total_density += compute_directed_density(G);
    else{
        cout << "f or t" << endl;
        exit(-1);
    }
}

void update_min_max_density(Graph G){
    cout << "update_min_max_density" << endl;
    double current_density;
    if (isDirected == "f") current_density = compute_undirected_density(G);
    else if (isDirected == "t") current_density = compute_directed_density(G);
    else{
        cout << "f or t" << endl;
        exit(-1);
    }

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
    int lb = 0;
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

    if(argc < 2)
    {
        cout << "Input format is: " << argv[0] << " [--upper or --lower] [filename] [isDirected, t or f] [(only upper need) strategy_index] [(only upper need) partial_degree]" << endl; 
        return -1;
    }
    else if(string(argv[1]) == "--upper")
    {
        if(argc < 5)
        {
            cout << "Input format is: " << argv[0] << " [--upper or --lower] [filename] [(only upper need) strategy_index] [(only upper need) partial_degree]" << endl; 
            return -1;
        }
    }
    else if(string(argv[1]) == "--lower")
    {
        if(argc < 3)
        {
            cout << "Input format is: " << argv[0] << " [--upper or --lower] [filename]" << endl; 
            return -1;
        }
    }

    string first_arg(argv[1]);  // 此寫法結果和下面那行一樣
    string filename = argv[2];
    isDirected = "f";
    ifstream file(filename);  // ifstream: Stream class to read from files
    vector<Graph> graphs;
    timestamp_t start_time, end_time;
    
    omp_set_num_threads(1000);
    start_time = get_timestamp();

    cout << "Preprocessing..." << endl;
    if (file.is_open()) {
        string line;
        Graph graph;
        while (getline(file, line)) {
            // 以'-'來分隔一組Graph data
            if (line[0] != '-'){
                vector<string> node_pair = string_split(line, "\t");
                if (line != ""){
                    unsigned long src = stoul(node_pair[0]);  // stoul : Convert string to unsigned integer
                    unsigned long target = stoul(node_pair[1]);  
                    if(src != target) graph.add_edge(src, target);
                }
            }
            else{
                if (graph.number_nodes() > 0){
                    graphs.push_back(graph);
                    graph.clear();
                }
            }
        }
        file.close();
    }
    cout << "Preprocessing Done" << endl;

    cout << "Update Sample Num" << endl;
    SampleNum = graphs.size();

    if(first_arg=="--upper"){
    	int strategy_index = atoi(argv[3]);
    	int partial_degree = atoi(argv[4]);
#pragma omp parallel for 
        for(int i = 0; i < SampleNum; i++){
            total_upper_bound += upper(graphs[i], i, strategy_index, partial_degree);
        }
        cout << "Average treewidth 'upper' bound: " << total_upper_bound / SampleNum << endl;
    } 
    else if(first_arg=="--lower"){
#pragma omp parallel for 
        for(int i = 0; i < SampleNum; i++){
            total_lower_bound += lower(graphs[i], i);
        }
        cout << "Average treewidth 'lower' bound: " << total_lower_bound / SampleNum << endl;
    } 

    cout << "Average density: " << total_density / SampleNum << endl;
    cout << "Max density graph: " << to_string(max_density_graph.number_nodes()) << " nodes, " << to_string(max_density_graph.number_edges()) << " edges" << endl;
    cout << "Max density = " << max_density << endl;
    cout << "Min density graph: " << to_string(min_density_graph.number_nodes()) << " nodes, " << to_string(min_density_graph.number_edges()) << " edges" << endl;
    cout << "Min density = " << min_density << endl;

    end_time = get_timestamp();
    cout << "Total elapsed time: " << (end_time - start_time)/(1000.0L*1000.0L) << " sec."<< endl;
    cout << "Filename: " << filename << endl;
    return 0;
}
