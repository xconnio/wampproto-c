#include "wampproto/id_generator.h"

#include <stdlib.h>

#include "wampproto/mutex.h"

#define MAX_ID ((int64_t)1 << 53)

static IDGenerator *global_id_generator = NULL;

static int64_t next(void) {
    IDGenerator *self = global_id_generator;
    mutex_lock(self->mutex);

    if (MAX_ID == self->id) {
        self->id = 0;
    }

    self->id++;

    mutex_unlock(self->mutex);

    return self->id;
}

IDGenerator *id_generator_new(void) {
    IDGenerator *id_generator = calloc(1, sizeof(*id_generator));
    id_generator->id = 0;
    id_generator->mutex = mutex_create();
    global_id_generator = id_generator;
    id_generator->next = next;

    return id_generator;
}
