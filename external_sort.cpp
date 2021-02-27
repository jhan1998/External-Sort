#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <time.h>
#include <bits/stdc++.h> 
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

using namespace std;
#define READ_SIZE 20
#define WRITE_SIZE 300000000
char read_buff[20];
stringstream output_buffer;
int buf_counter = 0;

typedef struct heap_node{
    int data;
    int file;
} heap_node;

void swap(heap_node *x, heap_node *y);

class min_heap
{
private:
    heap_node *harr;
    int heap_size;

public:
    min_heap(heap_node node[], int size);
    void heapify(int);
    int left(int i){ return 2 * i + 1;}
    int right(int i){ return 2 * i + 2;}
    heap_node get_min(){return harr[0];}
    void pop_min(heap_node x){
        harr[0] = x;
        heapify(0);
    }
    ~min_heap();
};

min_heap::min_heap(heap_node node[], int size)
{
    harr = node;
    heap_size = size;
    int current = (heap_size / 2) + 1;
    while(current >= 0){
        heapify(current);
        current--;
    }
}

min_heap::~min_heap()
{
}

void min_heap::heapify(int x){
    int left_node = left(x);
    int right_node = right(x);
    int s = x;
    if(left_node < heap_size && harr[left_node].data < harr[s].data)
        s = left_node;
    if(right_node < heap_size && harr[right_node].data < harr[s].data)
        s = right_node;
    if(s != x){
        swap(harr + x, harr + s);
        heapify(s);
    }
}

void swap(heap_node *x, heap_node *y){
    heap_node tmp;
    tmp = *x;
    *x = *y;
    *y = tmp;  
}

static inline int numcpy(char *dest, char *src) {
    int cnt = 0;
    while(src[cnt] >= '0' && src[cnt] <= '9' || src[cnt] == '-') {
        dest[cnt] = src[cnt];
        cnt++;
    } 
    return cnt;
}

FILE* openfile(const char *filename, const char *mode){
    FILE *fp = fopen(filename, mode);
    if(!fp){
        cerr <<"Error while opening the file.\n"; 
        exit(1);
    }
    return fp;
}
void merge_F(int l, int r){
    stringstream ss;
    ss << l;
    string name1 = ss.str();
    FILE *n1 = openfile((char*)name1.c_str(), "r");
    ss.str("");
    ss << r;
    string name2 = ss.str();
    FILE *n2 = openfile((char*)name2.c_str(), "r");
    FILE *out = openfile("out.txt", "w");
    double s, e;
    s = clock();
    int tmp1, tmp2;
    vector <int> out_buf;
    char read_buff1[READ_SIZE], read_buff2[READ_SIZE];
    fread(&tmp1, sizeof(vector<int>::value_type), 1, n1);
    fread(&tmp2, sizeof(vector<int>::value_type), 1, n2);
    while(1){
        if(tmp1 <= tmp2){
            //output_buffer << tmp1 << "\n";
            buf_counter++;
            out_buf.push_back(tmp1);
            if(fread(&tmp1, sizeof(vector<int>::value_type), 1, n1))
                continue;
            else
                break;
            //fscanf(n1,"%d",&tmp1);
        }
        else {
            //output_buffer << tmp2 << "\n";
            buf_counter++;
            out_buf.push_back(tmp2);
            if(fread(&tmp2, sizeof(vector<int>::value_type), 1, n2))
                continue;
            else    
                break;
            //fscanf(n2,"%d",&tmp2);
        }
        if(buf_counter == WRITE_SIZE){
            fwrite(&out_buf[0], sizeof(vector<int>::value_type), buf_counter, out);
            buf_counter = 0;
            out_buf.clear();
            //fputs(output_buffer.str().c_str(), out);
            //output_buffer.str("");
        }
    }
    if(feof(n1)){
        //output_buffer << tmp2 << "\n";
        out_buf.push_back(tmp2);
        buf_counter++;
    }
    else {
        //output_buffer << tmp1 << "\n";
        out_buf.push_back(tmp1);
        buf_counter++;
    }
    while(fread(&tmp1, sizeof(vector<int>::value_type), 1, n1)){
        //tmp1 = atoi(read_buff1);
        //output_buffer << tmp1 << "\n";
        out_buf.push_back(tmp1);
        buf_counter++;
        if(buf_counter == WRITE_SIZE){
            fwrite(&out_buf[0], sizeof(vector<int>::value_type), buf_counter, out);
            buf_counter = 0;
            out_buf.clear();
            //fputs(output_buffer.str().c_str(), out);
            //output_buffer.str("");
        }
    }
     while(fread(&tmp2, sizeof(vector<int>::value_type), 1, n2)){
        //tmp2 = atoi(read_buff2);
        //output_buffer << tmp2 << "\n";
        out_buf.push_back(tmp2);
        buf_counter++;
        if(buf_counter == WRITE_SIZE){
            fwrite(&out_buf[0], sizeof(vector<int>::value_type), buf_counter, out);
            buf_counter = 0;
            out_buf.clear();
            //fputs(output_buffer.str().c_str(), out);
            //output_buffer.str("");
        }
    }
    fwrite(&out_buf[0], sizeof(vector<int>::value_type), buf_counter, out);
    //fputs(output_buffer.str().c_str(), out);
    buf_counter = 0;
    out_buf.clear();
    //output_buffer.str("");
    e = clock();
    cout << "merge file time " << ((e - s)/CLOCKS_PER_SEC) << endl;
    fclose(n1);
    fclose(n2);
    remove((char*)name1.c_str());
    remove((char*)name2.c_str());
    rename("out.txt",(char*)name1.c_str());
    fclose(out);
}

