#include "wampproto/messages/result.h"

#include <stdlib.h>

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

static List* result_marshal(const Message* self) {
    Result* result = (Result*)self;

    int length = 3;
    if (result->args != NULL) length = 4;
    if (result->kwargs != NULL) length = 5;

    Value* list = value_list(length);
    value_list_append(list, value_int(MESSAGE_TYPE_RESULT));
    value_list_append(list, value_int(result->request_id));
    value_list_append(list, value_from_dict(result->details));

    List* args = NULL;
    if (result->args != NULL) value_list_append(list, value_from_list(result->args));

    if (result->kwargs != NULL) {
        if (result->args == NULL) value_list_append(list, value_list(0));
        value_list_append(list, value_from_dict(result->kwargs));
    }

    return (List*)list;
}

static void result_free(Message* self) {
    Result* result = (Result*)self;

    value_free(value_from_dict(result->details));
    if (result->args != NULL) value_free(value_from_list(result->args));
    if (result->kwargs != NULL) value_free(value_from_dict(result->kwargs));

    free(result);
}

Result* result_new(int64_t request_id, Dict* details, List* args, Dict* kwargs) {
    Result* result = calloc(1, sizeof(*result));

    result->base.message_type = MESSAGE_TYPE_RESULT;
    result->base.marshal = result_marshal;
    result->base.free = result_free;
    result->base.parse = result_parse;

    result->request_id = request_id;
    result->details = details;
    result->args = args;
    result->kwargs = kwargs;

    return result;
}

Message* result_parse(const List* val) {
    if (!val || val->len < 3) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict* details = value_as_dict(val->items[2]);

    List* args = NULL;
    if (val->len == 4) args = value_as_list(val->items[3]);

    Dict* kwargs = NULL;
    if (val->len == 5) {
        if (args == NULL) args = value_as_list(val->items[3]);

        kwargs = value_as_dict(val->items[4]);
    }

    return (Message*)result_new(request_id, details, args, kwargs);
}
