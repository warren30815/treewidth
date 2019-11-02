#ifndef Graph_h
#define Graph_h
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>

//Class for the graph
class Graph{
private:
  std::unordered_map<unsigned long, std::unordered_set<unsigned long>> adj_list;
  std::unordered_set<unsigned long> node_set;
  unsigned long num_edges = 0;

  
public:

  void add_edge(unsigned long src, unsigned long tgt, bool undirected=true){
    if(!has_edge(src, tgt)){
      node_set.insert(src);
      node_set.insert(tgt);
      adj_list[src].insert(tgt);
      if(undirected) adj_list[tgt].insert(src);
      num_edges++;
    }
  };

  void clear(){
    node_set.clear();
    adj_list.clear();
    num_edges = 0;
  }
  
  void add_node(unsigned long node){
    node_set.insert(node);
  }
  
  void remove_node(unsigned long node){
    node_set.erase(node);
    for(auto neighbour:adj_list[node]){
      adj_list[neighbour].erase(node);
      num_edges--;
    }
    adj_list.erase(node);
  }

  bool neighbour_improved(int k,unsigned long n1, unsigned long n2){
    bool retval = false;    
    std::unordered_set<unsigned long> neigh1 = get_neighbours(n1);
    std::unordered_set<unsigned long> neigh2 = get_neighbours(n2);
      
    unsigned long count = 0;
    if  (neigh1.size()>k-1 && neigh2.size()>k-1){
      for (auto nn1:neigh1){
        for (auto nn2:neigh2){
          if (nn1==nn2){
            count = count+1;    
            break;
          }
      
        }
      }   
    }
    if (count > k-1){
      retval = true;
    }

    return retval;
}
 
  void fill(const std::unordered_set<unsigned long>& nodes,\
            bool undirected=true){
    for(auto src: nodes)
      for(auto tgt: nodes)
        if(undirected){
          if((src<tgt)&&(adj_list[src].find(tgt)==adj_list[src].end()))
            add_edge(src, tgt, undirected);
        }
        else{
          if((src!=tgt)&&(adj_list[src].find(tgt)==adj_list[src].end()))
            add_edge(src, tgt, undirected);
        }
    
  }
  
  void contract_edge(unsigned long src, unsigned long tgt){
    for(auto v:get_neighbours(tgt))
      if((v!=src)&&!has_edge(src,v)) add_edge(src,v);
    remove_node(tgt);
  }
  
  bool has_neighbours(unsigned long node) const{
    return adj_list.find(node)!=adj_list.end();
  }
  
  bool has_node(unsigned long node) const{
    return node_set.find(node)!=node_set.end();
  }
  
  bool has_edge(unsigned long src, unsigned long tgt) {
    bool retval = false;
    if(has_neighbours(src)){
      std::unordered_set<unsigned long> neigh = get_neighbours(src);
      retval = neigh.find(tgt)!=neigh.end();
    }
    return retval;
  }
  
  std::unordered_set<unsigned long> get_neighbours(unsigned long node) const{
    if(has_node(node))
      return (adj_list.find(node))->second;
    else
      return std::unordered_set<unsigned long>();
  }
  
  std::unordered_set<unsigned long> get_nodes() const{
    return node_set;
  }
  
  unsigned long number_nodes() const{
    return node_set.size();
  }
  
  unsigned long number_edges() const{
    return num_edges;
  }
};


#endif /* Graph_h */