void merge_file(int l, int r){
    if(l < r){
        int m = l + (r - l) / 2;
        merge_file(l, m);
        merge_file(m+1, r);
        cout << "Merge File " << l << "------" << m + 1 << endl;
        merge_F(l, m+1);
    }
}

void partition(const char *input, int ways, int data_nums){
    FILE *in_file = openfile(input, "r");
    unsigned long long int max = data_nums / ways;
    double merge_time = 0, in_time = 0, out_time = 0;
    for(int i = 0; i < ways; i++){
        unsigned long long int count = 0;
        vector<int> in;
        double s, e;
        s = clock();
        cout << "IO part" << endl;
        while (count < max && fgets(read_buff, READ_SIZE, in_file)){
            in.push_back(atoi(read_buff));
            count++;
        }
        if(i == ways - 1){
            while(fgets(read_buff, READ_SIZE, in_file)){
                in.push_back(atoi(read_buff));
                count++;
            }
        }
        e = clock();
        in_time +=(e-s);
        
        s = clock();
        cout << "sort part\n";
        //merge_sort(in, 0, in.size() - 1);
        //q_sort(queue);
        sort(in.begin(), in.end());
        e = clock();
        merge_time += (e - s);
        stringstream ss;
        ss << i;
        string name = ss.str();
        FILE *out = openfile((char*)name.c_str(), "w");
        s = clock();
        // for(int i = 0; i < in.size(); i++){
        //     output_buffer << in[i] << "\n";
        //     buf_counter++;
        //     if(buf_counter == WRITE_SIZE){
        //         fputs(output_buffer.str().c_str(), out);
        //         buf_counter = 0;
        //         output_buffer.str("");
        //     }
        // }
        cout << "output part\n";
        fwrite(&in[0], sizeof(vector<int>::value_type), count, out);
        in.clear();
        vector<int>().swap(in);
        fputs(output_buffer.str().c_str(), out);
        buf_counter = 0;
        output_buffer.str("");
        e = clock();
        out_time += (e - s);
        fclose(out);
        in.clear();
        cout << "Partition " << i << " finish" <<endl;
    }
    fclose(in_file);
    cout << "total input time :" << in_time/CLOCKS_PER_SEC << endl;
    cout << "total sort time :" << merge_time/CLOCKS_PER_SEC << endl;
    cout << "total output time :" << out_time/CLOCKS_PER_SEC << endl;
}

