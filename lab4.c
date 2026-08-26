/*Реализовать структуру данных вида префиксное дерево (trie). Для структуры определить и
реализовать следующий функционал:
    ● создание пустого trie с настройкой алфавита;
    ● добавление значения по строковому ключу в trie;
    ● поиск значения по строковому ключу в trie;
    ● удаление значения по строковому ключу в trie.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Узел префиксного дерева
typedef struct TrieNode
{
    struct TrieNode** children; // Массив указателей на потомков
    int value;                  // Значение, привязанное к ключу
    bool is_terminal;           // Флаг окончания слова
} TrieNode;

// Само дерево с настройками алфавита
typedef struct Trie
{
    TrieNode* root;
    int alphabet_size;
    char base_char;
} Trie;

// Создание нового узла
TrieNode*
create_node(int alphabet_size)
{
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node)
    {
        node->is_terminal = false;
        node->value = 0;
        node->children = (TrieNode**)calloc(alphabet_size, sizeof(TrieNode*));
    }
    return node;
}

// Проверка, является ли узел пустым (нет детей)
bool
is_empty_node(TrieNode* node, int alphabet_size)
{
    for (int i = 0; i < alphabet_size; i++)
    {
        if (node->children[i] != NULL)
        {
            return false;
        }
    }
    return true;
}


// 1. Создание пустого trie с настройкой алфавита
Trie*
create_trie(int alphabet_size, char base_char)
{
    Trie* trie = (Trie*)malloc(sizeof(Trie));
    if (trie)
    {
        trie->alphabet_size = alphabet_size;
        trie->base_char = base_char;
        trie->root = create_node(alphabet_size);
    }
    return trie;
}

// 2. Добавление значения по строковому ключу
void
insert(Trie* trie, const char* key, int value)
{
    if (!trie || !trie->root || !key)
    {
        return;
    }

    TrieNode* current = trie->root;
    while (*key)
    {
        int index = *key - trie->base_char;
        
        // Проверка выхода за пределы настроенного алфавита
        if (index < 0 || index >= trie->alphabet_size)
        {
            fprintf(stderr, "Ошибка: символ вне заданного алфавита.\n");
            return;
        }

        if (current->children[index] == NULL)
        {
            current->children[index] = create_node(trie->alphabet_size);
        }
        current = current->children[index];
        key++;
    }

    current->is_terminal = true;
    current->value = value;
}

// 3. Поиск значения по строковому ключу
bool
search(Trie* trie, const char* key, int* out_value)
{
    if (!trie || !trie->root || !key)
    {
        return false;
    }

    TrieNode* current = trie->root;
    while (*key)
    {
        int index = *key - trie->base_char;
        
        if (index < 0 || index >= trie->alphabet_size)
        {
            return false;
        }

        if (current->children[index] == NULL)
        {
            return false; // Путь обрывается, ключа нет
        }
        current = current->children[index];
        key++;
    }

    if (current != NULL && current->is_terminal)
    {
        if (out_value)
        {
            *out_value = current->value; // Записываем найденное значение
        }
        return true;
    }

    return false;
}

// Вспомогательная рекурсивная функция для удаления
TrieNode*
delete_helper(TrieNode* node, const char* key, int depth, int alphabet_size, char base_char)
{
    if (!node)
    {
        return NULL;
    }

    // Если дошли до конца ключа
    if (key[depth] == '\0')
    {
        if (node->is_terminal)
        {
            node->is_terminal = false; // Снимаем флаг конца слова
        }

        // Если у узла нет потомков, его можно безопасно удалить
        if (is_empty_node(node, alphabet_size))
        {
            free(node->children);
            free(node);
            node = NULL;
        }

        return node;
    }

    int index = key[depth] - base_char;
    if (index >= 0 && index < alphabet_size)
    {
        node->children[index] = delete_helper(node->children[index], key, depth + 1, alphabet_size, base_char);
    }

    // Пост-обработка при возврате из рекурсии:
    // если узел больше не терминальный и у него не осталось детей — удаляем его
    if (!node->is_terminal && is_empty_node(node, alphabet_size))
    {
        free(node->children);
        free(node);
        node = NULL;
    }

    return node;
}

// 4. Удаление значения по строковому ключу
void
delete_key(Trie* trie, const char* key)
{
    if (!trie || !trie->root || !key)
    {
        return;
    }

    trie->root = delete_helper(trie->root, key, 0, trie->alphabet_size, trie->base_char);
    
    // Если корень удалился (дерево стало полностью пустым), создаем его заново
    if (trie->root == NULL)
    {
        trie->root = create_node(trie->alphabet_size);
    }
}


int
main()
{
    // Настраиваем алфавит: 26 символов, начиная с 'a' (английский нижний регистр)
    Trie* trie = create_trie(26, 'a');

    printf("Вставка элементов:\n");
    insert(trie, "", 100); // пустое слово
    insert(trie, "test", 50);
    insert(trie, "testing", 200);
    printf("Вставлены ключи: '' (100), 'test' (50), 'testing' (200).\n\n");

    printf("Поиск элементов:\n");
    int val;
    if (search(trie, "", &val))
    {
        printf("Найден ключ '', значение: %d\n", val);
    }

    if (search(trie, "test", &val))
    {
        printf("Найден ключ 'test', значение: %d\n", val);
    }

    if (search(trie, "testing", &val))
    {
        printf("Найден ключ 'testing', значение: %d\n", val);
    }

    if (!search(trie, "testin", &val))
    {
        printf("Ключ 'testin' не найден.\n\n");
    }

    printf("Удаление элементов:\n");
    delete_key(trie, "test");
    printf("Ключ 'test' удален.\n");

    if (!search(trie, "test", &val))
    {
        printf("Поиск 'test' после удаления не дал результата.\n");
    }
    
    if (search(trie, "testing", &val))
    {
        printf("При этом 'testing' все еще существует, значение: %d\n", val);
    }

    return 0;
}