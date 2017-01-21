#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <sstream>
using namespace std;


/* Compression Algorithm for Garmin: Implements Huffman Coding in O(nlogn) time
 * 
 * Computes probabilities of each symbol in set to maximize compression of most frequent symbols
 * Worst Case is therefore when all symbols have the same probability
 *
 * Assumptions: Data and Tree fit in Memory, input data is sound for both compression and decompression
 *
 * Possible Future Improvements:
 * 	-Support Command Line Arguments ( gar -c to compress, gar -x to extract)
 * 	-Support File Compression and Decompression
 *      -Raw Binary Output Format for truly compressed output
 *      -Decompress Raw Binary Output Format
 *      -Refactor to use bitsets instead of vector<bool>
 *      -Robust Testing
 */



//Hash map that counts frequency of each symbol
map< uint8_t, int > freq_count;
//Hash map that contains the encodings for each symbol 
map< uint8_t, vector <bool> > encodings;


//Custom node struct to store the Huffman Tree Nodes
typedef struct Node{
	    int symbol;
	    uint8_t value;
	    struct Node *left,*right;
} *Node_wsk;


//Comparator for priority queue - Highest priority will be one with lowest probability/frequency
struct MyComparator {
    	bool operator() (Node_wsk arg1, Node_wsk arg2) {
		 return arg1->value > arg2->value;
	}
};       

//Checks if we are at the end of an iterator 
template <typename Iter, typename Cont>
bool is_last(Iter iter, const Cont& cont)
{
    return (iter != cont.end()) && (next(iter) == cont.end());
}

//Counts the frequencies of each of the 128 symbols and stores in vector freq_count
//Returns the number of distinct symbols used
int count_frequencies(uint8_t * data_ptr,int data_size){

	int num_count=0;
	for(int i = 0;i<data_size;i++){
		if(freq_count[data_ptr[i]]==0) {
			num_count++;
		}

		freq_count[data_ptr[i]]++;
	}
        
	return num_count;

}

//Returns a string representation of a bit_vector
string print_bit_vector(vector<bool> bit_vector){
	string bit_string("");
	for (int i=0;i<bit_vector.size();i++){
		bit_vector.at(i) ? bit_string+="1" : bit_string+="0";
	}
	return bit_string;
}

//converts bit_vector to uint8_t, will break if bit_vector is too large - but it shouldn't ever be
uint8_t bitvector_to_int(vector<bool> bit_vec){

	auto p = bit_vec.begin()._M_p;
	return (int) *p;
}

