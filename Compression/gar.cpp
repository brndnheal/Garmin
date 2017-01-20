#include <iostream>
#include <vector>
#include <queue>

using namespace std;

vector<int8_t> freq_count(128,0);

//Custom node struct to store the Huffman Tree Nodes
struct node{
	    int8_t symbol;
	    int8_t value;
	    struct node *left,*right;
};

typedef struct node Node;
bool operator<(const Node &d1, const Node &d2){return d1.value < d2.value;}

//Counts the frequencies of each of the 128 character and stores in vector freq_count
void count_frequencies(int8_t * data_ptr,int data_size){

	for(int i = 0;i<data_size;i++){
		freq_count.at(data_ptr[i])++;
	}
	

	printf("\n");

}

void huffman_tree (Node **tree) {
        
	
	priority_queue<Node> pq;
	Node* leafs[128];
	
	//Create the leaf nodes and insert into priority queue
	for(int i=0; i<128; i++){
		leafs[i]= (Node *)malloc(sizeof(Node));
		leafs[i]->value = freq_count[i];
		leafs[i]->symbol = (int8_t)i;
		leafs[i]->left = NULL;
		leafs[i]->right = NULL;
		pq.push(*leafs[i]);
	}
}	

int main()
{
	
	 int data_size =  24;
	 int8_t raw_data[data_size] =  { 0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
		 		0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09 };

	 
	 count_frequencies(raw_data,data_size);

         Node* tree;
	 
	 huffman_tree(&tree);
	

/*	 for(vector<int8_t>::iterator it = compressed.begin(); it != compressed.end(); ++it) {
		 printf("%x",*it);
	 }*/
}

