#include <iostream>
#define STRLEN 50
#define MEMORY_ERROR 1
#define FILE_OPEN_ERROR 1
#define SPLIT_ERROR 3
#define CALCULATE_ERROR 4
using namespace std;

enum FuncType
{
    getData, // функция чтения данных из csv файла
    calculateData, // функция для вычислений
    cleanData, // освобождение памяти
};

typedef struct
{
    char ***data;                                     // данные (функция getData())
    char **headers;                                   // заголовки таблицы (функция getData())
    double solution_min;                              // результаты вычислений (функция calculateData())
    double solution_max;
    double solution_median;
    size_t len;                                       // число строк в таблице
    size_t fields_num;                                // число столбцов в таблице
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
    size_t len;                                        // число строк в таблице
    size_t fields_num;                                 // число столбцов в таблице
    size_t region_number;                              //индекс региона в списке
    size_t region_index_at_header;
} FuncArgument;


FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa);


