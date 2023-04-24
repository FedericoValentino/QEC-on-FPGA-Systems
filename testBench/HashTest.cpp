#include "../src/Project.h"
#include <assert.h>
#include <stdio.h>

Lint binaryToDec(int array[CORR_LEN]){
    Lint sum = 0;
    for(int i = 0; i < CORR_LEN; i++){
        sum += array[i] * (1 << i);
    }
    return sum;
}

int main(){
    //for the moment just change the commented line for testing, TODO
	//FILE* f=fopen("C:\\Users\\valef\\git\\QEC-on-FPGA-Systems\\testBench\\data.txt","r");
    FILE* f=fopen("C:\\Users\\franc\\git\\QEC-on-FPGA-Systems\\testBench\\2000samples.txt","r");
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
        printf("Execution n: %d \n",perc);

    }
    printf("ALL TESTs WERE SUCCESSFUL");
}