//Constructs the Huffman Tree using a priority queue - O(nlogn)
void huffman_tree (Node **tree,int num_count) {
	
	priority_queue<Node_wsk, vector<Node_wsk>, MyComparator> pq;
	Node_wsk leafs[num_count];
	int i=0;	
	//Create the leaf nodes and insert into priority queue
	for(map<uint8_t,int>::iterator iter = freq_count.begin(); iter != freq_count.end(); ++iter)
	{
		Node_wsk temp = new Node;
		leafs[i]=temp;
		leafs[i]->value = iter->second;
		leafs[i]->symbol = iter->first;
		leafs[i]->left = NULL;
		leafs[i]->right = NULL;
		pq.push(leafs[i]);
		i++;
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

//Creates the compressed bit string representing the data by performing lookups on the encoding table - O(nlogn)
vector<bool> compress_data(uint8_t* data_ptr, int data_size){
	vector<bool> compressed_data(0);
	vector<bool> code;
	for(int i=0;i<data_size;i++){
		code=encodings[data_ptr[i]];
		compressed_data.insert(compressed_data.end(),code.begin(),code.end());
	}
	cout << print_bit_vector(compressed_data)<<"\n";
	return compressed_data;	
}

//Build encoding table using pre-order traversal of tree - O(n) 
void build_table(Node *tree, vector <bool> *code){
	if(tree==NULL ){
		return;
	}
	
	if(tree->symbol<128 && (tree->symbol)>=0){
		encodings[tree->symbol]=*code;
	}
	else{
		vector<bool> clone=*code;
		code->push_back(0);
		clone.push_back(1);
		build_table( tree->left, code );
		build_table( tree->right, &clone );
	}

	return;

}


//String Split function to help parse the table
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

//Parse the encoding table from the table string
map< vector<bool>, uint8_t > parse_table(string table_str){
	map< vector<bool>, uint8_t> table;
	vector<string> rows = split(table_str, ',');
	int symbol;  
	for(vector<string>::iterator iter = rows.begin();iter!=rows.end(); ++iter){
		vector<string> parts = split(*iter, ':');
		stringstream ss;
		ss << hex << parts[0];
		ss >> symbol;

        	vector <bool> code(0);	
		for(auto a : parts[1])
			code.push_back(a == '1');

		table[code]= symbol;
			
	}
	return table;
}


//Decompress the compressed string by rebuilding the encoding map and computing the values
vector<uint8_t> decompress(string compressed){
	
	string table_str=compressed.substr(1,compressed.find("}")-1);
	map< vector<bool>, uint8_t > table = parse_table(table_str);
	
	string code = compressed.substr(compressed.find("}")+1);

	string temp=code;
	
	vector<bool> lookup_str(0);
	vector<uint8_t> decompressed;
	while(temp.size()>0){
		if(table.count( lookup_str )){
			decompressed.push_back(table[lookup_str]);
			vector <bool> cleared(0);
			lookup_str=cleared;

		}
		(int) (temp.at(0)-'0') ? lookup_str.push_back(1) : lookup_str.push_back(0);
		temp.erase(0,1);

	}
	decompressed.push_back(table[lookup_str]);
	return decompressed;
}


//Returns string output representing encoding table
string print_table(){
	stringstream ss;
	ss << '{';
	for(map<uint8_t,vector<bool>>::iterator iter = encodings.begin(); iter != encodings.end(); iter++){
		ss << hex << (int)iter->first <<dec;
	        ss << (':' + print_bit_vector( iter->second ));
		if(!is_last(iter,encodings)){
			ss<<',';	
		}
	}
	ss << '}';
        return ss.str();
}

int main()
{	
	 int data_size = 128 ;


	 /*uint8_t raw_data[data_size] =  { 0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
		 		0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09,0x07, 0x07,0x07,0x07,0x07,0x12,0x12,0x12,0x12,0x12 };
	 */
	 uint8_t raw_data[data_size]={0};
	
	 //Check that there is data to compress	 
	 if(data_size==0){
		cout << "Input data is empty \n";
		exit(0);
	 }

	 //Printing the Raw Data :	
	 cout << "Raw Data : \n{";
	 for(int i =0; i< data_size; i++){
		cout<< "0x"<<hex << (int)raw_data[i];
		if (i!=data_size-1){
			cout << ", ";
		}

	 }
	 cout << "}\n\n" << dec;

	 //Compute the frequency of each symbol
	 int num_count = count_frequencies(raw_data, data_size);

         //Build the huffman tree based on frequency
	 Node* tree;
	 
	 huffman_tree(&tree, num_count);

	 //Build Encoding Table
	 vector<bool> code(0);
	 //Special case for data containing only one character (Root Node is the leaf)
	 if(num_count==1){
		 code.push_back(0);
	 }

 	 build_table( tree, &code);
	 	 
	 //Compress Data using encoding table
	 vector<bool> compressed_data = compress_data( raw_data, data_size );
	 
	 //Get readable output for table
	 string table = print_table();

	 cout << "Compressed Data in comprehensive format : \n" << dec<<table << print_bit_vector(compressed_data) << "\n\n";
	 cout << "Original Data Size was " << (int)data_size*8 <<" bits\n";
	 cout << "Compressed Data Size is " << compressed_data.size()<< " bits\n\n";
	 cout << "Compression Ratio without table is " <<    compressed_data.size() / ( data_size*8.0 ) *100.0 <<" % \n";
	 cout << "Overhead size for table storage is " << table.size()*8 <<" bits\n";
	 cout << "True Compression Ratio is "<< (compressed_data.size()+table.size()*8.0) / ( data_size*8.0 ) *100.0 <<" % \n\n";
	 
	 //Decompress the data from text-based format
	 vector<uint8_t> decompressed_data = decompress( table+print_bit_vector(compressed_data) );
	 cout<<"Decompressed Data : \n{";
	 for(vector<uint8_t>::iterator iter = decompressed_data.begin();iter!= decompressed_data.end(); ++iter){
		cout <<"0x" << hex<<(int)*iter;
		if (!is_last(iter,decompressed_data)){
			cout << ", ";
		}
	 }
	 cout<<"}\n";

}

