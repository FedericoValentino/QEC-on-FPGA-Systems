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
	//FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/test.txt","r");
	//FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\test.txt","r");
	FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");

	Decoder decoder;
	ap_uint<CORR_LEN> correctionTest = 0;
	int syndrome[SYN_LEN]={0};
	int syndromeOriginal[SYN_LEN] = {0};
	int correctionTestArr[CORR_LEN] = {0};
	int correctionArr[CORR_LEN] = {0};
	int correctedCount = 0;

	while(!feof(f))
	{
		bool ok = true;

		int i = 0;

		int count = 0;

		while(count != 2 && !feof(f))
		{
			char c = fgetc(f);
			if(c == 49 || c == 48)
			{
				if(count == 0)
				{
					syndrome[i] = c-48;
					syndromeOriginal[i] = c-48;
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
		}

		for(i = 0; i < CORR_LEN; ++i)
		{
			if(correctionTestArr[i] != correctionArr[i])
			{
				ok = false;
				break;
			}
		}


		if(ok)
		{
			printf("good\n");
			correctedCount++;
		}
		else
		{
			printf("incorrectly decoded syndrome: \n");
			for(i = 0; i < SYN_LEN; ++i)
			{
				printf("%d ", syndromeOriginal[i]);
			}
			printf("\nOur decoding: \n");
			for(i = 0; i < CORR_LEN; ++i)
			{
				printf("%d ", correctionTestArr[i]);
			}
			printf("\nCorrect Decoding: \n");
			for(i = 0; i < CORR_LEN; ++i)
			{
				printf("%d ", correctionArr[i]);
			}
			printf("\n");
		}

	}

	printf("\nSimple Test Completed\nCorrectly Decoded %f", (float)correctedCount/100.0f);

}


void correctionTest(){

	FILE* f = fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\Decoder_dataset.txt","r");
	//FILE* f = fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/Decoder_dataset.txt","r");

	Decoder decoder;
	int syndrome[SYN_LEN] = {0};
	int correctionTestArr[CORR_LEN] = {0};
	ap_uint<CORR_LEN> correction = 0;
	ap_uint<CORR_LEN> logicals[K] = 0;
    int bitstring[K];
    int check[K];
    int accuracy=0;

    while(!feof(f)){

    	fgetc(f); //first square bracket

        for(int i=0;i<SYN_LEN;i++){
            syndrome[i]=fgetc(f)-48;
            fgetc(f);//space
        }

        fgetc(f);//end of line
        fgetc(f);//first square bracket
        fgetc(f);//second square bracket

        for(int i=0;i<K;i++){
            for(int j=0;j<CORR_LEN;j++){
            	logicals[i][j]=fgetc(f)-48;
            	fgetc(f);//space
            }
            fgetc(f);//end of line
            fgetc(f);//bracket
            fgetc(f);//space
        }

        for(int i=0;i<K;i++){
        	check[i]=fgetc(f)-48;
        	fgetc(f);
        }
        fgetc(f);
        correction = decoder.decode(syndrome);
        decoder.clear();
        for(int i=0;i<K;i++){
        	bitstring[i]=0;
        	for(int j=0;j<CORR_LEN;j++){
        		bitstring[i]+=correction[j]*logicals[i][j];
        	}
        	bitstring[i]=bitstring[i]%2;
        }


        for(int i=0;i<K;i++){
        	//assert(check[i]==bitstring[i]);
        	if(check[i]!=bitstring[i])
        		accuracy++;
        }

    }
	printf("All test have been concluded with accuracy %f\n",(float)accuracy/1000);

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

	//FILE* f=fopen("C:\\Users\\valef\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	//FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	//FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/2000samples.txt","r");
	FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");

	int syndrome[SYN_LEN] = {0};
    ap_uint<CORR_LEN> correction = 0;
    ap_uint<CORR_LEN> correctionTest = 0;
    int perc=0;
    while(!feof(f)){
        fgetc(f); //square bracket
        for(int i=0;i<SYN_LEN;i++){
            syndrome[i]=fgetc(f)-48;
            fgetc(f); //space
        }
        fgetc(f); //end of line
        fgetc(f); //square bracket
        for(int i=0;i<CORR_LEN;i++){
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
	//hashTest();
	//vectorTest();
	//MapTest();
	simpleCorrectionTest();
	//correctionTest();
}
