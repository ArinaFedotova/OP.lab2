#include "buisenesslogic.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string.h>

FuncReturningValue* getDataFromFile(const char *filename);
FuncReturningValue* solve(FuncArgument* fa);
void clean(FuncArgument* args);
char** read_csv(const char* filename, size_t *lines);
void clean2DArray(char**, size_t );
void clean3DArray(char***, size_t, size_t);
char** strSplit(string line,size_t*);
size_t calculating_region_lines_number(FuncArgument* fa);
char***memory_alloc_for_3DArray(FuncArgument*fa);
char**memory_alloc_for_2DArray(int n);
char**get_headers(FuncArgument* fa);
double calc_median(vector<double> vectorForMedian,int count_lines);
double first_value_minmax(FuncArgument *fa);
void functionForGetData(FuncReturningValue*frv,char***data,char**rawData,size_t lines,size_t fields);
void functionForCalcatSolve(FuncReturningValue*frv,FuncArgument*fa,char***data_for_chosen_region);

FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa)
{
    FuncReturningValue* result;
    switch(ft)
    {
        case getData:
            result = getDataFromFile(fa->filename);
            break;
        case calculateData:
            result = solve(fa);
            break;
        case cleanData:
            clean(fa);
        default:
            result = NULL;
            break;
    }
    return result;
}

FuncReturningValue* getDataFromFile(const char* filename){
    size_t lines,fields=0;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL){
        char **rawData = read_csv(filename, &lines);

        if(rawData==NULL){
            free(frv);
            frv=NULL;
        }
        else{
            char ***data = (char***)malloc((lines) * sizeof(char**));
            if (data==NULL){
                free(frv);
                clean2DArray(rawData,lines);
            }
            else
                functionForGetData(frv,data,rawData,lines,fields);
        }
    }
    return frv;
}

void functionForGetData(FuncReturningValue*frv, char***data, char**rawData, size_t lines, size_t fields){
    string str(*rawData);
    if (count(str.begin(), str.end(), ',')==0)
        frv->error=SPLIT_ERROR;
    else
    {
        char **headers = memory_alloc_for_2DArray(fields);
        if (headers==NULL){
            free(frv);
            clean2DArray(rawData,lines);
            clean3DArray(data,lines,fields);
        }
        else {
            headers=strSplit(*rawData, &fields);
            for (size_t i = 0; i < lines - 1; i++){
                *(data+i) = strSplit(*(rawData+i+1),&fields);
            }

            lines--;
            clean2DArray(rawData, lines);
            frv->len = lines;
            frv->fields_num = fields;
            frv->headers = headers;
            frv->data = data;
        }
    }
}

FuncReturningValue* solve(FuncArgument* fa){
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL){
        char ***data_for_chosen_region = memory_alloc_for_3DArray(fa);
        if (data_for_chosen_region == NULL){
            free(frv);
            frv=NULL;
        }
        else
            functionForCalcatSolve(frv, fa, data_for_chosen_region);
    }
    return frv;
}

void functionForCalcatSolve(FuncReturningValue*frv, FuncArgument*fa, char***data_for_chosen_region){
    size_t count_lines=0, count_ocur_lines=0, num_cols=0;
    double min = first_value_minmax(fa), max = min, current;
    string line;
    vector<double> vectorForMedian;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        char**array = memory_alloc_for_2DArray(1);
        if(array != NULL){
            while (getline(fp,line)){
                if(line.find('\r') != std::string::npos)
                    line[line.find('\r')] = '\0';
                array = strSplit(line,&num_cols);
                char * arr_reg = array[fa->region_index_at_header];
                if (strcmp(arr_reg,fa->region) == 0){
                    *(data_for_chosen_region + count_lines) = array;
                    count_lines++;

                    if ((strcmp(array[fa->column],"")!=0)&&(!isalpha(*(array[fa->column])))){
                        count_ocur_lines++;
                        current = atof(array[fa->column]);
                        (current<min)? min=current:((current>max)? max=current:max);
                        vectorForMedian.push_back(current);
                    }
                }
                frv->headers=get_headers(fa);
                frv->fields_num=fa->fields_num;
                frv->len=count_lines;
                frv->data=data_for_chosen_region;
                if (count_ocur_lines!=0){
                    frv->solution_min = min;
                    frv->solution_max = max;
                    frv->solution_median = calc_median(vectorForMedian,vectorForMedian.size());
                    frv->error = 0;
                }else{
                    frv->error = CALCULATE_ERROR;
                }
            }
        }
        fp.close();
    }
    else
        frv->error = CALCULATE_ERROR;
}

