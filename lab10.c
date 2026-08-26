#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define PI 3.14159265358979323846

// Нахождение первообразных корней

// Вспомогательная функция для вычисления НОД
int
gcd(int a, int b)
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

//1. Функции Эйлера

// 1.1 По определению
int
euler_definition(int n)
{
    int count = 0;
    for (int i = 1; i <= n; i++)
    {
        if (gcd(i, n) == 1)
        {
            count++;
        }
    }
    return count;
}

// 1.2 По теореме арифметики (факторизация)
int
euler_factorization(int n)
{
    int result = n;
    for (int p = 2; p * p <= n; p++)
    {
        if (n % p == 0)
        {
            while (n % p == 0)
            {
                n /= p;
            }
            result -= result / p;
        }
    }
    if (n > 1)
    {
        result -= result / n;
    }
    return result;
}

// 1.3 Через дискретное преобразование Фурье
double
euler_dft(int n)
{
    double sum = 0.0;
    for (int k = 1; k <= n; k++)
    {
        sum += gcd(k, n) * cos(2.0 * PI * k / n);
    }
    return sum;
}

//2 & 3. Корни из единицы


void
print_roots_of_unity(int n)
{
    printf("\nВсе корни степени %d из 1 в C\n", n);
    for (int k = 0; k < n; k++)
    {
        double angle = 2.0 * PI * k / n;
        double complex z = cos(angle) + I * sin(angle);
        printf("z_%d = %6.3f %+.3fi\n", k, creal(z), cimag(z));
    }
}

void
print_primitive_roots(int n)
{
    printf("\nПервообразные корни степени %d из 1 в C\n", n);
    for (int k = 0; k < n; k++)
    {
        if (gcd(k, n) == 1)
        {
            double angle = 2.0 * PI * k / n;
            double complex z = cos(angle) + I * sin(angle);
            printf("z_%d = %6.3f %+.3fi (k = %d)\n", k, creal(z), cimag(z), k);
        }
    }
}

//4. Матрица Вандермонда

void
build_vandermonde(int n, double complex root, double complex** V, double complex** V_inv)
{
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            // V_{j,k} = w^{j*k}
            V[j][k] = cpow(root, j * k);
            
            // V^{-1}_{j,k} = (w^{-j*k}) / n. 
            // Обратный элемент к корню на единичной окружности - это его сопряженное
            V_inv[j][k] = cpow(conj(root), j * k) / n;
        }
    }
}

//5. ДПФ

void
matrix_vector_mult(int n, double complex** matrix, double complex* vec, double complex* out)
{
    for (int i = 0; i < n; i++)
    {
        out[i] = 0;
        for (int j = 0; j < n; j++)
        {
            out[i] += matrix[i][j] * vec[j]; 
        }
    }
}

// Вспомогательные функции для памяти и вывода

double complex**
alloc_matrix(int n)
{
    double complex** mat = malloc(n * sizeof(double complex*));
    for (int i = 0; i < n; i++)
    {
        mat[i] = malloc(n * sizeof(double complex));
    }
    return mat;
}

void
free_matrix(int n, double complex** mat)
{
    for (int i = 0; i < n; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

void
demo_dft_transform(int n)
{
    printf("\nДемонстрация прямого и обратного ДПФ (n = %d)\n", n);

    double complex** V = alloc_matrix(n);
    double complex** V_inv = alloc_matrix(n);

    // Задаем первообразный корень: w = e^{-2pi*i / n} (стандарт ДПФ)
    double angle = -2.0 * PI / n;
    double complex w = cos(angle) + I * sin(angle);

    build_vandermonde(n, w, V, V_inv);

    double complex* x = malloc(n * sizeof(double complex));
    double complex* X = malloc(n * sizeof(double complex));
    double complex* x_rec = malloc(n * sizeof(double complex));

    // Исходный вектор из R^n
    printf("Исходный вектор x (в R^n):\n");
    for (int i = 0; i < n; i++)
    {
        x[i] = (double)(i + 1); // 1.0, 2.0, 3.0...
        printf("%6.3f ", creal(x[i]));
    }
    printf("\n");

    // Прямое ДПФ
    matrix_vector_mult(n, V, x, X);
    printf("Прямое ДПФ (X = V * x):\n");
    for (int i = 0; i < n; i++)
    {
        printf("X[%d] = %6.3f %+.3fi\n", i, creal(X[i]), cimag(X[i]));
    }

    // Обратное ДПФ
    matrix_vector_mult(n, V_inv, X, x_rec);
    printf("Обратное ДПФ (x' = V_inv * X):\n");
    for (int i = 0; i < n; i++)
    {
        // Мнимая часть должна обнулиться
        printf("%6.3f ", creal(x_rec[i])); 
    }
    printf("\n");

    free_matrix(n, V);
    free_matrix(n, V_inv);
    free(x);
    free(X);
    free(x_rec);
}


int
main()
{
    int n = 62; // Размерность для демонстрации
    
    printf("Демонстрация работы алгоритмов для n = %d\n", n);

    printf("\nВычисление функции Эйлера phi(%d)x\n", n);
    printf("1. По определению: %d\n", euler_definition(n));
    printf("2. По факторизации: %d\n", euler_factorization(n));
    printf("3. Через ДПФ: %.4f\n", euler_dft(n));

    print_roots_of_unity(n);
    print_primitive_roots(n);
    
    demo_dft_transform(n);

    return 0;
}