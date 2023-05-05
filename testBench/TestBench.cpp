#include <assert.h>
#include <stdio.h>
#include <chrono>


#include "../src/Utility/Map.h"
#include "../src/Utility/Vector.h"
#include "../src/Project.h"
#include "../src/UnionFind/Decoder.h"


Lint binaryToDec(int array[CORR_LEN]){
    Lint sum = 0;
    for(int i = 0; i < CORR_LEN; i++){
        sum += array[i] * (1 << i);
    }
    return sum;
}


void simpleCorrectionTest()
{
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");

	Decoder decoder;
	ap_uint<CORR_LEN> correctionTest = 0;
	int syndrome[SYN_LEN]={0};
	int correctionTestArr[CORR_LEN] = {0};
	int correctionArr[CORR_LEN] = {0};

	while(!feof(f))
	{
		bool ok = true;

		int i = 0;

		int count = 0;

		while(count != 2)
		{
			char c = fgetc(f);
			if(c == 49 || c == 48)
			{
				if(count == 0)
				{
					syndrome[i] = c-48;
					++i;
				}
				if(count == 1)
				{
					correctionArr[i] = c-48;
					++i;
				}
			}
			else if(c == 93)
			{
				i = 0;
				++count;
			}
		}


		correctionTest = decoder.decode(syndrome);
		decoder.clear();

		for(i = 0; i < CORR_LEN; ++i)
		{
			correctionTestArr[i] = correctionTest[i];
			if(correctionTestArr[i] != correctionArr[i])
			{
				ok = false;
				break;
			}
		}


		if(!ok)
			printf("\nSyndrome decoding was bad");
		else
			printf("\nSyndrome decoding was good");
	}

	printf("\nSimple Test Completed\n");

}


void correctionTest()
{
	Decoder decoder;
	int syndrome[SYN_LEN]={1, 0, 0, 0, 1, 0,
						   0, 0, 0, 1, 0, 0,
						   0, 0, 0, 0, 0, 1,
						   1, 1, 0, 0, 0, 1,
						   0, 0, 0, 0, 0, 1,
						   1, 1, 0, 0, 1, 1};
	int correctionTestArr[CORR_LEN] = {0};
	//1 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1
	//0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1 0
	ap_uint<CORR_LEN> correctionTest = 0;

	correctionTest = decoder.decode(syndrome);
	for(int i = 0; i < CORR_LEN; ++i)
	{
		correctionTestArr[i] = correctionTest[i];
		printf("%d ", correctionTestArr[i]);
	}

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
	//FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
    FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");
	//FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/2000samples.txt","r");
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
	/*hashTest();
	vectorTest();
	MapTest();
	simpleCorrectionTest();*/
	correctionTest();
}
