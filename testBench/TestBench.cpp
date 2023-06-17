#include <assert.h>
#include <stdio.h>
#include <chrono>

#include "../src/ProjectC/FRATC.h"


void debugd8()
{
	bool syndrome[SYN_LEN] = {0,0,1,0,0,0,1,0,0};
	bool correction[CORR_LEN] = {0};
	int correctionArr[CORR_LEN] = {0};
	decoderTop(syndrome, nullptr, correction, false);
	for(int i = 0; i < CORR_LEN; ++i)
	{
		correctionArr[i] = correction[i];
	}
	printf("\nOur decoding: \n");
	for(int i = 0; i < CORR_LEN; ++i)
	{
		printf("%d ", correctionArr[i]);
	}
}


void correctionTest(){

	//FILE* f = fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\Decoder_dataset.txt","r");
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/Decoder_dataset.txt","r");

	int logicals[K][N] = {0};
	bool syndrome[SYN_LEN] = {0};
	bool correction[CORR_LEN] = {0};
	int correctionArr[CORR_LEN] = {0};
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
        decoderTop(syndrome, nullptr, correction, false);
        auto stop=std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
        total=total+duration;
        for(int i = 0; i < CORR_LEN; ++i)
		{
			correctionArr[i] = correction[i];
		}


        for(int i=0; i<K; i++){
        	bitstring[i]=0;

        	for(int j=0; j<CORR_LEN; j++)
        		bitstring[i]+=logicals[i][j]*correctionArr[j];

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


void hashTest(){

	//FILE* f=fopen("C:\\Users\\valef\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	//FILE* f=fopen("C:\\Users\\mikim\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
	FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");
	//FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\LUT.txt","r");

	bool syndrome[SYN_LEN] = {0};
    bool correction[CORR_LEN] = {0};
    bool correctionTest[CORR_LEN] = {0};

    for(int i=0;i<CORR_LEN;i++)
	{
	   printf("%ld ", correction[i]);
	}
    printf("\n");

    int perc=0;
    while(!feof(f) && perc < 50){
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
        decoderTop(syndrome, correction, nullptr, true);

        decoderTop(syndrome, nullptr, correctionTest,false);
        for(int i=0;i<CORR_LEN;i++)
        {
		   printf("%ld ", correction[i]);
        }
        printf("\n");
        for(int i=0;i<CORR_LEN;i++)
		{
		   printf("%ld ", correctionTest[i]);
		}
        printf("\n");

        for(int i=0;i<CORR_LEN;i++)
		{
		   assert(correction[i] == correctionTest[i]);
		}
        ++perc;
        //printf("Execution n: %d \n",perc);

    }
    printf("ALL HASHTABLE TESTs WERE SUCCESSFUL\n");
}


void COSIM()
{
		FILE* f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");


		bool syndrome[SYN_LEN] = {0};
		int logicals[K][N] = {0};
		bool correction[CORR_LEN] = {0};
		int check[K] = {0};
		int bitstring[K]= {0};
		int accuracy=0;

		std::chrono::nanoseconds total=static_cast<std::chrono::nanoseconds>(0);

		while(!feof(f))
		{
			fgetc(f); //square bracket
			for(int i=0;i<SYN_LEN;i++)
			{
				syndrome[i]=fgetc(f)-48;
				fgetc(f); //space
			}
			fgetc(f); //end of line
			fgetc(f); //square bracket
			for(int i=0;i<CORR_LEN;i++)
			{
				correction[i]=fgetc(f)-48;
				fgetc(f);
			}
			fgetc(f);
			decoderTop(syndrome,correction, nullptr, true);
		}
		printf("LUT is loaded");

		f=fopen("/home/feder34/git/QEC-on-FPGA-Systems/testBench/Decoder_dataset.txt","r");


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
	        decoderTop(syndrome, nullptr, correction, false);
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
	        	++accuracy;
	        }

	    }

	    printf("\nCOSIM concluded with accuracy %f%\nAverage running time: %f",(float)accuracy*100.0/500.0,(float)total.count()/500);

}



int main()
{
	COSIM();
	return 0;
}