void clean(FuncArgument* args)
{
    if (args->data != NULL)
        clean3DArray(args->data, args->len, args->fields_num);
    if (args->filename != NULL)
        free(args->filename);
    if (args->headers != NULL)
        clean2DArray(args->headers, args->fields_num);
    if (args->region != NULL)
        free(args->region);
}

char** read_csv(const char* filename, size_t *lines){
    string line;
    size_t llen, counter = 1;
    ifstream fp(filename);
    char **data = (char **)calloc(1, sizeof(char *));
    if(fp.is_open()){
        if (data!=NULL) {
            while (getline(fp,line)){
                data = (char **)realloc(data,counter * sizeof(char *));
                if (data==NULL)
                    clean2DArray(data,counter-1);
                if(line.find('\r')!=std::string::npos)
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

void clean2DArray(char **arr, size_t size)
{
    for (size_t i = 0;i < size; i++){
        if (strstr((*arr+i),"") == NULL)
            free(*(arr+i));}
    free(arr);
}

void clean3DArray(char ***arr, size_t sizeX, size_t sizeY)
{
    for (size_t i = 0; i < sizeX; i++)
        clean2DArray(*(arr+i), sizeY);
    free(arr);
}

size_t calculating_region_lines_number(FuncArgument* fa)
{
    size_t lines=0;
    for (size_t i=0;i<fa->len;i++)
    {
        if (strstr(fa->data[i][1],fa->region)!=NULL)
            lines++;
    }
    return lines;
}

char***memory_alloc_for_3DArray(FuncArgument*fa)
{
    size_t rows=calculating_region_lines_number(fa);
    char ***data = (char ***)malloc((rows + 1) * sizeof(char**));
    if (data!=NULL){
        for (size_t i = 0; i < rows; i++){
            *(data+i) = (char **)malloc(sizeof(char*) * fa->fields_num);
            if (*(data+i)==NULL){
                clean2DArray(*data,i-1);
                data=NULL;
                break;
            }
            if (*(data+i)!=NULL){
                for (size_t j = 0; j < fa->fields_num; j++){
                    *(*(data+i)+j)=(char *)malloc(sizeof(char) *STRLEN);
                    if (*(*(data+i)+j)==NULL){
                        clean3DArray(data,j-1,i);
                        data=NULL;
                        break;
                    }
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

char**get_headers(FuncArgument* fa)
{
    size_t counter=1,llen;
    char** headers=(char**)calloc(counter, sizeof(char *));
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
    }
    return headers;
}

char **strSplit(string line,size_t*counter){
    size_t num = 0;
    string str, tmp = line;
    stringstream ss(line);
    //+1 потому что слов на 1 больше, чем разделителей
    char**array=(char **)calloc(count(line.begin(), line.end(), ',')+1, sizeof(char *));
    while (getline(ss, str, ',')){
        *(array+num) = (char *)malloc(sizeof(char) * (str.length() + 1));
        strcpy(*(array+num),str.c_str());
        num++;
    }
    *counter=num;
    return array;
}

double first_value_minmax(FuncArgument *fa)
{
    double elem=0;
    for (size_t i=0;i<fa->len;i++)
    {
        if (strcmp(fa->data[i][fa->region_index_at_header],fa->region)==0){
            if ((strcmp(fa->data[i][fa->column],"")!=0)&&(!isalpha(*(fa->data[i][fa->column])))){
                elem = atof(fa->data[i][fa->column]);
                break;
            }
        }
    }
    return elem;
}

double calc_median(vector<double> vectorForMedian,int count_lines)
{
    double median;
    sort(vectorForMedian.begin(), vectorForMedian.end());
    if((count_lines % 2) == 1)
        median = vectorForMedian[count_lines/2];
    else
        median = (vectorForMedian[count_lines/2-1] + vectorForMedian[count_lines/2]) / 2;
    return median;
}
