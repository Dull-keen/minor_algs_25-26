/*Реализовать структуру данных вида дерево Фенвика (Fenwick tree). Для структуры определить и
реализовать следующий функционал:
    ● создание пустого дерева для n элементов моноида (для моноида конфигурируются:
    множество элементов, ассоциативная бинарная функция и нейтральный элемент);
    ● обновление значения i-го элемента;
    ● вычисление значения ассоциативной бинарной функции моноида на заданном в виде
    индексов начального (включительно) и конечного (включительно) отрезке.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// 1. Конфигурация моноида: множество элементов
typedef int Element;

// 2. Конфигурация моноида: сигнатура ассоциативной бинарной функции
typedef Element (*BinaryOp)(Element, Element);

// Структура дерева
typedef struct
{
    Element* tree;
    Element* values; // Храним исходные элементы для работы без обратных операций
    int size;
    BinaryOp op;
    Element neutral; // 3. Конфигурация моноида: нейтральный элемент
} FenwickTree;

// Пример ассоциативной функции для моноида (максимум)
Element
max_op(Element a, Element b)
{
    return (a > b) ? a : b;
}

// Создание пустого дерева для n элементов
FenwickTree*
create_tree(int n, BinaryOp op, Element neutral)
{
    FenwickTree* ft = (FenwickTree*)malloc(sizeof(FenwickTree));
    ft->size = n;
    ft->op = op;
    ft->neutral = neutral;

    // Дерево Фенвика традиционно использует 1-индексацию
    ft->tree = (Element*)malloc(sizeof(Element) * (n + 1));
    ft->values = (Element*)malloc(sizeof(Element) * (n + 1));

    for (int i = 0; i <= n; i++)
    {
        ft->tree[i] = ft->neutral;
        ft->values[i] = ft->neutral;
    }

    return ft;
}

// Обновление значения i-го элемента
// Сложность: O(log^2 N)
void
update(FenwickTree* ft, int i, Element val)
{
    if (i < 1 || i > ft->size)
    {
        return;
    }

    ft->values[i] = val;

    for (int j = i; j <= ft->size; j += j & -j)
    {
        // Пересчитываем значение узла дерева, комбинируя дочерние элементы
        Element sum = ft->values[j];
        int z = j - 1;
        int limit = j - (j & -j);

        while (z > limit)
        {
            // Важно: порядок операндов op() учитывает некоммутативность моноида (слева направо)
            sum = ft->op(ft->tree[z], sum);
            z -= z & -z;
        }
        ft->tree[j] = sum;
    }
}

// Вычисление значения функции на отрезке [L, R]
// Сложность: O(log N)
Element
query(FenwickTree* ft, int L, int R)
{
    if (L < 1 || R > ft->size || L > R)
    {
        return ft->neutral;
    }

    Element res = ft->neutral;
    int curr = R;

    while (curr >= L)
    {
        int next = curr - (curr & -curr);
        
        // Если блок tree[curr] полностью входит в наш отрезок [L, R]
        if (next >= L - 1)
        {
            res = ft->op(ft->tree[curr], res);
            curr = next;
        }
        // В противном случае спускаемся по одному элементу, используя массив values
        else
        {
            res = ft->op(ft->values[curr], res);
            curr--;
        }
    }

    return res;
}

// Очистка памяти
void
free_tree(FenwickTree* ft)
{
    if (ft)
    {
        free(ft->tree);
        free(ft->values);
        free(ft);
    }
}

int
main()
{
    int n = 5;

    printf("Создание дерева отрезков для %d элементов...\n", n);
    printf("Массив [0, 0, 0, 0, 0, 0]\n\n");
    // Инициализация дерева с функцией max_op и нейтральным элементом INT_MIN
    FenwickTree* ft = create_tree(n, max_op, INT_MIN);

    printf("Добавление элементов 5, 2, 8, 3, 1 (индексы от 1 до 5)...\n");
    // Добавим элементы (работаем в 1-индексации)(индексы от 1 до 5)
    // Массив: [0, 5, 2, 8, 3, 1]
    update(ft, 1, 5);
    update(ft, 2, 2);
    update(ft, 3, 8);
    update(ft, 4, 3);
    update(ft, 5, 1);
    printf("Теперь массив [0, 5, 2, 8, 3, 1]\n\n");

    printf("Максимум в [1, 5]: %d\n", query(ft, 1, 5)); // Ожидаем 8
    printf("Максимум в [1, 2]: %d\n", query(ft, 1, 2)); // Ожидаем 5
    printf("Максимум в [4, 5]: %d\n", query(ft, 4, 5)); // Ожидаем 3

    printf("\nОбновление значения для элемента с индексом 2 (2 -> 10)...\n");
    // Обновляем элемент (заменяем 2 на 10)
    // Массив: [0, 5, 10, 8, 3, 1]
    update(ft, 2, 10);
    printf("Теперь массив [0, 5, 10, 8, 3, 1]\n\n");

    printf("После обновления:\n");
    printf("Максимум в [1, 5]: %d\n", query(ft, 1, 5)); // Ожидаем 10
    printf("Максимум в [3, 5]: %d\n", query(ft, 3, 5)); // Ожидаем 8 

    free_tree(ft);
    return 0;
}