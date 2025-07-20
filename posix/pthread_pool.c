#include <stdlib.h>
#include <pthread.h>

static void *thread_pool_worker(void *arg) {
    thread_pool_task_t *task = NULL;
    thread_pool_t *pool = (thread_pool_t *)arg;
    
    while (1) {
        pthread_mutex_lock(&pool->lock);
        
        // Wait for tasks or shutdown
        while (pool->queue_size == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }
        
        // Exit on shutdown
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }
        
        // Get next task
        task = pool->queue;
        if (task) {
            pool->queue = task->next;
            pool->queue_size--;
        }
        
        pthread_mutex_unlock(&pool->lock);
        
        // Execute task
        if (task) {
            task->function(task->argument);
            free(task);
        }
    }
    return NULL;
}

thread_pool_t *thread_pool_create(int thread_count) {
    thread_pool_t *pool = NULL;

    if (thread_count <= 0) thread_count = 1;
    
    pool = malloc(sizeof(thread_pool_t));
    if (!pool) return NULL;
    
    // Initialize pool
    pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (!pool->threads) {
        free(pool);
        return NULL;
    }
    
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);
    
    pool->queue = NULL;
    pool->queue_size = 0;
    pool->thread_count = thread_count;
    pool->shutdown = 0;
    pool->started = 0;
    
    // Create worker threads
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool) != 0) {
            thread_pool_destroy(pool, 1);
            return NULL;
        }
        pool->started++;
    }
    
    return pool;
}

int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *argument) {
    thread_pool_task_t *task = NULL;
    if (!pool || !function) return -1;
    
    task = malloc(sizeof(thread_pool_task_t));
    if (!task) return -1;
    
    task->function = function;
    task->argument = argument;
    task->next = NULL;
    
    pthread_mutex_lock(&pool->lock);
    
    // Add to queue
    if (!pool->queue) {
        pool->queue = task;
    } else {
        thread_pool_task_t *cur = pool->queue;
        while (cur->next) cur = cur->next;
        cur->next = task;
    }
    pool->queue_size++;
    
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    
    return 0;
}

int thread_pool_destroy(thread_pool_t *pool, int graceful) {
    thread_pool_task_t *task = NULL;

    if (!pool) return -1;
    
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    
    // Wait for threads to finish
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    // Cleanup
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    
    // Process remaining tasks
    task = pool->queue;
    while (task) {
        thread_pool_task_t *next = task->next;
        if (graceful) {
            task->function(task->argument);
        }
        free(task);
        task = next;
    }
    
    free(pool->threads);
    free(pool);
    return 0;
}