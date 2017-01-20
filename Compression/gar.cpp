#include <iostream>
#include <vector>
#include <queue>

using namespace std;

vector<int8_t> freq_count(128,0);
vector<int> encodings(128,0);

//Custom node struct to store the Huffman Tree Nodes
typedef struct Node{
	    int symbol;
	    uint8_t value;
	    struct Node *left,*right;
} *Node_wsk;


struct MyComparator {
	    bool operator() (Node_wsk arg1, Node_wsk arg2) {
		 return arg1->value > arg2->value; //calls your operator
	}
};       


//Highest priority will be one with lowest probability/frequency
//bool operator<(const Node*d1, const Node *d2){return d1.value > d2.value;}

//Counts the frequencies of each of the 128 character and stores in vector freq_count
void count_frequencies(uint8_t * data_ptr,int data_size){

	for(int i = 0;i<data_size;i++){
		freq_count.at(data_ptr[i])++;
	}
	

	printf("\n");

}

//Constructs the Huffman Tree using a priority queue - O(nlogn)
void huffman_tree (Node **tree) {
	
	priority_queue<Node_wsk, vector<Node_wsk>, MyComparator> pq;
	Node_wsk leafs[128];
	
	//Create the leaf nodes and insert into priority queue
	for(int i=0; i<128; i++){
		Node_wsk temp = new Node;
		leafs[i]=temp;
		leafs[i]->value = freq_count[i];
		leafs[i]->symbol = i;
		leafs[i]->left = NULL;
		leafs[i]->right = NULL;
		pq.push(leafs[i]);
	}

	//while there is more than one node in the queue:
	//1. Remove two nodes of highest priority/lowest probability
	//2. Create new node with these nodes as children and value equal to sum of their values
	//3. Add this node to the queue
	int symbol_counter=128;
	Node_wsk first, second, parent_node;
	while(pq.size()>1){
		first=pq.top();
		pq.pop();
		second=pq.top();
		pq.pop();
		parent_node = (Node *)malloc(sizeof(Node));
		parent_node->value=(first->value)+(second->value);
		parent_node->symbol= symbol_counter++;
		parent_node->left= first;
		parent_node->right= second;
		pq.push(parent_node);
	}
	//Remaining Node is the root node
	first = pq.top();
	*tree=first;
	
	pq.pop();
}	

//Build encoding table using pre-order traversal of tree
void build_table(Node *tree, long long int code){
	if(tree==NULL || tree== 0){
		return;
	}
	
	if(tree->symbol<128 && (tree->symbol)>=0){
		encodings.at(tree->symbol)=code;
		printf("%d %d\n",code, tree->symbol);
	}
	else{
		build_table( tree->left, code*10+1);
		build_table( tree->right, code*10+2);
	}
	return;
}

int main()
{
	
	 int data_size =  24;
	 uint8_t raw_data[data_size] =  { 0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
		 		0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09 };

	 
	 count_frequencies(raw_data,data_size);

         Node* tree;
	 
	 //Assumes data and tree fit in memory
	 huffman_tree(&tree);
 	 build_table( tree, 0);

	 


/*	 for(vector<int8_t>::iterator it = compressed.begin(); it != compressed.end(); ++it) {
		 printf("%x",*it);
	 }*/
}

