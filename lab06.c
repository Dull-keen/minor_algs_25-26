/*Реализовать структуру данных вида набросок count-min (count-min sketch). Для структуры
определить и реализовать следующий функционал:
    ● создание пустого наброска с конфигурацией используемых хеш-функций, а также размера
    состояния строк матрицы наброска (в битах);
    ● добавление значения в набросок в заданном количестве экземпляров;
    ● поиск информации о количестве вхождений значения в набросок.  
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Определение типа указателя на хеш-функцию
typedef uint32_t (*HashFunc)(const char* key);

// Структура наброска Count-Min
struct CountMinSketch
{
    size_t depth;              // Количество хеш-функций (строки)
    size_t width;              // Количество счетчиков (столбцы)
    uint32_t** table;          // Двумерная матрица счетчиков
    HashFunc* hash_functions;  // Массив указателей на хеш-функции
};

typedef struct CountMinSketch CountMinSketch;

// 1. Создание пустого наброска.
// Принимает массив хеш-функций, их количество и размер строки в битах.
CountMinSketch*
cms_create(HashFunc* hash_funcs, size_t num_hashes, size_t row_size_bits)
{
    CountMinSketch* cms = (CountMinSketch*)malloc(sizeof(CountMinSketch));
    if (!cms)
    {
        return NULL;
    }

    cms->depth = num_hashes;
    
    // Один счетчик uint32_t занимает 32 бита. Вычисляем количество столбцов (width)
    cms->width = row_size_bits / 32;
    if (cms->width == 0)
    {
        cms->width = 1; // Защита от некорректного (слишком маленького) размера
    }

    cms->hash_functions = hash_funcs;

    // Выделение памяти под матрицу
    cms->table = (uint32_t**)malloc(cms->depth * sizeof(uint32_t*));
    for (size_t i = 0; i < cms->depth; i++)
    {
        cms->table[i] = (uint32_t*)calloc(cms->width, sizeof(uint32_t));
    }

    return cms;
}

// 2. Добавление значения в набросок в заданном количестве экземпляров.
void
cms_add(CountMinSketch* cms, const char* key, uint32_t count)
{
    for (size_t i = 0; i < cms->depth; i++)
    {
        uint32_t hash_val = cms->hash_functions[i](key);
        size_t col = hash_val % cms->width;
        
        // Увеличиваем счетчик в соответствующей ячейке
        cms->table[i][col] += count;
    }
}

// 3. Поиск информации о количестве вхождений значения.
uint32_t
cms_query(CountMinSketch* cms, const char* key)
{
    uint32_t min_count = UINT32_MAX;

    for (size_t i = 0; i < cms->depth; i++)
    {
        uint32_t hash_val = cms->hash_functions[i](key);
        size_t col = hash_val % cms->width;

        // Ищем минимальное значение среди всех строк
        if (cms->table[i][col] < min_count)
        {
            min_count = cms->table[i][col];
        }
    }

    return min_count;
}

// Очистка памяти
void
cms_destroy(CountMinSketch* cms)
{
    if (cms)
    {
        for (size_t i = 0; i < cms->depth; i++)
        {
            free(cms->table[i]);
        }
        free(cms->table);
        free(cms);
    }
}


uint32_t
hash_djb2(const char* key)
{
    uint32_t hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

uint32_t
hash_fnv1a(const char* key)
{
    uint32_t hash = 2166136261u;
    int c;
    while ((c = *key++))
    {
        hash ^= (uint32_t)c;
        hash *= 16777619u;
    }
    return hash;
}

uint32_t
hash_sdbm(const char* key)
{
    uint32_t hash = 0;
    int c;
    while ((c = *key++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}


int
main()
{
    // 1. Конфигурация используемых хеш-функций
    HashFunc funcs[] = {hash_djb2, hash_fnv1a, hash_sdbm};
    size_t num_hashes = 3;

    // 2. Задаем размер состояния строки в битах.
    // Например, 1024 бита на строку означает, что у нас будет (1024 / 32) = 32 счетчика.
    size_t row_size_bits = 1024;

    CountMinSketch* cms = cms_create(funcs, num_hashes, row_size_bits);

    // Добавление элементов
    printf("Добавление элементов 'test' (5), 'check' (2), 'testing' (7)...\n");
    cms_add(cms, "test", 5);
    cms_add(cms, "check", 2);
    cms_add(cms, "testing", 7);

    // Добавляем еще 3 test
    printf("Добавление 'test' еще 3 раза...\n");
    cms_add(cms, "test", 3); 

    // Проверка вхождений
    printf("Count 'test': %u\n", cms_query(cms, "test"));   // Ожидаем 8 (5 + 3)
    printf("Count 'check': %u\n", cms_query(cms, "check")); // Ожидаем 2
    printf("Count 'testing': %u\n", cms_query(cms, "testing")); // Ожидаем 7    

    cms_destroy(cms);
    return 0;
}