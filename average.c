#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>



#define ElementsCount 100000
#define ThreadsCount  4

long double *array;

long double sum[ThreadsCount];
long double disp[ThreadsCount];
long double average;
  
struct Task
{
  int b, a, index;
  
};

struct Thread
{
  pthread_t id;
};


void* my_thread(void* task) 
{
    long int i;
    long double summa = 0;
    pthread_t my_thread_id;
    for(i = ((struct Task*)task) -> a; i < ((struct Task*)task) -> b; i++)
    {
       
        summa = summa + array[i]; 
    } 
    sum[((struct Task*)task)->index] = summa;
}

void* my_thread1(void* task) 
{
    long int i;
    long double summa = 0;
    pthread_t my_thread_id;
    for (i = ((struct Task*)task) -> a; i < ((struct Task*)task) -> b; i++)
    {
        summa = summa + (array[i] - average) * (array[i] - average);
    } 
    disp[((struct Task*)task)->index] = summa;
    return NULL;
}


int main()
{
    int i;
    struct Task tasks[ElementsCount];
    struct Thread threads[ElementsCount];
    srand(time(NULL));

    for(i = 0; i < ThreadsCount; i++)
    {
        tasks[i].index = i;
        tasks[i].a =  i * ElementsCount / ThreadsCount;
        tasks[i].b = (i + 1) * ElementsCount / ThreadsCount;
    }

    if (ElementsCount % ThreadsCount != 0)
    {
        tasks[ThreadsCount - 1].b = ElementsCount;
    }

    array = (long double *)malloc(ElementsCount * sizeof(long double));
    
    for (i = 0; i < ElementsCount; i++)
    array[i] = 1 + rand() % 2;  
    
   
    
    /*printf("Данные: ");
    
    for (i = 0; i < ElementsCount; i++)
    {
        printf("%Lf " , array[i]);
    }
    printf("\n");*/
    
    
    /*
     * Видимо, здесь должно быть условие j < ThreadsCount, т.к. нумерация с 0.
     */
    
    clock_t startTime = clock();
    
    for(i = 0; i < ThreadsCount; i++)
    {
      /*
       * Так делать нельзя. У вас j - глобальная переменная, для всех нитей, имеющая одно и то же значение.
       * Если ваша программа в общем случае будет работать некорректно, особенно при больших ElementsCount порядка 10^8 и более.
       * 
       * Ваши ThreadsCount нитей "быстро" создаются, начинают работать, и пишут результат фактически в одну и ту же ячейку j == ThreadsCount.
       * Как мы с вами видели на семинаре это приводит к условию гонки, и результат будет некорректный. 
       */
        sum[i] = pthread_create(&(threads[i].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread ,
                                      &tasks[i]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
      
    
    for (i = 0; i < ThreadsCount; i++)
    {
       sum[ThreadsCount] = sum[ThreadsCount] + sum[i];
    } 
  
    /*for (i = 0; i <= ThreadsCount; i++)
    {
        printf("%Lf " , sum[i]);
    }
    printf("\n");
    
    sum[ThreadsCount] = sum[ThreadsCount] / ElementsCount;
    average = sum[ThreadsCount];
    printf("%Lf \n" , sum[ThreadsCount]);*/
    
    for(i = 0; i < ThreadsCount; i++)
    {
        disp[i] = pthread_create(&(threads[i].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread1 ,
                                      &tasks[i]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
   
    for (i = 0; i < ThreadsCount; i++)
    {
        disp[ThreadsCount] = disp[ThreadsCount] + disp[i];
    }
    
    
   disp[ThreadsCount] = disp[ThreadsCount] / ElementsCount;
    
    /*
     * Следующая пара строк измеряет затрачиваемое на расчёт время.
     * Если вы используете виртуальную машину дома, то ускорения от многопоточности вы скорее всего не увидете.
     * На многоядерной машине, где Linux стоит основной ОС, ускорение видно, и завистит от числа ядер.
     * 
     * Также время работы можно измерять с помощью системной утилиты time:
     * time ./a.out
     */
    
    clock_t endTime = clock();
    printf("Time elapsed: %Lf\n", (long double)(endTime - startTime) / CLOCKS_PER_SEC);
    
    printf("Среднее значение = %Lf\n" , sum[ThreadsCount]);
    printf("Дисперсия = %Lf\n" , disp[ThreadsCount]);
    return 0;
}

/*
 * Итого:
 * Уже очень близко к правде. Если вы протестируете сами свою программу на большом числе элементов
 * и выведете результат работы (сумму элементов) для каждой нити, то увидете, что результат неверен.
 * 
 * Пытайтесь сами тестировать свою программу на "маленьких" тестах, чтобы проверить корректность работы, и на "больших" стресс-тестах, чтобы
 * проверить производительность (прирост ускорения в данном случае).
 * 
 * Для этого можо выводить на экран некоторую отладочную информация после каждого этапа вычисления. Когда убеждаетесь, что всё ок, то 
 * вывод отладочной информации убераете.
 */
