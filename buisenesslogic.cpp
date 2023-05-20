#include "buisenesslogic.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string.h>

FuncReturningValue* getDataFromFile(const char *filename);
FuncReturningValue* solve(FuncArgument* fa);
FuncReturningValue* get_header(FuncArgument* fa);
void clean(FuncArgument* args);
char** read_csv(const char* filename, size_t *lines);
void clean2DArray(char**, size_t );
void clean3DArray(char***, size_t, size_t);
char** strSplit(string line,size_t*);
size_t calculating_region_lines_number(FuncArgument* fa);
char***memory_alloc_for_3DArray(FuncArgument*fa);
char**memory_alloc_for_2DArray(int n);
char**get_headers(FuncArgument* fa, size_t* num);
void calc_median_max_min(FuncReturningValue*frv, vector<double> vectorForMedian, size_t count_ocur_lines);
double first_value_minmax(FuncArgument *fa);
void functionForGetData(FuncReturningValue*frv,char***data,char**rowData,size_t lines,size_t fields);
void functionForCalcatSolve(FuncReturningValue*frv,FuncArgument*fa,char***data_for_chosen_region);

FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa)
{
    FuncReturningValue* result;
    switch(ft)
    {
        case getData:
            result = getDataFromFile(fa->filename);
            break;
        case getHeaders:
            result = get_header(fa);
        break;
        case calculateData:
            result = solve(fa);
            break;
        case cleanData:
            clean(fa);
        default:
            result = NULL;
    }
    return result;
}

FuncReturningValue* getDataFromFile(const char* filename){
    size_t lines, fields=0;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL){
        char **rowData = read_csv(filename, &lines);

        if(rowData==NULL){
            free(frv);
            frv=NULL;
        }
        else{
            char ***data = (char***)malloc((lines) * sizeof(char**));
            if (data==NULL){
                free(frv);
                clean2DArray(rowData,lines);
            }
            else
                functionForGetData(frv, data, rowData, lines, fields);
        }
    }
    return frv;
}

FuncReturningValue* get_header(FuncArgument* fa){
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL){
        size_t num=0;
        frv->headers = get_headers(fa, &num);
        frv->col_num=num;
    }
    return frv;
}

char**get_headers(FuncArgument* fa, size_t* num){
    size_t counter=1,llen;
    char **headers=(char **)calloc(counter, sizeof(char *));
    string line,str;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        getline(fp,line);
        // ss is an object of stringstream that references the S string.
        stringstream ss(line);
        while (getline(ss, str, ',')){
            headers = (char **)realloc(headers,counter * sizeof(char *));
                if (headers == NULL)
                    clean2DArray(headers,counter-1);
            llen = str.length();
            *(headers+counter-1) = (char *)calloc(sizeof(char), llen+1);
            strcpy(*(headers+counter-1), str.c_str());
            counter++;
        }
        fp.close();
        *num=counter;
    }
    return headers;
}

void functionForGetData(FuncReturningValue*frv, char***data, char**rowData, size_t lines, size_t fields){
    string str(*rowData);
    if (count(str.begin(), str.end(), ',')==0)
        frv->error=SPLIT_ERROR;
    else
    {
        char **headers = memory_alloc_for_2DArray(fields);
        if (headers==NULL){
            free(frv);
            clean2DArray(rowData,lines);
            clean3DArray(data,lines,fields);
        }
        else {
            headers=strSplit(*rowData, &fields);
            for (size_t i = 0; i < lines - 1; i++){
                *(data+i) = strSplit(*(rowData+i+1),&fields);
                if (*(data + i) == NULL){
                    clean3DArray(data,i-1,fields);
                    break;
                }
            }

            lines--;
            clean2DArray(rowData, lines);
            frv->row_num= lines;
            frv->col_num = fields;
            frv->headers = headers;
            frv->data = data;
        }
    }
}

char** read_csv(const char* filename, size_t *lines){
    string line;
    size_t llen, counter = 1;
    ifstream fp(filename);
    char **data = (char **)calloc(1, sizeof(char *));
    if(fp.is_open()){
        if (data != NULL) {
            while (getline(fp,line)){
                data = (char **)realloc(data,counter * sizeof(char *));
                if (data == NULL)
                    clean2DArray(data,counter-1);
                if(line.find('\r') != std::string::npos)
                    line[line.find('\r')] = '\0';
                llen = line.length();
                *(data+counter-1) = (char *)calloc(sizeof(char), llen+1);
                strcpy(*(data+counter-1), line.c_str());
                counter++;
            }
        }
        fp.close();
        *lines = counter-1;
    }
    return data;
}

FuncReturningValue* solve(FuncArgument* fa){
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL){
        char ***data_for_chosen_region = memory_alloc_for_3DArray(fa);
        if (data_for_chosen_region == NULL){
            free(frv);
            frv=NULL;
        }
        else{
            functionForCalcatSolve(frv, fa, data_for_chosen_region);
        }
    }
    return frv;
}

size_t calculating_region_lines_number(FuncArgument* fa)
{
    size_t lines=0,num_cols=0;
    string line;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        char**array=memory_alloc_for_2DArray(1);
        while (getline(fp,line)){
            array = strSplit(line, &num_cols);
            if (strcmp(array[1],fa->region)==0){
                lines++;
            }
        }
        clean2DArray(array, num_cols);
        fp.close();
    }
    return lines;
}

