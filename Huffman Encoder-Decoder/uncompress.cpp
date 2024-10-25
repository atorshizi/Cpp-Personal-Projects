#include "Helper.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <bitset>
using namespace std;


void recDelete(HCNode* currNode){
    if (currNode->c0 != nullptr){
        recDelete(currNode->c0);
    }
    if (currNode->c1 != nullptr){
        recDelete(currNode->c1);
    }
    if ((currNode->c0 == nullptr) && (currNode->c1 == nullptr)){
        if (currNode->p != nullptr){
            if(currNode->p->c0 == currNode){
                currNode->p->c0 = nullptr;
            }
            else if(currNode->p->c1 == currNode){
                currNode->p->c1 = nullptr;
            }
        }
        delete currNode;
    }
}
// unordered_map<unsigned char,string> encodings;
int main(int argc, char* argv[]) {
    FancyInputStream inStream(argv[1]);
    if (inStream.filesize() == 0){
        FancyOutputStream inStream(argv[2]);
        return 0;
    }
    unordered_map<int,unsigned int> counts;
    int origFileSz = inStream.read_int();
    int readInt;
    for(int i = 0 ; i < 256 ; i++){
        readInt = inStream.read_byte() * 65536 + inStream.read_byte() * 256 + inStream.read_byte();
        if (readInt != 0){
            pair<int, unsigned int> newPair(i,readInt);
            counts.insert(newPair);
        }
    }
    // for (auto elem : counts){
    //     cout << elem.first << " " << elem.second << endl;
    // }
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    for (auto elem : counts){
        pq.push(new HCNode(elem.second,elem.first));
    }

    HCNode* first;
    HCNode* second;
    while (pq.size() > 1){
        first = pq.top();
        pq.pop();
        second = pq.top();
        pq.pop();
        HCNode* combo = new HCNode((first->count)+(second->count), first->symbol);
        combo->c0 = first;
        first->p = combo;
        combo->c1 = second;
        second->p = combo;
        combo->p = nullptr;
        pq.push(combo);
    }
    HCNode* currNode = pq.top();

    int currBit;
    FancyOutputStream outStream(argv[2]);
    while (((currBit = inStream.read_bit()) != -1) && (origFileSz > 0)){
        if ((currNode->c0 == nullptr) && (currNode->c1 == nullptr)){
            outStream.write_byte(currNode->symbol);
            origFileSz--;
            currNode = pq.top();
        }
        if (currBit == 0){
            currNode = currNode->c0;
        }
        else {
            currNode = currNode->c1;
        }
    }
    if ((currNode->c0 == nullptr) && (currNode->c1 == nullptr)){
        outStream.write_byte(currNode->symbol);
    }

    recDelete(pq.top());
}




