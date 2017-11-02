#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<list>
#include<pthread.h>
#include<exception>
#include<semaphore.h>
#include<assert.h>

template <class T>
class ThreadPool
{
    public:
        ThreadPool(int Max_Thread_Num,int Max_request_Num);
        virtual ~ThreadPool();
        ThreadPool(const ThreadPool& other);
        int append(T* AppendClass);
        static void* StartWork(void* arg);
        void run();
    protected:
    private:
        int             Max_Thread_Num;
        int             Max_request_Num;
        pthread_mutex_t m_Mutex;
        sem_t           m_Sem;
        pthread_t*      m_Pthread;
        std::list<T*>   Task_List;
        bool            Stop_Tag;
};
/************************************************************************/
/* fun:construct  create threads,inital mutex and semaphore             */
/* Entrance:Max_Thread_Num,Max_request_Num
/* Return:nothing
/************************************************************************/
template<class T>
ThreadPool<T>::ThreadPool(int Max_Thread_Num,int Max_request_Num):
    Max_Thread_Num(Max_Thread_Num),Max_request_Num(Max_request_Num),m_Pthread(NULL),Stop_Tag(false)
{
    assert(Max_Thread_Num > 0);
    assert(Max_request_Num > 0);
    //create threads,inital mutex and semaphore
    m_Pthread = new pthread_t[Max_Thread_Num];
    if(!m_Pthread)
    {
        std::cout<<"create thread error"<<std::endl;
    }
    if(sem_init(&m_Sem,0,0) != 0)
    {
        std::cout<<"semaphore initial error"<<std::endl;
    }
    if(pthread_mutex_init(&m_Mutex,NULL) != 0)
    {
        std::cout<<"mutex initial error"<<std::endl;
    }
    //start thread
    for(int i = 0;i < Max_Thread_Num;i++)
    {
        if( pthread_create(&m_Pthread[i],NULL,StartWork,this) != 0)
        {
            delete[] m_Pthread;
        }
        if(pthread_detach(m_Pthread[i]))
        {
            delete[] m_Pthread;
        }
    }

}


template<class T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_Pthread;
    m_Pthread = NULL;
    Stop_Tag = true;
}
/************************************************************************/
/* fun:append new task into TaskList and notify thread pool             */
/* Entrance:the pointer of Task
/* Return:1 represent success ,-1 represent fail
/************************************************************************/
template<class T>
int ThreadPool<T>::append(T* AppendClass)
{
    pthread_mutex_lock(&m_Mutex);
    int sz = Task_List.size();
    if(sz == Max_request_Num)
    {
        std::cout<<"request has reached max"<<std::endl;
        pthread_mutex_unlock(&m_Mutex);
        return -1;
    }
    Task_List.push_back(AppendClass);
    pthread_mutex_unlock(&m_Mutex);
    sem_post(&m_Sem);
    return 1;
}

/************************************************************************/
/* fun:callback fun when pthread create,then start the thread           */
/* Entrance:the pointer this
/* Return:the pointer this
/************************************************************************/
template<class T>
void* ThreadPool<T>::StartWork(void* arg)
{
    ThreadPool* m_threadpool = (ThreadPool*)arg;
    m_threadpool->run();
    return m_threadpool;
}

/************************************************************************/
/* fun:The main function of dealing with append tasks                   */
/* Entrance:nothing
/* Return:nothing
/************************************************************************/
template<class T>
void ThreadPool<T>::run()
{
    while(!Stop_Tag)
    {
        sem_wait(&m_Sem);
        pthread_mutex_lock(&m_Mutex);
        if(Task_List.empty())
        {
            pthread_mutex_unlock(&m_Mutex);
            continue;
        }
        T* temp = Task_List.front();
        Task_List.pop_front();
        pthread_mutex_unlock(&m_Mutex);
        temp->fun();
    }
}

#endif // THREADPOOL_H
