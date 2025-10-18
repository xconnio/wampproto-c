#include "wampproto/mutex.h"

#ifdef ESP_PLATFORM
// ===================== FreeRTOS =====================
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct Mutex {
    SemaphoreHandle_t handle;
};

Mutex *mutex_create(void) {
    Mutex *m = malloc(sizeof(Mutex));
    if (!m) return NULL;
    m->handle = xSemaphoreCreateMutex();
    return m;
}

void mutex_lock(Mutex *m) { xSemaphoreTake(m->handle, portMAX_DELAY); }

void mutex_unlock(Mutex *m) { xSemaphoreGive(m->handle); }

void mutex_destroy(Mutex *m) {
    vSemaphoreDelete(m->handle);
    free(m);
}

#else
// ===================== POSIX pthreads =====================
#include <pthread.h>
#include <stdlib.h>

struct Mutex {
    pthread_mutex_t handle;
};

Mutex *mutex_create(void) {
    Mutex *m = malloc(sizeof(Mutex));
    if (!m) return NULL;
    pthread_mutex_init(&m->handle, NULL);
    return m;
}

void mutex_lock(Mutex *m) { pthread_mutex_lock(&m->handle); }

void mutex_unlock(Mutex *m) { pthread_mutex_unlock(&m->handle); }

void mutex_destroy(Mutex *m) {
    pthread_mutex_destroy(&m->handle);
    free(m);
}

#endif
