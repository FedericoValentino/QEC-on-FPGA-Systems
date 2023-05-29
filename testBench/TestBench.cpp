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


void singleCorrectionTest()
{
	int syndrome[SYN_LEN] = {0, 0, 0, 1, 1, 1, 0, 0, 1};
	//0 0 0 0 0 0 0 0 1 0 0 0 1 0 0 0 0 0
	//0 0 0 0 0 0 0 0 1 0 0 0 1 0 0 0 0 0
	ap_uint<CORR_LEN> correctionTest = 0;
	int correctionArr[CORR_LEN] = {0};

	Decoder decoder;

	correctionTest = decoder.decode(syndrome);
	decoder.clear();

	for(int i = 0; i < CORR_LEN; ++i)
	{
		correctionArr[i] = correctionTest[i];
	}


	printf("\nOur decoding: \n");
	for(int i = 0; i < CORR_LEN; ++i)
	{
		printf("%d ", correctionArr[i]);
	}


}


void simpleCorrectionTest()
{
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");
	//FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\test.txt","r");
	//FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");

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

	//FILE* f = fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\Decoder_dataset.txt","r");
	FILE* f = fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/Decoder_dataset.txt","r");

	Decoder decoder;
	int logicals[K][N] = {0};
	int syndrome[SYN_LEN] = {0};
	ap_uint<CORR_LEN> correction = 0;
	int check[K];
	int bitstring[K];
    int accuracy=0;
    std::chrono::nanoseconds total=static_cast<std::chrono::nanoseconds>(0);



    fgetc(f); //first bracket
    fgetc(f); //second bracket
    for(int i=0; i<K && !feof(f); i++){

    	for(int j=0; j<N && !feof(f); j++){
    		logicals[i][j]=fgetc(f)-48;
            fgetc(f);//space or bracket
    	}

        fgetc(f);//end of line
        fgetc(f);//space
        fgetc(f);//bracket
    }

    while(!feof(f)){

        for(int i=0; i<SYN_LEN && !feof(f); i++){
            syndrome[i]=fgetc(f)-48;
            fgetc(f);//space
        }

        fgetc(f);//end of line
        fgetc(f);//first square bracket

        for(int i=0; i<K && !feof(f); i++){
        	check[i]=fgetc(f)-48;
        	fgetc(f); //space or bracket
        }

        fgetc(f);//end of line
        fgetc(f); //next square bracket
        auto start=std::chrono::high_resolution_clock::now();
        correction = decoder.decode(syndrome);
        decoder.clear();
        auto stop=std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
        total=total+duration;


        for(int i=0; i<K; i++){
        	bitstring[i]=0;

        	for(int j=0; j<CORR_LEN; j++)
        		bitstring[i]+=logicals[i][j]*correction[j];

        	bitstring[i]=bitstring[i]%2;
        }


        if(check[0] == bitstring[0] && check[1] == bitstring[1])
        {
        	printf("Correct!\n");
        	++accuracy;
        }

    }
	printf("\nDecoding test concluded with accuracy %f%\nAverage running time: %f",(float)accuracy/5,(float)total.count()/500);

}

void mapTest()
{
	Map<uint32_t, uint32_t> test;

	test.add(1, 45);
	test.add(2, 78);
	test.add(4, 192);

	assert(test.find(1) == 45);

	test.update(1, 49);

	assert(test.find(1) == 49);

	printf("ALL MAP TESTS WERE SUCCESSFUL\n");

}


void vectorTest()
{
	Vector<uint32_t> test;
	test.emplace(0);
	test.emplace(1);
	test.emplace(2);
	test.emplace(4);
	test.elementEmplace(5);
	test.elementEmplace(3);
	test.elementEmplace(7);
	test.elementEmplace(8);
	test.elementEmplace(6);

	assert(test.getSize() == 9);

	assert(test.at(0) == 0);
	assert(test.at(3) == 3);

	test.set(2, 3);

	assert(test.at(3) == 2);

	printf("ALL VECTOR TESTs WERE SUCCESSFUL\n");

}



void hashTest(){

	//FILE* f=fopen("C:\\Users\\valef\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	//FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");
	//FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");

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
	//mapTest();
	//singleCorrectionTest();
	//simpleCorrectionTest();
	correctionTest();
}
