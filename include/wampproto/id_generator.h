#ifndef WAMPPROTO_ID_GENERATOR_H
#define WAMPPROTO_ID_GENERATOR_H

#include <stdint.h>

#include "wampproto/mutex.h"

typedef struct IDGenerator IDGenerator;

typedef struct IDGenerator {
    int64_t id;
    int64_t (*next)(void);
    Mutex *mutex;
} IDGenerator;

IDGenerator *id_generator_new(void);

#endif
