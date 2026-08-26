/*Выполнение заданий возможно в одном из двух представлений: реализация на языке
программирования (предпочтительными являются C, C++, C#) требуемых алгоритмов, либо
демонстрация работы требуемых алгоритмов на примерах, расписанных от руки. Сдача заданий
предполагает собеседование, в рамках которого необходимо уметь ориентироваться в
коде/алгоритмах, понимать асимптотические сложности алгоритмов.
Реализовать структуру данных вида фильтр Блума. Для структуры определить и реализовать
следующий функционал:
    ● создание пустого фильтра Блума с конфигурацией используемых хеш-функций, а также
    размера состояния фильтра (в битах);
    ● добавление информации о наличии значения в фильтр;
    ● поиск информации о наличии значения в фильтре.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Тип указателя на хеш-функцию
typedef uint32_t (*hash_function_t)(const char* str);

// Структура фильтра Блума
typedef struct
{
    uint8_t* bit_array;         // Массив бит (через массив байтов)
    size_t bit_size;            // Размер массива в битах
    size_t num_hashes;          // Количество используемых хеш-функций
    hash_function_t* hash_funcs;// Массив указателей на хеш-функции
} bloom_filter_t;

// Вспомогательная функция для установки бита в 1
void 
set_bit(bloom_filter_t* bf, size_t bit_index)
{
    bf->bit_array[bit_index / 8] |= (1 << (bit_index % 8)); 
}

// Вспомогательная функция для проверки значения бита
bool
get_bit(bloom_filter_t* bf, size_t bit_index)
{
    return (bf->bit_array[bit_index / 8] & (1 << (bit_index % 8))) != 0;
}

// 1. Создание пустого фильтра Блума
bloom_filter_t*
bloom_filter_create(size_t bit_size, size_t num_hashes, hash_function_t* hash_funcs)
{
    bloom_filter_t* bf = (bloom_filter_t*)malloc(sizeof(bloom_filter_t));
    if (!bf)
    {
        return NULL;
    }

    bf->bit_size = bit_size;
    bf->num_hashes = num_hashes;
    
    // Переводим биты в байты с округлением вверх
    size_t byte_size = (bit_size + 7) / 8;
    bf->bit_array = (uint8_t*)calloc(byte_size, sizeof(uint8_t));
    
    // Копируем указатели на хеш-функции
    bf->hash_funcs = (hash_function_t*)malloc(num_hashes * sizeof(hash_function_t));
    for (size_t i = 0; i < num_hashes; ++i)
    {
        bf->hash_funcs[i] = hash_funcs[i];
    }
    
    return bf;
}

// 2. Добавление информации о наличии значения в фильтр
void
bloom_filter_add(bloom_filter_t* bf, const char* item)
{
    for (size_t i = 0; i < bf->num_hashes; i++)
    {
        uint32_t hash = bf->hash_funcs[i](item);
        size_t bit_index = hash % bf->bit_size;
        set_bit(bf, bit_index);
    }
}

// 3. Поиск информации о наличии значения в фильтре
bool
bloom_filter_check(bloom_filter_t* bf, const char* item)
{
    for (size_t i = 0; i < bf->num_hashes; i++)
    {
        uint32_t hash = bf->hash_funcs[i](item);
        size_t bit_index = hash % bf->bit_size;
        
        // Если хотя бы один бит равен 0, элемента точно нет в фильтре
        if (!get_bit(bf, bit_index))
        {
            return false; 
        }
    }
    // Все биты равны 1 — элемент, ВОЗМОЖНО, присутствует
    return true; 
}

// Очистка памяти
void
bloom_filter_destroy(bloom_filter_t* bf)
{
    if (bf)
    {
        free(bf->bit_array);
        free(bf->hash_funcs);
        free(bf);
    }
}

// Хеш-функции

// Хеш-функция djb2
uint32_t
hash_djb2(const char* str)
{
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// Хеш-функция sdbm
uint32_t
hash_sdbm(const char* str)
{
    uint32_t hash = 0;
    int c;
    while ((c = *str++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}


int
main()
{
    // Настраиваем хеш-функции
    hash_function_t my_hashes[] = { hash_djb2, hash_sdbm };
    size_t num_hashes = 2;
    
    // Создаем фильтр на 256 бит (32 байта)
    bloom_filter_t* bf = bloom_filter_create(256, num_hashes, my_hashes);

    // Добавляем значения
    printf("Добавляем 'пухосос' и 'тополь' в фильтр...\n");
    bloom_filter_add(bf, "пухосос");
    bloom_filter_add(bf, "тополь");

    // Проверяем наличие
    printf("Проверка:\n");
    
    const char* test_words[] = {"пухосос", "тополь", "проверка", "тест"};
    
    for (int i = 0; i < 4; i++)
    {
        bool exists = bloom_filter_check(bf, test_words[i]);
        if (exists)
        {
            printf("Слово '%s': ВОЗМОЖНО находится в множестве.\n", test_words[i]);
        }
        else
        {
            printf("Слово '%s': ТОЧНО ОТСУТСТВУЕТ в множестве.\n", test_words[i]);
        }
    }

    bloom_filter_destroy(bf);
    return 0;
}