#include <iostream>
#include <vector> 
using namespace std;
 
vector<int8_t> compress(int8_t * data_ptr,int data_size){
        vector<int8_t> compressed;
	int8_t next;
	int8_t count=0;
	for (int i =0; i < data_size-1; i++){
		count++;
		next= data_ptr[i+1];
		if(next!=data_ptr[i]){

			compressed.push_back(data_ptr[i]);
			compressed.push_back(count);			
			if (i==data_size-2){
				compressed.push_back(data_ptr[i+1]);
				compressed.push_back(1);			
			   
			}
			count=0;
			
		}
		else if(i==data_size-2){
			compressed.push_back(data_ptr[i]);
			compressed.push_back(count+1);			
		}

	}
	printf("\n");
	return compressed;
}	

int main()
{
	
	 int data_size =  24;
	 int8_t raw_data[data_size] =  { 0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
		 		0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09 };
	 vector<int8_t> compressed= compress(raw_data,data_size);
	
	 for(vector<int8_t>::iterator it = compressed.begin(); it != compressed.end(); ++it) {
		 printf("%x",*it);
	 }
}

