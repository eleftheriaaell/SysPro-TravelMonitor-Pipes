#include "hashtable.hpp"

HashTable::HashTable(int tableSize, int window){
    k = tableSize;
    w = window;
    hashTable = new list<bucketList>[k];
}
        
void HashTable::hashPush(int item){
    bucketList node;
    node.ID = item;
    hashTable[gp(node.ID)].push_front(node);
}

float HashTable::inner_product(data p){

    random_device rd; 
    mt19937 gen(rd()); 

    float v;
    float product = 0.0;
    
    for(int i = 0; i < p.d; ++i){
        v = 0.0;
        
        normal_distribution<float> d(0, 1); 
        v = d(gen); 
        
        product = product + v * p.p_data[i];
    }

    return product;
}

int HashTable::hi(data p){
    int inner_p = inner_product(p);

    int t = rand() % w;

    //cout << "t " << t << endl;

    int ans = (inner_p + t) / w;

    //cout << ans << endl;
    return ans;
}

void HashTable::hashFun(data p){

    int arr_hi[k];
    for(int i = 0; i < k; i++)
        arr_hi[i] = hi(p);

}

void HashTable::print(){
    
    for(int i = 0; i < k; i++){
        for(auto v: hashTable[i])
            cout << v.ID << " ";
        cout << endl;
    }

}