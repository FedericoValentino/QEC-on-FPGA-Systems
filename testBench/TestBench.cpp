#include <assert.h>
#include <stdio.h>


#include "../src/Utility/Map.h"
#include "../src/Utility/Vector.h"
#include "../src/Project.h"


Lint binaryToDec(int array[CORR_LEN]){
    Lint sum = 0;
    for(int i = 0; i < CORR_LEN; i++){
        sum += array[i] * (1 << i);
    }
    return sum;
}


void MapTest()
{
	Map<uint32_t, uint32_t> test;

	test.add(1, 45);
	test.add(2, 78);
	test.add(4, 192);

	assert(*test.find(1) == 45);


	test.update(1, 49);

	assert(*test.find(1) == 49);

	printf("ALL MAP TESTS WERE SUCCESSFUL\n");

}


void vectorTest()
{
	Vector<uint32_t> test;

	test.emplace(4);
	test.emplace(4);
	test.emplace(4);

	assert(test.getSize() == 3);

	assert(test.at(0) == test.at(1));
	assert(test.at(1) == test.at(2));

	test.insert(3, 0);

	assert(test.at(0) == 3);
	assert(test.at(3) == 4);

	test.set(2, 3);

	assert(test.at(3) == 2);

	printf("ALL VECTOR TESTs WERE SUCCESSFUL\n");

}






void hashTest(){
    //for the moment just change the commented line for testing, TODO
	//FILE* f=fopen("C:\\Users\\valef\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
    //FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/2000samples.txt","r");
	int syndrome[SYN_LEN] = {0};
    ap_uint<CORR_LEN> correction = 0;
    ap_uint<CORR_LEN> correctionTest = 0;
    int i;
    int perc=0;
    while(!feof(f)){
        fgetc(f);
        for(i=0;i<SYN_LEN;++i){
            syndrome[i]=fgetc(f)-48;
            fgetc(f);
        }
        fgetc(f);
        fgetc(f);
        for(i=0;i<CORR_LEN;++i){
            correction[i]=fgetc(f)-48;
            fgetc(f);
        }
        fgetc(f);
        decoderTop(syndrome,&correction,true);

        correctionTest = decoderLUT.retrieve(syndrome);
        assert(correctionTest == correction);
        ++perc;
        //printf("Execution n: %d \n",perc);

    }
    printf("ALL HASHTABLE TESTs WERE SUCCESSFUL\n");
}


int main()
{
	hashTest();
	vectorTest();
	MapTest();
}
