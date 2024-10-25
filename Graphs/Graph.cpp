#include "Graph.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <queue>
#include <tuple>
#include <limits>
using namespace std;

Graph::Graph(const char* const & edgelist_csv_fn) {
    numEdges=0;
    ifstream edgesFile(edgelist_csv_fn);
    string line;
    string::size_type sz;
    while (getline(edgesFile,line)){
        numEdges++;
        istringstream ss(line);
        string first,second,third;
        getline(ss,first,',');
        getline(ss,second,',');
        getline(ss,third,'\n');
        double thirdDoub = stod(third,&sz);
        if (nodesEdges.find(first) == nodesEdges.end()){
            pair<string,double> newIn(second,thirdDoub);
            unordered_map<string,double> mapIn;
            mapIn.insert(newIn);
            pair<string,unordered_map<string,double>> newOut(first,mapIn);
            nodesEdges.insert(newOut);
        } else{
            pair<string,double> newIn(second,thirdDoub);
            ((nodesEdges.find(first))->second).insert(newIn);
        }
        if (nodesEdges.find(second) == nodesEdges.end()){
            pair<string,double> newIn(first,thirdDoub);
            unordered_map<string,double> mapIn;
            mapIn.insert(newIn);
            pair<string,unordered_map<string,double>> newOut(second,mapIn);
            nodesEdges.insert(newOut);
        } else{
            pair<string,double> newIn(first,thirdDoub);
            ((nodesEdges.find(second))->second).insert(newIn);
        }
    }
    // for (auto elem : nodesEdges){
    //     cout << elem.first << "-";
    //     for (auto elem2 : elem.second){
    //         cout << elem2.first;
    //     }
    //     cout << endl;
    // }
}

unsigned int Graph::num_nodes() {
    return nodesEdges.size();
}

vector<string> Graph::nodes() {
    vector<string> toReturn;
    for (auto elem : nodesEdges){
        toReturn.insert(toReturn.end(),elem.first);
    }
    return toReturn;
}

unsigned int Graph::num_edges() {
    return numEdges;
}

unsigned int Graph::num_neighbors(string const & node_label) {
    unordered_map<string,unordered_map<string,double>>::iterator it = nodesEdges.find(node_label);
    if (it == nodesEdges.end()){
        return 0;
    }
    return (it->second.size());
}

double Graph::edge_weight(string const & u_label, string const & v_label) {
    unordered_map<string,unordered_map<string,double>>::iterator it = nodesEdges.find(u_label);
    if (it == nodesEdges.end()){
        return -1;
    }
    unordered_map<string,double>::iterator it2 = it->second.find(v_label);
    if (it2 == it->second.end()){
        return -1;
    }
    return (it2->second);
}

vector<string> Graph::neighbors(string const & node_label) {
    vector<string> toReturn;
    for (auto elem : nodesEdges.find(node_label)->second){
        toReturn.insert(toReturn.end(), elem.first);
    }
    return toReturn;
}

vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    int i = 0;
    queue<string> mainQ;
    queue<int> mainQInts;
    unordered_map<string,int> visited;
    vector<string> path;
    // pair<int,string> newPair(i,start_label);
    mainQ.push(start_label);
    mainQInts.push(i);
    while (mainQ.size() != 0){
        string p = mainQ.front();
        int pInt = mainQInts.front();
        mainQ.pop();
        mainQInts.pop();
        if (visited.find(p) == visited.end()){
            pair<string,int> npair(p,pInt);
            visited.insert(npair);
            if (p == end_label){
                path.insert(path.begin(), p);
                string trav = p;
                // for (auto elem : visited){
                //             cout << "elems: " << elem;
                //         }
                while(trav != start_label){
                    // cout << p;
                    for (string neigh : this->neighbors(p)){
                        if ((visited.find(neigh) != visited.end())){
                            if (visited.find(neigh)->second < visited.find(p)->second){
                            visited.erase(p);
                            p = neigh;
                            // cout << neigh;
                            path.insert(path.begin(), p);
                            trav = p;
                            break;
                            }
                        }
                    }
                }
                return path;
            }
            i++;
            for (string edge : this->neighbors(p)){
                if (visited.find(edge) == visited.end()){
                    mainQ.push(edge);
                    mainQInts.push(i);
                }
            }
        }
    }
    return path;
}
class myComp{
public:
    bool operator()(tuple<double,string> a, tuple<double,string> b){
        return (get<0>(a) > get<0>(b));
    }
};