void Merge_File_Buffer(int page_nums, int buffer_nums){
    int turns = 0;
    if(page_nums == 0 || page_nums == 1)
        return;
    while((turns * buffer_nums) < page_nums){
        int begin_page = turns * buffer_nums;
        int end_page = (begin_page + buffer_nums) > page_nums ? page_nums : (begin_page + buffer_nums);
        int way = end_page - begin_page;
        FILE *in[way], *out = openfile("out", "w");
        int in_fd[way];
        void *input[way];
        struct stat st[way];
        for(int i = begin_page; i < end_page; i++){
            stringstream ss;
            ss << i;
            //in[i-begin_page] = openfile((char*)ss.str().c_str(), "r");
            in_fd[i-begin_page] = open((char*)ss.str().c_str(),O_RDONLY);
            if(in_fd[i-begin_page] == -1){
                cout << "failed to open\n";
                exit(1);
            }
            fstat(in_fd[i-begin_page], &st[i-begin_page]);
            input[i-begin_page] = mmap(NULL, st[i-begin_page].st_size, PROT_READ, MAP_PRIVATE, in_fd[i-begin_page], 0);
            if((intptr_t)input[i-begin_page] == -1){
                munmap(input[i-begin_page],st[i-begin_page].st_size);
                close(in_fd[i-begin_page]);
                exit(2);
            }
            ss.str("");
        }

        heap_node *node = new heap_node[way];
        vector<int>::value_type *cur[way];
        for(int i = begin_page; i < end_page; i++){
            cur[i-begin_page] = (int*)input[i-begin_page];
            //fgets(buf,READ_SIZE,in[i-begin_page]);
            node[i-begin_page].file = i;
            node[i-begin_page].data = *cur[i-begin_page];
            cur[i-begin_page]++;
        }
        min_heap heap(node, way);
        int count = 0;
        vector<int> tmp;
        while(count != way){
            heap_node min = heap.get_min();
            tmp.push_back(min.data);
            //output_buffer << min.data << "\n";
            buf_counter++;
            if(buf_counter == WRITE_SIZE){
                //fputs(output_buffer.str().c_str(), out);
                fwrite(&tmp[0], sizeof(vector<int>::value_type), buf_counter, out);
                buf_counter = 0;
                //output_buffer.str("");
                tmp.clear();
            }
            char buf[READ_SIZE];
            if((intptr_t)cur[min.file - begin_page] - (intptr_t)input[min.file - begin_page] < st[min.file-begin_page].st_size){
                min.data = *cur[min.file - begin_page];
                cur[min.file - begin_page]++;
            }
            else {
                min.data = INT_MAX;
                count++;
            }
            heap.pop_min(min);
        }
        //fputs(output_buffer.str().c_str(), out);
        fwrite(&tmp[0], sizeof(vector<int>::value_type), buf_counter, out);
        buf_counter = 0;
        tmp.clear();
        //output_buffer.str("");
        for(int i = begin_page; i < end_page; i++){
            //fclose(in[i-begin_page]);
            munmap(input[i-begin_page], st[i-begin_page].st_size);
            close(in_fd[i-begin_page]);
            stringstream ss;
            ss << i;
            remove(ss.str().c_str());
            ss.str("");
        }
        fclose(out);
        stringstream new_name;
        new_name << (begin_page/buffer_nums);
        rename("out", new_name.str().c_str());
        turns++;
        cout << "merge " << begin_page << " -------->" << end_page-1 << endl;
    }
    Merge_File_Buffer(turns, buffer_nums);
}

int main(int argc, char *argv[]){
    if(argc < 5){
        cout << "Please enter nums of data and pages and ways and mode" << endl;
        exit(1);
    }
    unsigned long long int nums = atoll(argv[1]);
    unsigned long long int pages = atoi(argv[2]);
    int way = atoi(argv[3]);
    int mode = atoi(argv[4]);
    cout << "Nums of data : " << nums << endl;
    cout << "Nums of ways : " << pages << endl;
    cout << "Nums of data in one page : " << (nums/pages) << endl;
    double START, END;
    START = clock();
    partition("input.txt", pages, nums);
    cout << "merge start " << endl;
    //merge_file(0, pages-1);
    if(mode == 0){
        merge_file(0, pages-1);
        int in_fd = open("0",O_RDONLY);
        if(in_fd == -1){
                cout << "failed to open\n";
                exit(1);
        }
        struct stat st;
        fstat(in_fd, &st);
        void *input = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, in_fd, 0);
        if((intptr_t)input == -1){
            munmap(input,st.st_size);
            close(in_fd);
            exit(2);
        }
        FILE *out = fopen("output.txt", "w");\
        vector<int>::value_type *cur = (int*)input;
        while((intptr_t)cur - (intptr_t)input < st.st_size){
            output_buffer << *cur << "\n";
            cur++;
            buf_counter++;
            if(buf_counter == WRITE_SIZE){
                fputs(output_buffer.str().c_str(),out);
                buf_counter = 0;
                output_buffer.str("");
            }
        }
        munmap(input, st.st_size);
        close(in_fd);
        fputs(output_buffer.str().c_str(),out);
        fclose(out);
        buf_counter = 0;
        output_buffer.str("");
        remove("0");
    }
    else if(mode == 1){
        Merge_File_Buffer(pages, way);
        int in_fd = open("0",O_RDONLY);
        if(in_fd == -1){
                cout << "failed to open\n";
                exit(1);
        }
        struct stat st;
        fstat(in_fd, &st);
        void *input = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, in_fd, 0);
        if((intptr_t)input == -1){
            munmap(input,st.st_size);
            close(in_fd);
            exit(2);
        }
        FILE *out = fopen("output.txt", "w");\
        vector<int>::value_type *cur = (int*)input;
        while((intptr_t)cur - (intptr_t)input < st.st_size){
            output_buffer << *cur << "\n";
            cur++;
            buf_counter++;
            if(buf_counter == WRITE_SIZE){
                fputs(output_buffer.str().c_str(),out);
                buf_counter = 0;
                output_buffer.str("");
            }
        }
        munmap(input, st.st_size);
        close(in_fd);
        fputs(output_buffer.str().c_str(),out);
        fclose(out);
        buf_counter = 0;
        output_buffer.str("");
        remove("0");
    }
    END = clock();
    cout << "EXTERNAL SORTING TIME : " << (END - START) / CLOCKS_PER_SEC << endl;
    return 0;
}
