#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

double  x, *a , r;
long int  n, k;
double m[5] = {0, 0, 0, 0, 0};
double o[4] = {0, 0, 0, 0};
double h[5] = {0, 0, 0, 0, 0};
double t[5] = {0, 0, 0, 0, 0};
double u[5] = {0, 0, 0, 0, 0};

/* сортируем массив данных*/
typedef int (*MyComp)(double, double);
int g(double c, double b)
{
    if (c < b)
        return 1;
    else 
        return 0;
}      

void sort(double* c, int n, MyComp comp)
{
    int i, j, y;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n - 1; j++)
        {
            if (comp(a[j], a[j + 1]) == 0)
            {
                y = a[j + 1];
                a[j + 1] = a[j];
                a[j] = y;
            }
        }
    }   
}

void* my_thread1(void* dummy) 
{
    int i, j, d;
    pthread_t my_thread_id;
    for (i = 0; i < k; i++)
    {
        m[1] = m[1] + a[i]; /*для нахождения среднего*/
    } 
    d = 0;
    for (j = 0; j < (n - 1); j++)
    {
    if (a[j] < (a[0] + r)) /*считаем количество элементов массива входящих в данный интервал r  это шаг*/
    {
        o[1]++;
    }  
    }
    h[1] = a[0] + r/ 2; /*ищем середину интервала*/
    t[1] = h[1] * o[1];
    return NULL;
}

void* my_thread2(void* dummy) 
{
    int i, j, d;
    pthread_t my_thread_id;
    for (i = k; i < (2 * k); i++)
    {
        m[2] = m[2] + a[i];
    } 
    d = o[1];
    for (j = d; j < n; j++)
    {
    if (a[j] <= (a[0] + (2 * r)))
    {
        o[2] = o[2] + 1;
    }
    }
    h[2] = a[0] + 3 * r/ 2;
    t[2] = h[2] * o[2];
    return NULL;
}

void* my_thread3(void* dummy) 
{
    int i, j, d;
    pthread_t my_thread_id;
    for (i = (2 * k); i < (3 * k); i++)
    {
        m[3] = m[3] + a[i];
    } 
    d = o[2] + o[1];
    for (j = d; j < n; j++)
    {
    if (a[j] <= (a[0] + (3 * r)))
    {
        o[3] = o[3] + 1;
    }
    }
    h[3] = a[0] + 5 * r/ 2;
    t[3] = h[3] * o[3];
    return NULL;
}

void* my_thread4(void* dummy) 
{
    int j = 0;
    pthread_t my_thread_id;
    for (j = (3 * k) ; j < n; j++)
    {
        m[4] = m[4] + a[j];
    } 
    o[0] = n - (o[1] + o[2] + o[3]);
    h[0] = a[0] + 7 * r/ 2;
    t[4] = h[0] * o[0];
    return NULL;
}

int main()
{
    int i;
    srand(time(NULL));
    pthread_t thread_id1 , thread_id2, thread_id3, thread_id4, my_thread_id;
    int result;
    scanf("%ld" , &n);
    k = n / 4;
    a = (double *)malloc(n*sizeof(double));
    for (i = 0; i < n; i++)
    a[i] = 1 + rand() % 100;  
    sort(a, n, g);
    r = (a[n - 1] - a[0]) / 4;
    printf("Данные: ");
    for (i = 0; i < n; i++)
        printf("%f " , a[i]);
    printf("\n");
    
    result = pthread_create(&thread_id1 , 
                            (pthread_attr_t *)NULL , 
                            my_thread1 ,
                            NULL);
    
    
    result = pthread_create(&thread_id2 , 
                            (pthread_attr_t *)NULL , 
                            my_thread2 ,
                            NULL);
    
    result = pthread_create(&thread_id3 , 
                            (pthread_attr_t *)NULL , 
                            my_thread3 ,
                            NULL);
    
    result = pthread_create(&thread_id4 , 
                            (pthread_attr_t *)NULL , 
                            my_thread4 ,
                            NULL);
    pthread_join(thread_id1 , (void **) NULL);
    pthread_join(thread_id2 , (void **) NULL);
    pthread_join(thread_id3 , (void **) NULL);
    pthread_join(thread_id4 , (void **) NULL);
    
    for (i = 1; i < 5; i++)
    {
        m[0] = m[0] + m[i];
        t[0] = t[0] + t[i];
    } 
    x = t[0] / n;
      for (i = 1; i < 5; i++)
    {
        u[i] = (h[i] - x) * (h[i] - x) * o[i];
        u[0] = u[0] + u[i];
    } 
    m[0] = m[0] / n;
    u[0] = u[0] / n;
    u[0] = sqrt(u[0]);
    printf("Среднее значение = %f\n" , m[0]);
    printf("Дисперсия = %f\n" , u[0]);
    return 0;
}    