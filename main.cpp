#include <iostream>
#include "include/ThreadPool.h"
#include <unistd.h>
#include<cstdio>
using namespace std;
const int Max_Thread_Num = 5;
const int Max_request_Num = 6;

class Task
{
public:
    Task(int in_num):i(0){i = in_num;};
    void fun(){cout<<"this is task "<<i<<endl;sleep(3);};
private:
    int i;

};

int main()
{
    ThreadPool<Task>* m_ThreadPool = new ThreadPool<Task>(Max_Thread_Num,Max_request_Num);
    Task** task_list = new Task*[Max_Thread_Num*3];
    for(int i = 0;i < Max_Thread_Num*3;i++)
    {
        task_list[i] = new Task(i);
        if(m_ThreadPool->append(task_list[i]) == -1)
        {
            delete task_list[i];
            i--;
            sleep(2);
        }
    }
    getchar();
    return 0;
}