double first_value_minmax(FuncArgument *fa)
{
    double elem=0;
    for (size_t i=0;i<fa->row_num;i++)
        if (strcmp(fa->data[i][fa->region_index_at_header],fa->region)==0)
            if ((strcmp(fa->data[i][fa->column],"")!=0)&&(!isalpha(*(fa->data[i][fa->column])))){
                elem = atof(fa->data[i][fa->column]);
                break;
            }

    return elem;
}

char** get_array(string line, size_t num_cols){
    if(line.find('\r') != std::string::npos)
        line[line.find('\r')] = '\0';
    return strSplit(line, &num_cols);
}

void functionForCalcatSolve(FuncReturningValue*frv, FuncArgument*fa, char***data_for_chosen_region){
    size_t count_lines=0, count_ocur_lines=0, num_cols=0;
    string line;
    vector<double> vectorForMedian;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        frv->headers=get_headers(fa, &num_cols);
        frv->col_num = fa->col_num;
        char**array = memory_alloc_for_2DArray(1);
        if(array != NULL)
            while (getline(fp,line)){
                array = get_array(line, num_cols);
                if (array != NULL && num_cols > fa->region_index_at_header){
                    char * arr_reg = array[fa->region_index_at_header];
                    if (strcmp(arr_reg,fa->region) == 0){
                        *(data_for_chosen_region + count_lines) = array;
                        count_lines++;
                        if ((strcmp(array[fa->column],"")!=0)&&(!isalpha(*(array[fa->column])))){
                            count_ocur_lines++;
                            vectorForMedian.push_back(atof(array[fa->column]));
                        }
                    }
                    frv->row_num = count_lines;
                    frv->data=data_for_chosen_region;
                    calc_median_max_min(frv, vectorForMedian, count_ocur_lines);
                }
            }
        fp.close();
    }else
        frv->error = CALCULATE_ERROR;
}

void calc_median_max_min(FuncReturningValue*frv, vector<double> vectorForMedian, size_t count_ocur_lines)
{
    if (count_ocur_lines!=0){
        double median;
        int count_lines = vectorForMedian.size();
        sort(vectorForMedian.begin(), vectorForMedian.end());
        frv->solution_max = vectorForMedian[count_lines-1];
        frv->solution_min = vectorForMedian[0];
        if((count_lines % 2) == 1)
            median = vectorForMedian[count_lines/2];
        else
            median = (vectorForMedian[count_lines/2-1] + vectorForMedian[count_lines/2]) / 2;
        frv->solution_median = median;
        frv->error = 0;
    }else
        frv->error = CALCULATE_ERROR;
}

void clean(FuncArgument* args)
{
    if (args->data != NULL)
        clean3DArray(args->data, args->row_num, args->col_num);
    if (args->filename != NULL)
        free(args->filename);
    if (args->headers != NULL)
        clean2DArray(args->headers, args->col_num);
    if (args->region != NULL)
        free(args->region);
}

void clean2DArray(char **arr, size_t size)
{
    for (size_t i = 0;i < size; i++){
        free(*(arr+i));
    }
    free(arr);
}

void clean3DArray(char ***arr, size_t sizeX, size_t sizeY)
{
    for (size_t i = 0; i < sizeX; i++)
        clean2DArray(*(arr+i), sizeY);
    free(arr);
}

char***memory_alloc_for_3DArray(FuncArgument*fa)
{
    size_t rows = calculating_region_lines_number(fa);
    char ***data = (char ***)malloc(rows*sizeof(char**));
    if (data!=NULL){
        for (size_t i = 0; i < rows; i++){
            *(data+i) = (char **)malloc(sizeof(char*) * fa->col_num);
            if (*(data+i)==NULL){
                clean2DArray(*data,i-1);
                data=NULL;
                break;
            }
            for (size_t j = 0; j < fa->col_num; j++){
                *(*(data+i)+j)=(char *)malloc(sizeof(char) *STRLEN);
                if (*(*(data+i)+j)==NULL){
                    clean3DArray(data,j-1,i);
                    data=NULL;
                    break;
                }
            }

        }

    }
    return data;
}

char**memory_alloc_for_2DArray(int n)
{
    char **data = (char **)malloc(n*sizeof(char*));
    if (data!=NULL)
    {
        for (int i = 0; i < n; i++)
        {
            *(data+i) = (char *)malloc(sizeof(char) * STRLEN);
            if (*(data+i)==NULL){
                clean2DArray(data,i-1);
                data=NULL;
                break;
            }
        }
    }
    return data;
}

char **strSplit(string line, size_t*counter)
{
    size_t num=0;
    string str,tmp=line;
    stringstream ss(line);
    //+1 потому что слов на 1 больше, чем разделителей
    char**array=(char **)calloc(count(line.begin(), line.end(), ',')+1, sizeof(char *));
    if (array!=NULL){
        while (getline(ss, str, ',')){
            *(array+num) = (char *)malloc(sizeof(char) * str.length()+1);
            if (*(array+num)!=NULL){
                strcpy(*(array+num),str.c_str());
                num++;
            }else{
                clean2DArray(array,num-1);
                num=0;
                break;
            }
        }
    }
    *counter=num;
    return array;
}
