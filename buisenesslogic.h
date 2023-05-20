#include <iostream>
#define STRLEN 50
#define MEMORY_ERROR 1
#define FILE_OPEN_ERROR 1
#define SPLIT_ERROR 3
#define CALCULATE_ERROR 4
using namespace std;

enum FuncType
{
    getData,
    getHeaders,
    calculateData,
    cleanData,
};

typedef struct
{
    char ***data;                                     // данные (функция getData())
    char **headers;                                   // заголовки таблицы (функция getData())
    double solution_min;                              // результаты вычислений (функция calculateData())
    double solution_max;
    double solution_median;
    size_t row_num;                                   // число строк в таблице
    size_t col_num;                                   // число столбцов в таблице
    size_t error;                                     //1-ошибка выделения памяти,2-невозможно откыть файл,
                                                      //3-неверные разделители,4-calc
} FuncReturningValue;

typedef struct
{
    char* filename;                                    // имя файла (функция getData())
    char ***data;                                      // данные (функция calculateData())
    char **headers;                                    // заголовки таблицы (функция getData())
    char *region;                                      //регион по которому вычисляются метрики
    size_t column;                                     //колонка по которой вычисляются метрики
    size_t row_num;                                    // число строк в таблице
    size_t col_num;                                    // число столбцов в таблице
    size_t region_index_at_header;                     //индекс региона в списке

} FuncArgument;


FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa);


