#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int
gcd(int a, int b)
{
    while (b)
    {
        a %= b;
        int temp = a;
        a = b;
        b = temp;
    }
    return a;
}

// Функция для вычисления a^exp mod m
int
power_mod(long long base, int exp, int mod)
{
    long long res = 1;
    base %= mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            res = (res * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return (int)res;
}

// Функция для вычисления обратного элемента по модулю
int
mod_inverse(int a, int m)
{
    int m0 = m, t, q;
    int x0 = 0, x1 = 1;

    if (m == 1)
    {
        return 0;
    }

    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
    {
        x1 += m0;
    }

    return x1;
}

int
phi(int n)
{
    int result = n;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            while (n % i == 0)
            {
                n /= i;
            }
            result -= result / i;
        }
    }
    if (n > 1)
    {
        result -= result / n;
    }
    return result;
}

bool
has_primitive_root(int n)
{
    if (n == 2 || n == 4)
    {
        return true;
    }
    if (n % 2 == 0)
    {
        n /= 2;
    }
    if (n % 2 == 0)
    {
        return false;
    }
    for (int i = 3; i * i <= n; i += 2)
    {
        if (n % i == 0)
        {
            while (n % i == 0)
            {
                n /= i;
            }
            return n == 1;
        }
    }
    return n > 1;
}

int
find_first_primitive_root(int n, int phi_n)
{
    int factors[100];
    int count = 0;
    int temp = phi_n;
    
    for (int i = 2; i * i <= temp; i++)
    {
        if (temp % i == 0)
        {
            factors[count++] = i;
            while (temp % i == 0)
            {
                temp /= i;
            }
        }
    }
    if (temp > 1)
    {
        factors[count++] = temp;
    }

    for (int g = 1; g < n; g++)
    {
        if (gcd(g, n) != 1)
        {
            continue;
        }
        
        bool ok = true;
        for (int i = 0; i < count; i++)
        {
            if (power_mod(g, phi_n / factors[i], n) == 1)
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            return g;
        }
    }
    return -1;
}

void
print_all_primitive_roots(int n)
{
    if (!has_primitive_root(n))
    {
        printf("Для n = %d первообразных корней не существует.\n", n);
        return;
    }
    
    int phi_n = phi(n);
    int g = find_first_primitive_root(n, phi_n);
    
    if (g == -1)
    {
        return;
    }

    printf("Первообразные корни для n = %d: ", n);
    for (int i = 1; i <= phi_n; i++)
    {
        if (gcd(i, phi_n) == 1)
        {
            printf("%d ", power_mod(g, i, n));
        }
    }
    printf("\n");
}


int**
create_ntt_matrix(int n, int omega, int mod, bool is_inverse)
{
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int*)malloc(n * sizeof(int));
    }

    int base_omega = is_inverse ? mod_inverse(omega, mod) : omega;
    int n_inv = mod_inverse(n, mod);

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            int exp = (row * col) % n;
            matrix[row][col] = power_mod(base_omega, exp, mod);
            
            if (is_inverse)
            {
                matrix[row][col] = (1LL * matrix[row][col] * n_inv) % mod;
            }
        }
    }
    return matrix;
}

void
free_matrix(int** matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void
ntt_matrix_multiply(int** matrix, int* input, int* output, int n, int mod)
{
    for (int i = 0; i < n; i++)
    {
        long long sum = 0;
        for (int j = 0; j < n; j++)
        {
            sum = (sum + 1LL * matrix[i][j] * input[j]) % mod;
        }
        output[i] = (int)sum;
    }
}

void
print_matrix(const char* label, int** matrix, int n)
{
    printf("%s:\n", label);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%4d ", matrix[i][j]); 
        }
        printf("\n");
    }
}


void
fft_recursive(int* a, int n, int root_n, int mod)
{
    if (n <= 1)
    {
        return;
    }

    int* a0 = (int*)malloc((n / 2) * sizeof(int));
    int* a1 = (int*)malloc((n / 2) * sizeof(int));

    for (int i = 0; 2 * i < n; i++)
    {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }

    int next_root = power_mod(root_n, 2, mod);
    
    fft_recursive(a0, n / 2, next_root, mod);
    fft_recursive(a1, n / 2, next_root, mod);

    int w = 1;
    for (int i = 0; 2 * i < n; i++)
    {
        int u = a0[i];
        int v = (1LL * w * a1[i]) % mod;
        
        a[i] = (u + v) % mod;
        a[i + n / 2] = (u - v + mod) % mod;
        
        w = (1LL * w * root_n) % mod;
    }

    free(a0);
    free(a1);
}

void
fft(int* a, int n, int root_n, int mod, bool is_inverse)
{
    if (is_inverse)
    {
        root_n = mod_inverse(root_n, mod);
    }
    
    fft_recursive(a, n, root_n, mod);
    
    if (is_inverse)
    {
        int n_inv = mod_inverse(n, mod);
        for (int i = 0; i < n; i++)
        {
            a[i] = (1LL * a[i] * n_inv) % mod;
        }
    }
}


void
print_array(const char* label, int* arr, int n)
{
    printf("%s: ", label);
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int
main()
{
    printf("1. Поиск первообразных корней\n");
    print_all_primitive_roots(7);  
    print_all_primitive_roots(8);  
    print_all_primitive_roots(62);  
    printf("\n");

    int mod = 17;
    int n = 4; 
    int omega = 13; 

    int signal_matrix[] = {1, 2, 3, 4};
    int signal_fft[] = {1, 2, 3, 4};
    
    printf("2. ДПФ через матрицу Вандермонда в Z_17\n");
    int output_dft[4];
    int output_idft[4];

    int** w_matrix = create_ntt_matrix(n, omega, mod, false);
    int** inv_w_matrix = create_ntt_matrix(n, omega, mod, true);

    print_matrix("Прямая матрица Вандермонда", w_matrix, n);
    print_matrix("Обратная матрица Вандермонда", inv_w_matrix, n);

    ntt_matrix_multiply(w_matrix, signal_matrix, output_dft, n, mod);
    print_array("Прямое NTT (Матрица)", output_dft, n);

    ntt_matrix_multiply(inv_w_matrix, output_dft, output_idft, n, mod);
    print_array("Обратное NTT (Матрица)", output_idft, n);

    free_matrix(w_matrix, n);
    free_matrix(inv_w_matrix, n);
    printf("\n");

    printf("3. Быстрое NTT (Кули-Тьюки) в Z_17\n");
    fft(signal_fft, n, omega, mod, false);
    print_array("Прямое быстрое NTT", signal_fft, n);

    fft(signal_fft, n, omega, mod, true);
    print_array("Обратное быстрое NTT", signal_fft, n);

    return 0;
}