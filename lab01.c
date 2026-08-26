/*Реализовать структуру данных вида дерево двоичного поиска (BST). Для структуры описать тип
узла дерева и самого дерева, а также определить и реализовать следующий функционал:
    ● создание пустого BST;
    ● удаление BST;
    ● добавления значения по ключу в BST;
    ● удаления значения по ключу из BST;
    ● поиск значения по ключу в BST;
    ● обход BST (префиксный, инфиксный, постфиксный).
Типы ключа и значения в рамках дерева произвольны. Отношение порядка на множестве ключей
должно быть вариативным.*/

#include <stdio.h>
#include <stdlib.h>


// Указатель на функцию сравнения двух ключей. Должна возвращать: < 0, если a < b  или  0, если a == b  или  > 0, если a > b
typedef int (*CompareFunc)(const void* a, const void* b);

// Указатель на функцию для освобождения памяти (если ключи/значения лежат в куче)
typedef void (*FreeFunc)(void* data);

// Указатель на функцию для обработки узла при обходе
typedef void (*VisitFunc)(void* key, void* value);

// Структура узла дерева
typedef struct Node 
{
    void* key;
    void* value;
    struct Node* left;
    struct Node* right;
} Node;

// Структура самого дерева
typedef struct BST 
{
    Node* root;
    CompareFunc cmp;     // Функция сравнения
    FreeFunc freeKey;    // Функция удаления ключа (может быть NULL)
    FreeFunc freeValue;  // Функция удаления значения (может быть NULL)
} BST;



// 1. Создание пустого BST
BST* 
bst_create(CompareFunc cmp, FreeFunc freeKey, FreeFunc freeValue) 
{
    BST* tree = (BST*)malloc(sizeof(BST));
    if (!tree) return NULL;
    tree->root = NULL;
    tree->cmp = cmp;
    tree->freeKey = freeKey;
    tree->freeValue = freeValue;
    return tree;
}

// Вспомогательная функция для рекурсивного удаления узлов
void 
_bst_destroy_nodes(Node* node, FreeFunc freeKey, FreeFunc freeValue) 
{
    if (node == NULL) return;
    _bst_destroy_nodes(node->left, freeKey, freeValue);
    _bst_destroy_nodes(node->right, freeKey, freeValue);
    
    if (freeKey) freeKey(node->key);
    if (freeValue) freeValue(node->value);
    free(node);
}

// 2. Удаление BST
void 
bst_destroy(BST* tree) 
{
    if (tree == NULL) return;
    _bst_destroy_nodes(tree->root, tree->freeKey, tree->freeValue);
    free(tree);
}

// Вспомогательная функция вставки
Node* 
_bst_insert(Node* node, void* key, void* value, CompareFunc cmp) 
{
    if (node == NULL) 
    {
        Node* new_node = (Node*)malloc(sizeof(Node));
        if (!new_node) return NULL;
        new_node->key = key;
        new_node->value = value;
        new_node->left = new_node->right = NULL;
        return new_node;
    }

    int res = cmp(key, node->key);
    if (res < 0) 
    {
        node->left = _bst_insert(node->left, key, value, cmp);
    } else if (res > 0) 
    {
        node->right = _bst_insert(node->right, key, value, cmp);
    } else 
    {
        // Если ключ уже существует, можно обновить значение (старое не удаляем автоматически здесь для простоты)
        node->value = value; 
    }
    return node;
}

// 3. Добавление значения по ключу
void 
bst_insert(BST* tree, void* key, void* value) 
{
    if (tree == NULL) return;
    tree->root = _bst_insert(tree->root, key, value, tree->cmp);
}

// Вспомогательная функция для поиска
Node* 
_bst_search(Node* node, void* key, CompareFunc cmp) 
{
    if (node == NULL) return NULL;
    
    int res = cmp(key, node->key);
    if (res < 0) return _bst_search(node->left, key, cmp);
    if (res > 0) return _bst_search(node->right, key, cmp);
    
    return node; // res == 0, нашли
}

// 4. Поиск значения по ключу
void* 
bst_search(BST const* tree, void* key) 
{
    if (tree == NULL) return NULL;
    Node* result = _bst_search(tree->root, key, tree->cmp);
    return result ? result->value : NULL;
}

// Вспомогательная функция: извлечение узла с минимальным ключом
Node* 
_bst_extract_min(Node* root, Node** min_node) 
{
    if (root->left == NULL)
    {
        *min_node = root;
        return root->right;
    }
    root->left = _bst_extract_min(root->left, min_node);
    return root;
}

