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
    FILE* f=fopen("../testBench/data.txt","r");
    int syndrome[SYN_LEN];
    int correction[CORR_LEN];
    int i;
    int perc=0;
    Lint corrDec;
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
        corrDec = binaryToDec(correction);
        decoderTop(syndrome,&corrDec,true);
        assert(decoderLUT.retrieve(syndrome)==corrDec);
        ++perc;
        printf("%f %\n",(float)perc/5000);
    }
    printf("ALL TESTs WERE SUCCESSFUL");
}
