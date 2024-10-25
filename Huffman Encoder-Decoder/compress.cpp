#include "Helper.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <bitset>
using namespace std;

unordered_map<unsigned char,string> encodings;
void recHelper(HCNode* currNode, HCNode* root){
    if (currNode->c0 != nullptr){
        recHelper(currNode->c0, root);
    }
    if (currNode->c1 != nullptr){
        recHelper(currNode->c1, root);
    }
    if ((currNode->c0 == nullptr) && (currNode->c1 == nullptr)){
        string encodingStr = "";
        HCNode* travPtr = currNode;
        while (travPtr != root){
            if (travPtr->p->c0 == travPtr){
                encodingStr += "0";
            }
            else if (travPtr->p->c1 == travPtr){
                encodingStr += "1";
            }
            travPtr = travPtr->p;
        }
        reverse(encodingStr.begin(), encodingStr.end());
        pair<unsigned char, string> newPair(currNode->symbol, encodingStr);
        encodings.insert(newPair);
    }
}
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

int main(int argc, char* argv[]) {
    // your program's main execution code
    FancyInputStream inStream(argv[1]);
    if (!inStream.good()){
        return EXIT_FAILURE;
    }
    unordered_map<int,unsigned int> counts;
    int fileSz = inStream.filesize();
    if (fileSz == 0){
        FancyOutputStream outStream(argv[2]);
        return 0;
    }
    for (int i = 0 ; i < fileSz ; i++){
        int nextByte = inStream.read_byte();
        if (counts.find(nextByte) != counts.end()){
            unsigned int count = (counts.find(nextByte)->second);
            pair<int, unsigned int> newPair(nextByte, count+1);
            counts.erase(nextByte);
            counts.insert(newPair);
        }
        else {
            pair<int, unsigned int> newPair(nextByte, 1);
            counts.insert(newPair);
        }
    }
    inStream.reset();

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
    HCNode* root = pq.top();
    recHelper(root,root);
        

    unsigned char charByte;
    FancyOutputStream outStream(argv[2]);


    // make the header
    outStream.write_int(inStream.filesize());

    for (unsigned int i = 0 ; i < 256 ; i++){
        if (counts.find(i) != counts.end()){
            bitset<24> bs((counts.find(i))->second);
            string bits = (bs.to_string());
            // cout << i << " " << bits << endl;
            for (int i = 0; i<24;i++){
                char bitForOut = bits.at(i);
                if (bitForOut == '0'){
                    outStream.write_bit(0);
                }
                else {
                    outStream.write_bit(1);
                }
            }
        }
        else{
            for (int i = 0; i<3;i++){
                outStream.write_byte(0);
            }
        }
    }

    for (int i = 0 ; i < inStream.filesize() ; i++){
        charByte = (unsigned char) inStream.read_byte();
        string encStr = (encodings.find(charByte))->second;
        const char* charPtr = encStr.c_str();
        while (*charPtr != '\0'){
            if (*charPtr == '0'){
                outStream.write_bit(0);
            }
            else {
                outStream.write_bit(1);
            }
            charPtr++;
        }
    }
    outStream.flush_bitwise();
    // for (auto elem : encodings){
    //     cout << elem.first << " " << elem.second << endl;
    // }
    recDelete(root);
}

