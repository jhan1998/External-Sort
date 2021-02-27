#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <iostream>
using namespace std;
#define MAX 300000000

int main(int argc, char **argv){
    srand(time(NULL));
    FILE *fp;
    fp = fopen("input.txt", "w");
    if(argc < 2) {
        printf("Please enter the nums of data\n");
        exit(1);
    }
    stringstream out;
    int counter = 0;
    long long int num = atoll(argv[1]);
    for (long long int i = 0; i < num; i++)
    {
        int x = rand() >> 16 | rand() << 16;
        out << x << "\n";
        counter++;
        if(counter == MAX){
            fputs(out.str().c_str(),fp);
            out.str("");
            counter = 0;
        }
    }
    fputs(out.str().c_str(),fp);
    out.str("");
    fclose(fp);
    return 0;
}
