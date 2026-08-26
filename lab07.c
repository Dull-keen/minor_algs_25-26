/*Реализовать структуру данных вида дерево отрезков (segment tree). Для структуры определить и
реализовать следующий функционал:
    ● создание пустого дерева для n элементов моноида (для моноида конфигурируются:
    множество элементов, ассоциативная бинарная функция и нейтральный элемент);
    ● обновление значения i-го элемента;
    ● вычисление значения ассоциативной бинарной функции моноида на заданном в виде
    индексов начального (включительно) и конечного (включительно) отрезке.
*/


#include <stdio.h>
#include <stdlib.h>

// Определяем базовый тип элементов моноида.
// Для примера используем long long, но его легко заменить.
typedef long long Item;

// Указатель на ассоциативную бинарную функцию моноида.
typedef Item (*CombineFunc)(Item, Item);

// Структура дерева отрезков.
typedef struct SegmentTree
{
    Item* tree;
    int size;
    Item neutral_element;
    CombineFunc combine;
} SegmentTree;

// Вспомогательные функции для работы с деревом отрезков
int
min_int(int a, int b)
{
    return (a < b) ? a : b;
}

int
max_int(int a, int b)
{
    return (a > b) ? a : b;
}

// Пример ассоциативной функции для моноида: сложение.

Item
sum_combine(Item a, Item b)
{
    return a + b;
}


// 1. Создание пустого дерева для n элементов моноида
SegmentTree*
create_segment_tree(int n, Item neutral, CombineFunc comb_func)
{
    SegmentTree* st = (SegmentTree*)malloc(sizeof(SegmentTree));
    if (!st)
    {
        return NULL;
    }

    st->size = n;
    st->neutral_element = neutral;
    st->combine = comb_func;

    st->tree = (Item*)malloc(4 * n * sizeof(Item));
    if (!st->tree)
    {
        free(st);
        return NULL;
    }

    // Инициализация пустого дерева нейтральным элементом моноида
    for (int i = 0; i < 4 * n; i++)
    {
        st->tree[i] = st->neutral_element;
    }

    return st;
}

// Скрытая рекурсивная функция для обновления значения
void
update_rec(SegmentTree* st, int node, int tl, int tr, int pos, Item new_val)
{
    if (tl == tr)
    {
        st->tree[node] = new_val;
    }
    else
    {
        int tm = tl + (tr - tl) / 2;
        if (pos <= tm)
        {
            update_rec(st, node * 2, tl, tm, pos, new_val);
        }
        else
        {
            update_rec(st, node * 2 + 1, tm + 1, tr, pos, new_val);
        }
        
        // Пересчет значения узла через ассоциативную функцию моноида
        st->tree[node] = st->combine(st->tree[node * 2], st->tree[node * 2 + 1]);
    }
}

// 2. Обновление значения i-го элемента
void
update(SegmentTree* st, int index, Item value)
{
    if (index < 0 || index >= st->size)
    {
        return; // Защита от выхода за границы
    }
    update_rec(st, 1, 0, st->size - 1, index, value);
}

// Скрытая рекурсивная функция для вычисления значения на отрезке
Item
query_rec(SegmentTree* st, int node, int tl, int tr, int l, int r)
{
    // Если отрезок некорректен, возвращаем нейтральный элемент
    if (l > r)
    {
        return st->neutral_element;
    }
    
    // Если отрезок полностью совпадает с ответственностью текущего узла
    if (l == tl && r == tr)
    {
        return st->tree[node];
    }
    
    int tm = tl + (tr - tl) / 2;
    
    // Вычисляем результаты для левого и правого детей
    Item left_res = query_rec(st, node * 2, tl, tm, l, min_int(r, tm));
    Item right_res = query_rec(st, node * 2 + 1, tm + 1, tr, max_int(l, tm + 1), r);
    
    // Объединяем результаты функцией моноида
    return st->combine(left_res, right_res);
}

// 3. Вычисление значения функции на заданном отрезке [l, r] (включительно)
Item
query(SegmentTree* st, int l, int r)
{
    if (l < 0 || r >= st->size || l > r)
    {
        return st->neutral_element;
    }
    return query_rec(st, 1, 0, st->size - 1, l, r);
}

// Очистка памяти
void
destroy_segment_tree(SegmentTree* st)
{
    if (st)
    {
        free(st->tree);
        free(st);
    }
}


int
main()
{
    int n = 5;
    
    // Конфигурируем моноид: 
    // Множество: целые числа
    // Бинарная функция: сложение (sum_combine)
    // Нейтральный элемент: 0 
    printf("Создание дерева отрезков для %d элементов...\n", n);
    SegmentTree* st = create_segment_tree(n, 0, sum_combine);

    printf("Массив [0, 0, 0, 0, 0]\n\n");
    
    printf("Добавление элементов 5, 2, 8, 1, 3...\n");
    update(st, 0, 5);
    update(st, 1, 2);
    update(st, 2, 8);
    update(st, 3, 1);
    update(st, 4, 3);
    printf("Теперь массив [5, 2, 8, 1, 3]\n\n");

    printf("Вычисление суммы на отрезке [1, 3]...\n");
    // Вычисление суммы на отрезке индексов [1, 3] (включительно). Ожидается: 2 + 8 + 1 = 11
    printf("Результат на отрезке [1, 3] = %lld\n\n", query(st, 1, 3));

    printf("Обновление значения для элемента с индексом 2 (8 -> 10)...\n");
    // Обновляем элемент с индексом 2 (значение 8 меняем на 10). Массив: [5, 2, 10, 1, 3]
    update(st, 2, 10);

    printf("Снова запрашиваем сумму на [1, 3]. Ожидается: 2 + 10 + 1 = 13\n");
    printf("Результат на отрезке [1, 3] после обновления = %lld\n", query(st, 1, 3));

    destroy_segment_tree(st);
    
    return 0;
}