// Вспомогательная функция удаления
Node* 
_bst_delete(Node* node, void* key, BST* tree) 
{
    if (node == NULL) return NULL;

    int res = tree->cmp(key, node->key);
    if (res < 0) 
    {
        node->left = _bst_delete(node->left, key, tree);
    } else if (res > 0) 
    {
        node->right = _bst_delete(node->right, key, tree);
    } else 
    {
        // Узел найден
        if (node->left == NULL) 
        {
            Node* temp = node->right;
            if (tree->freeKey) tree->freeKey(node->key);
            if (tree->freeValue) tree->freeValue(node->value);
            free(node);
            return temp;
        } else if (node->right == NULL) 
        {
            Node* temp = node->left;
            if (tree->freeKey) tree->freeKey(node->key);
            if (tree->freeValue) tree->freeValue(node->value);
            free(node);
            return temp;
        }
        
        // Узел с двумя потомками: находим минимум в правом поддереве
        Node* min_node;
        node->right = _bst_extract_min(node->right, &min_node);
        
        // Очищаем старые данные узла, который удаляем
        if (tree->freeKey) tree->freeKey(node->key);
        if (tree->freeValue) tree->freeValue(node->value);
        
        // Переносим данные из минимального узла
        node->key = min_node->key;
        node->value = min_node->value;
        free(min_node); // Освобождаем только структуру узла
    }
    return node;
}

// 5. Удаление значения по ключу
void 
bst_delete(BST* tree, void* key) 
{
    if (tree == NULL) return;
    tree->root = _bst_delete(tree->root, key, tree);
}

// 6. Обходы дерева (префиксный, инфиксный, постфиксный)
void 
_bst_preorder(Node* node, VisitFunc visit) 
{
    if (!node) return;
    visit(node->key, node->value);
    _bst_preorder(node->left, visit);
    _bst_preorder(node->right, visit);
}

void 
bst_preorder(BST* tree, VisitFunc visit) 
{
    if (tree) _bst_preorder(tree->root, visit);
}

void 
_bst_inorder(Node* node, VisitFunc visit) 
{
    if (!node) return;
    _bst_inorder(node->left, visit);
    visit(node->key, node->value);
    _bst_inorder(node->right, visit);
}

void 
bst_inorder(BST* tree, VisitFunc visit) 
{
    if (tree) _bst_inorder(tree->root, visit);
}

void 
_bst_postorder(Node* node, VisitFunc visit) 
{
    if (!node) return;
    _bst_postorder(node->left, visit);
    _bst_postorder(node->right, visit);
    visit(node->key, node->value);
}

void 
bst_postorder(BST* tree, VisitFunc visit) 
{
    if (tree) _bst_postorder(tree->root, visit);
}


// Функция сравнения для ключей типа int
int 
compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b); // Безопасное сравнение
}

// Функция для вывода узла (предполагаем ключ int, значение char*)
void 
print_node(void* key, void* value) 
{
    printf("[%d: %s] ", *(int*)key, (char*)value);
}

int 
main() 
{
    // Создаем дерево (память под ключи и значения выделяем на стеке/сегменте данных, поэтому free = NULL)
    BST* tree = bst_create(compare_ints, NULL, NULL);

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    char* values[] = {"Пятьдесят", "Тридцать", "Семьдесят", "Двадцать", "Сорок", "Шестьдесят", "Восемьдесят"};

    for (int i = 0; i < 7; i++) 
    {
        bst_insert(tree, &keys[i], values[i]);
    }

    printf("Инфиксный обход (Inorder): ");
    bst_inorder(tree, print_node);
    printf("\n\n");

    printf("Префиксный обход (Preorder): ");
    bst_preorder(tree, print_node);
    printf("\n\n");

    printf("Постфиксный обход (Postorder): ");
    bst_postorder(tree, print_node);
    printf("\n\n");

    int search_key = 40;
    char* found_val = (char*)bst_search(tree, &search_key);
    printf("Поиск ключа 40: %s\n\n", found_val ? found_val : "Не найден");

    printf("Удаляем узел с ключом 50 (корень)...\n\n");
    int delete_key = 50;
    bst_delete(tree, &delete_key);

    printf("Инфиксный обход после удаления: ");
    bst_inorder(tree, print_node);
    printf("\n\n");

    bst_destroy(tree);


    BST* empty_tree = bst_create(compare_ints, NULL, NULL);

    bst_inorder(empty_tree, print_node); // Должно ничего не вывести
    
    int search_key_empty = 10;
    char* found_empty = (char*)bst_search(empty_tree, &search_key_empty);
    printf("Поиск ключа 10 в пустом дереве: %s\n\n", found_empty ? found_empty : "Не найден");
   
    int delete_key_empty = 10; 
    bst_delete(empty_tree, &delete_key_empty); // Должно ничего не делать

    bst_destroy(empty_tree);
    return 0;
}