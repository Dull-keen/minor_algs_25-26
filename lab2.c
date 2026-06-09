/*Реализовать структуру данных вида B-дерево. Для структуры описать тип узла дерева и самого
дерева, а также определить и реализовать следующий функционал:
    ● создание пустого B-дерева;
    ● удаление B-дерева;
    ● добавления значения по ключу в B-дерево;
    ● удаления значения по ключу из B-дерева;
    ● поиск значения по ключу в B-дереве;
    ● обход B-дерева (инфиксный).
Типы ключа и значения в рамках дерева произвольны. Отношение порядка на множестве ключей
должно быть вариативным.*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Сигнатуры функций
typedef int (*CompareFunc)(const void* a, const void* b);
typedef void (*PrintFunc)(const void* key, const void* value);

// Структура узла B-дерева
typedef struct BTreeNode
{
    void **keys;               
    void **values;             
    struct BTreeNode **children; 
    int num_keys;              
    bool is_leaf;              
} BTreeNode;

// Структура B-дерева
typedef struct BTree
{
    BTreeNode *root;           
    int t;                     
    CompareFunc cmp;           
} BTree;


BTreeNode*
createNode(int t, bool is_leaf)
{
    BTreeNode *newNode = (BTreeNode*)malloc(sizeof(BTreeNode));
    newNode->keys = (void**)malloc((2 * t - 1) * sizeof(void*));
    newNode->values = (void**)malloc((2 * t - 1) * sizeof(void*));
    newNode->children = (BTreeNode**)malloc((2 * t) * sizeof(BTreeNode*));
    newNode->num_keys = 0;
    newNode->is_leaf = is_leaf;
    return newNode;
}

// ● Создание пустого B-дерева
BTree*
createBTree(int t, CompareFunc cmp)
{
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    tree->t = t;
    tree->cmp = cmp;
    tree->root = createNode(t, true);
    return tree;
}

// ● Удаление узла B-дерева (рекурсивно)
void
destroyNode(BTreeNode *node)
{
    if (node != NULL)
    {
        if (!node->is_leaf)
        {
            for (int i = 0; i <= node->num_keys; i++)
            {
                destroyNode(node->children[i]);
            }
        }
        free(node->keys);
        free(node->values);
        free(node->children);
        free(node);
    }
}

// ● Удаление B-дерева
void
destroyBTree(BTree *tree)
{
    if (tree != NULL)
    {
        destroyNode(tree->root);
        free(tree);
    }
}

// ● Поиск значения по ключу в B-дереве
void*
searchBTree(BTree *tree, BTreeNode *node, void *key)
{
    int i = 0;
    
    while (i < node->num_keys && tree->cmp(key, node->keys[i]) > 0)
    {
        i++;
    }
    
    if (i < node->num_keys && tree->cmp(key, node->keys[i]) == 0)
    {
        return node->values[i];
    }
    
    if (node->is_leaf)
    {
        return NULL;
    }
    
    return searchBTree(tree, node->children[i], key);
}


// Обновленная функция обхода с детальным выводом
void
traverseNode(BTreeNode *node, PrintFunc printFunc, int level, bool is_root)
{
    int i;
    
    // Определяем тип узла для вывода
    const char *node_type;
    if (is_root)
    {
        node_type = "Корень";
    }
    else if (node->is_leaf)
    {
        node_type = "Лист  ";
    }
    else
    {
        node_type = "Внутр.";
    }

    for (i = 0; i < node->num_keys; i++)
    {
        if (!node->is_leaf)
        {
            traverseNode(node->children[i], printFunc, level + 1, false);
        }
        
        // Выводим метаданные перед самим значением
        // %p выведет адрес узла в памяти, что послужит уникальным ID
        printf("[Ур:%d | %s | ID:%p] ", level, node_type, (void*)node);
        printFunc(node->keys[i], node->values[i]);
        printf("\n");
    }
    
    if (!node->is_leaf)
    {
        traverseNode(node->children[i], printFunc, level + 1, false);
    }
}

// ● Обход B-дерева (инфиксный)
void
traverseBTree(BTree *tree, PrintFunc printFunc)
{
    if (tree->root != NULL)
    {
        traverseNode(tree->root, printFunc, 0, true);
    }
}


void
splitChild(BTree *tree, BTreeNode *parent, int i, BTreeNode *full_child)
{
    int t = tree->t;
    BTreeNode *new_child = createNode(t, full_child->is_leaf);
    new_child->num_keys = t - 1;

    for (int j = 0; j < t - 1; j++)
    {
        new_child->keys[j] = full_child->keys[j + t];
        new_child->values[j] = full_child->values[j + t];
    }

    if (!full_child->is_leaf)
    {
        for (int j = 0; j < t; j++)
        {
            new_child->children[j] = full_child->children[j + t];
        }
    }
    full_child->num_keys = t - 1;

    for (int j = parent->num_keys; j >= i + 1; j--)
    {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = new_child;

    for (int j = parent->num_keys - 1; j >= i; j--)
    {
        parent->keys[j + 1] = parent->keys[j];
        parent->values[j + 1] = parent->values[j];
    }

    parent->keys[i] = full_child->keys[t - 1];
    parent->values[i] = full_child->values[t - 1];
    parent->num_keys++;
}

// ● Добавления значения по ключу в B-дерево
void
insertNonFull(BTree *tree, BTreeNode *node, void *key, void *value)
{
    int i = node->num_keys - 1;

    if (node->is_leaf)
    {
        while (i >= 0 && tree->cmp(key, node->keys[i]) < 0)
        {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && tree->cmp(key, node->keys[i]) < 0)
        {
            i--;
        }
        i++;
        
        if (node->children[i]->num_keys == 2 * tree->t - 1)
        {
            splitChild(tree, node, i, node->children[i]);
            if (tree->cmp(key, node->keys[i]) > 0)
            {
                i++;
            }
        }
        insertNonFull(tree, node->children[i], key, value);
    }
}


void
insertBTree(BTree *tree, void *key, void *value)
{
    BTreeNode *root = tree->root;
    
    if (root->num_keys == 2 * tree->t - 1)
    {
        BTreeNode *new_root = createNode(tree->t, false);
        tree->root = new_root;
        new_root->children[0] = root;
        splitChild(tree, new_root, 0, root);
        insertNonFull(tree, new_root, key, value);
    }
    else
    {
        insertNonFull(tree, root, key, value);
    }
}


int
int_cmp(const void *a, const void *b)
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}


void
print_int(const void *key, const void *value)
{
    printf("Ключ: %3d | Значение: %d", *(const int*)key, *(const int*)value);
}


int
main()
{
    // t=3 означает, что узлы могут хранить от 2 до 5 ключей
    BTree *tree = createBTree(3, int_cmp);

    // Расширенный набор из 20 элементов для наглядного ветвления
    int keys[20] = {10, 20, 5, 6, 12, 30, 7, 17, 1, 3, 15, 25, 40, 50, 8, 14, 18, 22, 27, 35};
    int values[20];
    
    // Заполним значения просто умножением на 10 для удобства
    for (int i = 0; i < 20; i++)
    {
        values[i] = keys[i] * 10;
        insertBTree(tree, &keys[i], &values[i]);
    }

    printf("Инфиксный обход B-дерева:\n");
    traverseBTree(tree, print_int);
    printf("\n");

    printf("Проверка поиска:\n");
    int search_key = 14;
    void *res = searchBTree(tree, tree->root, &search_key);
    
    if (res)
    {
        printf("Поиск ключа %d: Найдено значение %d\n\n", search_key, *(int*)res);
    }
    else
    {
        printf("Поиск ключа %d: Ключ не найден.\n\n", search_key);
    }

    insertBTree(tree, &(int){23}, &(int){230}); // Добавляем новый ключ для проверки динамического роста

    printf("Повторный инфиксный обход B-дерева:\n");
    traverseBTree(tree, print_int);
    printf("\n");

    destroyBTree(tree);
    return 0;
}