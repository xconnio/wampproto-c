#ifndef MUTEX_H
#define MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Mutex Mutex;

Mutex *mutex_create(void);
void mutex_lock(Mutex *m);
void mutex_unlock(Mutex *m);
void mutex_destroy(Mutex *m);

#ifdef __cplusplus
}
#endif

#endif  // MUTEX_H
