/*Реализовать структуру данных вида хеш-таблица (hashtable) с методом цепочек разрешения
коллизий. Для структуры определить и реализовать следующий функционал:
    ● создание пустой хеш-таблицы с настройкой хеш-функции;
    ● добавление значения в хеш-таблицу;
    ● поиск значения в хеш-таблице;
    ● удаление значения из хеш-таблицы.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Узел односвязного списка для цепочки
typedef struct Node
{
    char* key;
    int value;
    struct Node* next;
} Node;

// Тип указателя на функцию хеширования
typedef size_t (*HashFunction)(const char* key, size_t capacity);

// Структура хеш-таблицы
typedef struct HashTable
{
    Node** table;
    size_t capacity;
    HashFunction hash_func;
} HashTable;

// Вспомогательная функция для дублирования строк
char*
duplicate_string(const char* src)
{
    size_t len = strlen(src) + 1;
    char* dest = (char*)malloc(len);
    if (dest)
    {
        strcpy(dest, src);
    }
    return dest;
}

// Простая хеш-функции (типа DJB2)
size_t
simple_hash(const char* key, size_t capacity)
{
    size_t hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % capacity;
}

// 1. Создание пустой хеш-таблицы с настройкой хеш-функции
HashTable*
create_table(size_t capacity, HashFunction hash_func)
{
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht)
    {
        return NULL;
    }

    ht->capacity = capacity;
    ht->hash_func = hash_func;
    
    // Выделяем память под массив указателей на узлы и инициализируем их NULL
    ht->table = (Node**)calloc(capacity, sizeof(Node*));
    if (!ht->table)
    {
        free(ht);
        return NULL;
    }

    return ht;
}

// 2. Добавление значения в хеш-таблицу
void
insert(HashTable* ht, const char* key, int value)
{
    if (!ht || !key)
    {
        return;
    }

    size_t index = ht->hash_func(key, ht->capacity);
    Node* current = ht->table[index];

    // Проверяем, существует ли уже такой ключ в цепочке. 
    // Если да — обновляем значение.
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Если ключ не найден, создаем новый узел
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node)
    {
        return;
    }

    new_node->key = duplicate_string(key);
    new_node->value = value;
    
    // Вставляем новый узел в начало цепочки (O(1) для вставки)
    new_node->next = ht->table[index];
    ht->table[index] = new_node;
}

// 3. Поиск значения в хеш-таблице
// Возвращает 1, если найдено (результат пишется в out_value), и 0, если нет
int
search(HashTable* ht, const char* key, int* out_value)
{
    if (!ht || !key)
    {
        return 0;
    }

    size_t index = ht->hash_func(key, ht->capacity);
    Node* current = ht->table[index];

    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (out_value)
            {
                *out_value = current->value;
            }
            return 1;
        }
        current = current->next;
    }

    return 0;
}

// 4. Удаление значения из хеш-таблицы
void
delete_key(HashTable* ht, const char* key)
{
    if (!ht || !key)
    {
        return;
    }

    size_t index = ht->hash_func(key, ht->capacity);
    Node* current = ht->table[index];
    Node* prev = NULL;

    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            // Если удаляем не первый элемент в цепочке
            if (prev)
            {
                prev->next = current->next;
            }
            // Если удаляем первый элемент в цепочке
            else
            {
                ht->table[index] = current->next;
            }

            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Вспомогательная функция для очистки памяти
void
free_table(HashTable* ht)
{
    if (!ht)
    {
        return;
    }

    for (size_t i = 0; i < ht->capacity; i++)
    {
        Node* current = ht->table[i];
        while (current)
        {
            Node* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }

    free(ht->table);
    free(ht);
}


int
main()
{
    // Создаем таблицу размером 10 и передаем нашу хеш-функцию
    HashTable* ht = create_table(10, simple_hash);

    printf("Добавление значений...\n");
    insert(ht, "пухосос", 100);
    insert(ht, "пример", 200);
    insert(ht, "тест", 300);
    printf("Добавлены ключи: 'пухосос' (100), 'пример' (200), 'тест' (300).\n\n");
    
    int val;
    printf("Поиск значений:\n");
    if (search(ht, "пухосос", &val))
    {
        printf("Найден ключ 'пухосос', значение: %d\n\n", val);
    }

    printf("Обновление значения для 'пухосос' до 150...\n");
    insert(ht, "пухосос", 150); // Обновление существующего ключа

    printf("Поиск 'пухосос' после обновления...\n");
    if (search(ht, "пухосос", &val))
    {
        printf("После обновления ключ 'пухосос' имеет значение: %d\n\n", val);
    }
    printf("Поиск значения для 'нету'...\n");
    if (!search(ht, "нету", &val))
    {
        printf("Ключ 'нету' не найден.\n");
    }

    printf("\nУдаление 'пример'...\n");
    delete_key(ht, "пример");

    if (!search(ht, "пример", &val))
    {
        printf("Ключ 'пример' успешно удален.\n");
    }

    free_table(ht);
    return 0;
}