class myComp2{
public:
    bool operator()(tuple<double,string,string> a, tuple<double,string,string> b){
        return (get<0>(a) > get<0>(b));
    }
};

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    vector<tuple<string,string,double>> toRet2;
    if (start_label == end_label){
            tuple<string,string,double> tup(start_label,start_label,-1);
            toRet2.clear();
            toRet2.insert(toRet2.end(),tup);
            return toRet2;
    }
    unordered_map<string,double> nodeDist;
    for (auto elem : nodesEdges){
        pair<string,double> toIns(elem.first,numeric_limits<double>::max());
        nodeDist.insert(toIns);
    }

    unordered_map<string,double>::iterator it = nodeDist.find(start_label);
    if (it != nodeDist.end()){
        nodeDist.erase(start_label);
        pair<string,double> toIns(start_label,0.0);
        nodeDist.insert(toIns);
    }

    unordered_map<string,double> visited;
    unordered_map<string,string> prevs;
    priority_queue<tuple<double,string>,vector<tuple<double,string>>,myComp> mainPQ;
    
    tuple<double,string> t1(0,start_label);
    mainPQ.push(t1);
    while (mainPQ.size() != 0){
        double popedInt = get<0>(mainPQ.top());
        string poped = get<1>(mainPQ.top());
        mainPQ.pop();
        if (visited.find(poped) == visited.end()){
            pair<string,double> in1(poped,popedInt);
            visited.insert(in1);
            for (auto edge : this->neighbors(poped)){
                if ((nodeDist.find(poped)->second)+this->edge_weight(poped,edge) < (nodeDist.find(edge)->second)){
                    nodeDist.erase(edge);
                    pair<string,double> p2(edge,(nodeDist.find(poped)->second)+this->edge_weight(poped,edge));
                    nodeDist.insert(p2);
                    tuple<double,string> t2((nodeDist.find(poped)->second)+this->edge_weight(poped,edge),edge);

                    pair<string,string> p3(edge,poped);
                    prevs.erase(edge);
                    prevs.insert(p3);
                    mainPQ.push(t2);
                }
            }
        }
    }
    // cout << endl << "prevs: ";
    // for (auto elem : prevs){
    //     cout << elem.first << ", " << elem.second << endl;
    // }
    if (prevs.find(end_label) == prevs.end()){
        return toRet2;
    }
    string poped = prevs.find(end_label)->first;
    if (poped == end_label){
        vector<string> toRet;
        string trav = poped;
        while(trav != start_label){
            toRet.insert(toRet.begin(),trav);
            trav = prevs.find(trav)->second;
        }
        toRet.insert(toRet.begin(),start_label);
        if (toRet.front() != start_label || toRet.back() != end_label){
            toRet.clear();
        }
        for (long unsigned int i=0; i<toRet.size()-1;i++){
            tuple<string,string,double> tup(toRet.at(i),toRet.at(i+1),this->edge_weight(toRet.at(i),toRet.at(i+1)));
            toRet2.insert(toRet2.end(),tup);
        }
        return toRet2;
    }
    return toRet2;
}


vector<string> nodesToSearch;
void Graph::recHelperCC(string start, vector<string> & comp, double const & threshold){
    //if not in comp, add it and remove it from nodes to nodesToSearch
    comp.insert(comp.end(),start);
    vector<string>::iterator it = nodesToSearch.begin();
    while (it != nodesToSearch.end()){
        if (*it == start){
            nodesToSearch.erase(it);
            break;
        }
        it++;
    }
    // recurse over all neighbors of start
    for(auto neigh : this->neighbors(start)){
        bool found2 = false;
        if (this->edge_weight(start,neigh) > threshold){
                continue;
        }
        // if already in comp, dont recurse over it
        for (auto strings : comp){
            if (neigh == strings){
                found2 = true;
                break;
            }
        }
        if (!found2){
            recHelperCC(neigh, comp, threshold);
        }
    }
}

vector<vector<string>> Graph::connected_components(double const & threshold) {
    vector<string> toReturnCC;
    vector<vector<string>> toReturn;
    nodesToSearch = this->nodes();

    while (nodesToSearch.size() != 0){
        string currNode = nodesToSearch.at(0);
        toReturnCC.clear();
        recHelperCC(currNode, toReturnCC, threshold);
        toReturn.insert(toReturn.end(),toReturnCC);
    }
    return toReturn;
}
string findDS(string a, unordered_map<string,string> & DS){
    string curr = a;
    while (DS.find(curr)->second != ""){
        curr = DS.find(curr)->second;
    }
    return curr;
}

void unionDS(string a, string b, unordered_map<string,string> & DS){
    string setA = findDS(a,DS);
    string setB = findDS(b,DS);
    if (setA == setB){
        return;
    }
    DS.find(setA)->second = setB;
}
double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label) {
    if (start_label == end_label){
        return 0.0;
    }
    priority_queue<tuple<double,string,string>,vector<tuple<double,string,string>>,myComp2> mainPQ;
    for (auto e : nodesEdges){
        for (auto ee : e.second){
            tuple<double,string,string> tup(ee.second,e.first,ee.first);
            mainPQ.push(tup);
        }
    }
    vector<string> allNodes = this->nodes();
    unordered_map<string,string> disjointSet;
    for (string s : allNodes){
        pair<string,string> pairToAdd(s,"");
        disjointSet.insert(pairToAdd);
    }
    while (mainPQ.size() != 0){
        // cout << get<0>(mainPQ.top()) << get<1>(mainPQ.top()) << get<2>(mainPQ.top()) << endl;
        unionDS(get<1>(mainPQ.top()),get<2>(mainPQ.top()),disjointSet);
        if (findDS(start_label,disjointSet) == findDS(end_label,disjointSet)){
            return get<0>(mainPQ.top());
        }
        mainPQ.pop();

    }
    return -1;
}