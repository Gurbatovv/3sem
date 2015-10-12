#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

/*
 * Обязательно сами тестируйте свои программы на разных значениях входных параметров:
 * ElementsCount, ThreadsCount, все элементы равны + то, как у вас сейчас.
 */

#define ElementsCount 1000
#define ThreadsCount  4

double *a;
long int j;
/*
 * Странно, что у вас массивы на 1 больше, чем нужно.
 * 
 * Немного странные название. Понятно, что "a" означало array.
 * Что означают m и o - загадка. Можно воспользоваться переменной result структуры Task, тогда ясно, что это результат работы данной нити.
 */
double m[ThreadsCount + 1];
double o[ThreadsCount + 1];
  
struct Task
{
  int b, c;
  /*
   * Вам нужно добавить сюда индекс задачи index, чтобы каждый поток знал, куда ему писать результат.
   */
};

struct Thread
{
  pthread_t id;
  int result;
  /*
   * Вы создали переменную result по аналогии с программой Дарьи. Но в неё писала результат работы каждого потока.
   * У вас для этого заведены массивы m, o.
   */
};


void* my_thread(void* task) 
{
    long int i;
    for(i = ((struct Task*)task) -> c; i < ((struct Task*)task) -> b; i++)
    {
        /*
         * Нужно суммировать в элемент массива task->index. Вы же фактически суммировали в одну ячейку массива m[ThreadsCount] результаты всех потоков.
         */
        m[j] = m[j] + a[i]; 
    } 
    return NULL;
}

void* my_thread1(void* task) 
{
    long int i;
    pthread_t my_thread_id;
    for (i = ((struct Task*)task) -> c; i < ((struct Task*)task) -> b; i++)
    {
      /*
       * Та, же беда с j.
       */
        o[j] = o[j] + (a[i] - m[ThreadsCount + 1]) * (a[i] - m[ThreadsCount + 1]);
    } 
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
        tasks[i].c =  i      * ElementsCount / ThreadsCount;
        tasks[i].b = (i + 1) * ElementsCount / ThreadsCount;
    }

    if (ElementsCount % ThreadsCount != 0)
    {
        tasks[ThreadsCount - 1].b = ElementsCount;
    }

    a = (double *)malloc(ElementsCount * sizeof(double));
    
    for (i = 0; i < ElementsCount; i++)
    a[i] = 1 + rand() % 2;  
    
    /*
     * Когда будете тестировать программу на большом числе элементов или измерять производительность, то
     * вывод на экран надо закомментировать, т.к. он потребует больше времени, чем расчёт среднего и дисперсии.
     */
    /*
    printf("Данные: ");
    
    for (i = 0; i < ElementsCount; i++)
    {
        printf("%f " , a[i]);
    }
    printf("\n");
    */
    
    /*
     * Видимо, здесь должно быть условие j < ThreadsCount, т.к. нумерация с 0.
     */
    
    clock_t startTime = clock();
    
    for(j = 0; j <= ThreadsCount; j++)
    {
      /*
       * Так делать нельзя. У вас j - глобальная переменная, для всех нитей, имеющая одно и то же значение.
       * Если ваша программа в общем случае будет работать некорректно, особенно при больших ElementsCount порядка 10^8 и более.
       * 
       * Ваши ThreadsCount нитей "быстро" создаются, начинают работать, и пишут результат фактически в одну и ту же ячейку j == ThreadsCount.
       * Как мы с вами видели на семинаре это приводит к условию гонки, и результат будет некорректный. 
       */
        threads[j].result = pthread_create(&(threads[j].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread ,
                                      &tasks[j]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
      
    
    for (i = 0; i <= ThreadsCount; i++)
    {
        m[ThreadsCount + 1] = m[ThreadsCount + 1] + m[i];
    } 
  
    
    m[ThreadsCount + 1] = m[ThreadsCount + 1] / ElementsCount;
    
    
    for(j = 0; j <= ThreadsCount; j++)
    {
        threads[j].result = pthread_create(&(threads[j].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread1 ,
                                      &tasks[j]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
   
    for (i = 0; i <= ThreadsCount; i++)
    {
        o[ThreadsCount + 1] = o[ThreadsCount + 1] + o[i];
    }
    
    
    o[ThreadsCount + 1] = o[ThreadsCount + 1] / ElementsCount;
    
    /*
     * Следующая пара строк измеряет затрачиваемое на расчёт время.
     * Если вы используете виртуальную машину дома, то ускорения от многопоточности вы скорее всего не увидете.
     * На многоядерной машине, где Linux стоит основной ОС, ускорение видно, и завистит от числа ядер.
     * 
     * Также время работы можно измерять с помощью системной утилиты time:
     * time ./a.out
     */
    clock_t endTime = clock();
    printf("Time elapsed: %f\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);
    
    printf("Среднее значение = %f\n" , m[ThreadsCount + 1]);
    printf("Дисперсия = %f\n" , o[ThreadsCount + 1]);
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
