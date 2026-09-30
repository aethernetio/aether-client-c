

/*
 * ADSL parser and recursive dependency loader.
 *
 * This is a development-time tool and may allocate while building its semantic
 * document; none of those allocations are linked into client firmware.
 *
 * Parsing/include resolution belongs here. C layout and wire-emission policy
 * belongs in emitter.c.
 */
#include "adslc.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    SECTION_NONE = 0,
    SECTION_INCLUDE,
    SECTION_TYPES,
    SECTION_API
} section_t;


typedef enum {
    TYPE_BODY_NONE = 0,
    TYPE_BODY_FIELDS,
    TYPE_BODY_ENUM,
    TYPE_BODY_STREAM,
    TYPE_BODY_STREAM_APIS,
    TYPE_BODY_STREAM_KEYS
} type_body_t;


typedef enum {
    API_BODY_NONE = 0,
    API_BODY_PARENTS,
    API_BODY_METHODS,
    API_BODY_PARAMS,
    API_BODY_PARAM_OBJECT,
    API_BODY_PARAM_STREAM,
    API_BODY_PARAM_STREAM_APIS,
    API_BODY_PARAM_STREAM_KEYS,
    API_BODY_RETURN_OBJECT,
    API_BODY_RETURN_FIELDS
} api_body_t;



static void set_error(
    char *error,
    size_t capacity,
    const char *format,
    ...) {

    if (error == NULL ||
        capacity == 0u) {

        return;
    }

    va_list args;
    va_start(args, format);

    (void)vsnprintf(
        error,
        capacity,
        format,
        args);

    va_end(args);
}

static char *copy_string(
    const char *text) {

    size_t length =
        strlen(text);

    char *copy =
        malloc(length + 1u);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(
        copy,
        text,
        length + 1u);

    return copy;
}

static char *trim_left(
    char *text) {

    while (*text != '\0' &&
           isspace((unsigned char)*text)) {

        text++;
    }

    return text;
}

static void trim_right(
    char *text) {

    size_t length =
        strlen(text);

    while (length > 0u &&
           isspace(
               (unsigned char)text[length - 1u])) {

        text[length - 1u] =
            '\0';

        length--;
    }
}

static char *trim(
    char *text) {

    char *left =
        trim_left(text);

    trim_right(left);

    return left;
}

static void strip_comment(
    char *text) {

    bool single_quote =
        false;

    bool double_quote =
        false;

    for (size_t i = 0u;
         text[i] != '\0';
         ++i) {

        if (text[i] == '\'' &&
            !double_quote) {

            single_quote =
                !single_quote;

            continue;
        }

        if (text[i] == '"' &&
            !single_quote) {

            double_quote =
                !double_quote;

            continue;
        }

        if (text[i] == '#' &&
            !single_quote &&
            !double_quote) {

            text[i] =
                '\0';

            return;
        }
    }
}

static void unquote(
    char *text) {

    size_t length =
        strlen(text);

    if (length < 2u) {
        return;
    }

    bool quoted =
        (text[0] == '\'' &&
         text[length - 1u] == '\'') ||
        (text[0] == '"' &&
         text[length - 1u] == '"');

    if (!quoted) {
        return;
    }

    memmove(
        text,
        text + 1,
        length - 2u);

    text[length - 2u] =
        '\0';
}

static bool split_mapping(
    char *text,
    char **key,
    char **value) {

    char *colon =
        strchr(text, ':');

    if (colon == NULL) {
        return false;
    }

    *colon =
        '\0';

    *key =
        trim(text);

    *value =
        trim(colon + 1);

    unquote(*value);

    return **key != '\0';
}

static bool append_string(
    char ***items,
    size_t *count,
    const char *value) {

    char **next =
        realloc(
            *items,
            (*count + 1u) *
                sizeof(**items));

    if (next == NULL) {
        return false;
    }

    *items =
        next;

    next[*count] =
        copy_string(value);

    if (next[*count] == NULL) {
        return false;
    }

    (*count)++;

    return true;
}

static adsl_type_t *append_type(
    adsl_document_t *document,
    const char *name) {

    adsl_type_t *next =
        realloc(
            document->types,
            (document->type_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    document->types =
        next;

    adsl_type_t *type =
        &next[document->type_count];

    memset(
        type,
        0,
        sizeof(*type));

    type->name =
        copy_string(name);

    if (type->name == NULL) {
        return NULL;
    }

    document->type_count++;

    return type;
}

static adsl_field_t *append_field(
    adsl_type_t *type,
    const char *name,
    const char *field_type) {

    adsl_field_t *next =
        realloc(
            type->fields,
            (type->field_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    type->fields =
        next;

    adsl_field_t *field =
        &next[type->field_count];

    memset(
        field,
        0,
        sizeof(*field));

    field->name =
        copy_string(name);

    field->type =
        copy_string(field_type);

    if (field->name == NULL ||
        field->type == NULL) {

        return NULL;
    }

    type->field_count++;

    return field;
}

static adsl_api_t *append_api(
    adsl_document_t *document,
    const char *name) {

    adsl_api_t *next =
        realloc(
            document->apis,
            (document->api_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    document->apis =
        next;

    adsl_api_t *api =
        &next[document->api_count];

    memset(
        api,
        0,
        sizeof(*api));

    api->name =
        copy_string(name);

    if (api->name == NULL) {
        return NULL;
    }

    document->api_count++;

    return api;
}

static adsl_method_t *append_method(
    adsl_api_t *api,
    const char *name) {

    adsl_method_t *next =
        realloc(
            api->methods,
            (api->method_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    api->methods =
        next;

    adsl_method_t *method =
        &next[api->method_count];

    memset(
        method,
        0,
        sizeof(*method));

    method->name =
        copy_string(name);

    if (method->name == NULL) {
        return NULL;
    }

    api->method_count++;

    return method;
}

static adsl_param_t *append_param(
    adsl_method_t *method,
    const char *name,
    const char *type) {

    adsl_param_t *next =
        realloc(
            method->params,
            (method->param_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    method->params =
        next;

    adsl_param_t *param =
        &next[method->param_count];

    memset(
        param,
        0,
        sizeof(*param));

    param->name =
        copy_string(name);

    param->type =
        copy_string(type);

    if (param->name == NULL ||
        param->type == NULL) {

        return NULL;
    }

    method->param_count++;

    return param;
}


static bool parse_boolean(
    const char *value,
    bool *out) {

    if (strcmp(value, "true") == 0) {
        *out = true;
        return true;
    }

    if (strcmp(value, "false") == 0) {
        *out = false;
        return true;
    }

    return false;
}

static bool replace_owned_string(
    char **target,
    const char *value) {

    char *copy =
        copy_string(value);

    if (copy == NULL) {
        return false;
    }

    free(*target);
    *target = copy;

    return true;
}

static adsl_stream_t *ensure_stream(
    adsl_stream_t **target) {

    if (*target != NULL) {
        return *target;
    }

    *target =
        calloc(
            1u,
            sizeof(**target));

    return *target;
}

static adsl_stream_api_t *append_stream_api(
    adsl_stream_t *stream,
    const char *api) {

    adsl_stream_api_t *next =
        realloc(
            stream->apis,
            (stream->api_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    stream->apis = next;

    adsl_stream_api_t *binding =
        &stream->apis[stream->api_count];

    memset(
        binding,
        0,
        sizeof(*binding));

    binding->api =
        copy_string(api);

    if (binding->api == NULL) {
        return NULL;
    }

    stream->api_count++;

    return binding;
}

static adsl_field_t *append_stream_key(
    adsl_stream_t *stream,
    const char *name,
    const char *type) {

    adsl_field_t *next =
        realloc(
            stream->keys,
            (stream->key_count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return NULL;
    }

    stream->keys = next;

    adsl_field_t *field =
        &stream->keys[stream->key_count];

    memset(
        field,
        0,
        sizeof(*field));

    field->name =
        copy_string(name);

    field->type =
        copy_string(type);

    if (field->name == NULL ||
        field->type == NULL) {

        free(field->name);
        free(field->type);
        field->name = NULL;
        field->type = NULL;
        return NULL;
    }

    stream->key_count++;

    return field;
}

static bool parse_stream_scalar_property(
    adsl_stream_t *stream,
    const char *key,
    const char *value,
    char *error,
    size_t error_capacity,
    size_t line_number) {

    if (strcmp(key, "api") == 0) {
        if (*value == '\0' ||
            !replace_owned_string(
                &stream->api,
                value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream api",
                line_number);

            return false;
        }

        return true;
    }

    if (strcmp(key, "remoteApi") == 0) {
        if (*value == '\0' ||
            !replace_owned_string(
                &stream->remote_api,
                value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream remoteApi",
                line_number);

            return false;
        }

        return true;
    }

    if (strcmp(key, "crypto") == 0) {
        if (!parse_boolean(
                value,
                &stream->crypto)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream crypto boolean",
                line_number);

            return false;
        }

        stream->has_crypto =
            true;

        return true;
    }

    return false;
}

static void free_stream(
    adsl_stream_t *stream) {

    if (stream == NULL) {
        return;
    }

    free(stream->api);
    free(stream->remote_api);

    for (size_t i = 0u;
         i < stream->api_count;
         ++i) {

        free(stream->apis[i].api);
        free(stream->apis[i].remote_api);
    }

    free(stream->apis);

    for (size_t i = 0u;
         i < stream->key_count;
         ++i) {

        free(stream->keys[i].name);
        free(stream->keys[i].type);
    }

    free(stream->keys);
    free(stream);
}


static bool parse_u8(
    const char *value,
    uint8_t *out) {

    errno = 0;

    char *end =
        NULL;

    unsigned long parsed =
        strtoul(
            value,
            &end,
            10);

    if (errno != 0 ||
        end == value ||
        *end != '\0' ||
        parsed > 255ul) {

        return false;
    }

    *out =
        (uint8_t)parsed;

    return true;
}

static size_t leading_spaces(
    const char *line,
    bool *has_tab) {

    size_t count =
        0u;

    *has_tab =
        false;

    while (line[count] != '\0') {
        if (line[count] == ' ') {
            count++;
            continue;
        }

        if (line[count] == '\t') {
            *has_tab = true;
        }

        break;
    }

    return count;
}

static bool parse_type_body(
    adsl_type_t *type,
    type_body_t *body,
    size_t indent,
    char *text,
    char *error,
    size_t error_capacity,
    size_t line_number) {

    if (indent == 4u) {
        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: expected type property",
                line_number);

            return false;
        }

        if (strcmp(key, "fields") == 0) {
            *body = TYPE_BODY_FIELDS;
            return true;
        }

        if (strcmp(key, "enum") == 0) {
            *body = TYPE_BODY_ENUM;
            return true;
        }

        if (strcmp(key, "stream") == 0) {
            if (*value != '\0' ||
                ensure_stream(
                    &type->stream) == NULL) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid stream declaration",
                    line_number);

                return false;
            }

            *body =
                TYPE_BODY_STREAM;

            return true;
        }

        if (strcmp(key, "parent") == 0) {
            free(type->parent);
            type->parent = copy_string(value);
            return type->parent != NULL;
        }

        if (strcmp(key, "abstract") == 0) {
            if (!parse_boolean(
                    value,
                    &type->is_abstract)) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid abstract boolean",
                    line_number);

                return false;
            }

            return true;
        }

        if (strcmp(key, "id") == 0) {
            if (!parse_u8(
                    value,
                    &type->id)) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid type id",
                    line_number);

                return false;
            }

            type->has_id =
                true;

            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported type property '%s'",
            line_number,
            key);

        return false;
    }

    if (indent == 6u &&
        *body == TYPE_BODY_FIELDS) {

        char *name = NULL;
        char *field_type = NULL;

        if (!split_mapping(
                text,
                &name,
                &field_type) ||
            *field_type == '\0') {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid field",
                line_number);

            return false;
        }

        return append_field(
                   type,
                   name,
                   field_type) != NULL;
    }

    if (indent == 6u &&
        *body == TYPE_BODY_ENUM &&
        text[0] == '-') {

        char *value =
            trim(text + 1);

        unquote(value);

        return append_string(
            &type->enum_values,
            &type->enum_count,
            value);
    }

    if (type->stream != NULL &&
        indent == 6u) {

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream property",
                line_number);

            return false;
        }

        if (strcmp(key, "apis") == 0 &&
            *value == '\0') {

            *body =
                TYPE_BODY_STREAM_APIS;

            return true;
        }

        if (strcmp(key, "keys") == 0 &&
            *value == '\0') {

            *body =
                TYPE_BODY_STREAM_KEYS;

            return true;
        }

        if (parse_stream_scalar_property(
                type->stream,
                key,
                value,
                error,
                error_capacity,
                line_number)) {

            *body =
                TYPE_BODY_STREAM;

            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported stream property '%s'",
            line_number,
            key);

        return false;
    }

    if (type->stream != NULL &&
        indent == 8u &&
        *body == TYPE_BODY_STREAM_APIS &&
        text[0] == '-') {

        char *mapping =
            trim(text + 1);

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                mapping,
                &key,
                &value) ||
            strcmp(key, "api") != 0 ||
            *value == '\0' ||
            append_stream_api(
                type->stream,
                value) == NULL) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream API binding",
                line_number);

            return false;
        }

        return true;
    }

    if (type->stream != NULL &&
        indent == 10u &&
        *body == TYPE_BODY_STREAM_APIS &&
        type->stream->api_count > 0u) {

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value) ||
            strcmp(key, "remoteApi") != 0 ||
            *value == '\0' ||
            !replace_owned_string(
                &type->stream
                     ->apis[type->stream->api_count - 1u]
                     .remote_api,
                value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream remoteApi binding",
                line_number);

            return false;
        }

        return true;
    }

    if (type->stream != NULL &&
        indent == 8u &&
        *body == TYPE_BODY_STREAM_KEYS) {

        char *name = NULL;
        char *key_type = NULL;

        if (!split_mapping(
                text,
                &name,
                &key_type) ||
            *key_type == '\0' ||
            append_stream_key(
                type->stream,
                name,
                key_type) == NULL) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid stream key",
                line_number);

            return false;
        }

        return true;
    }

    set_error(
        error,
        error_capacity,
        "line %zu: unsupported type indentation",
        line_number);

    return false;
}

static bool parse_api_body(
    adsl_api_t *api,
    adsl_method_t **method,
    api_body_t *body,
    size_t indent,
    char *text,
    char *error,
    size_t error_capacity,
    size_t line_number) {

    if (indent == 4u) {
        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            return false;
        }

        if (strcmp(key, "parents") == 0) {
            *body = API_BODY_PARENTS;
            return true;
        }

        if (strcmp(key, "methods") == 0) {
            *body = API_BODY_METHODS;
            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported API property '%s'",
            line_number,
            key);

        return false;
    }

    if (indent == 6u &&
        *body == API_BODY_PARENTS &&
        text[0] == '-') {

        char *parent =
            trim(text + 1);

        unquote(parent);

        return append_string(
            &api->parents,
            &api->parent_count,
            parent);
    }

    if (indent == 6u &&
        (*body == API_BODY_METHODS ||
         *body == API_BODY_PARAMS ||
         *body == API_BODY_PARAM_OBJECT ||
         *body == API_BODY_PARAM_STREAM ||
         *body == API_BODY_PARAM_STREAM_APIS ||
         *body == API_BODY_PARAM_STREAM_KEYS ||
         *body == API_BODY_RETURN_OBJECT ||
         *body == API_BODY_RETURN_FIELDS)) {

        char *name = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &name,
                &value) ||
            *value != '\0') {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid method declaration",
                line_number);

            return false;
        }

        *body =
            API_BODY_METHODS;

        *method =
            append_method(
                api,
                name);

        return *method != NULL;
    }

    if (indent == 8u &&
        *method != NULL) {

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            return false;
        }

        if (strcmp(key, "params") == 0) {
            *body = API_BODY_PARAMS;
            return true;
        }

        if (strcmp(key, "id") == 0) {
            if (!parse_u8(
                    value,
                    &(*method)->id)) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid method id",
                    line_number);

                return false;
            }

            (*method)->has_id =
                true;

            return true;
        }

        if (strcmp(key, "returns") == 0) {
            free((*method)->returns_type);
            (*method)->returns_type = NULL;

            if (*value == '\0') {
                *body =
                    API_BODY_RETURN_OBJECT;

                return true;
            }

            (*method)->returns_type =
                copy_string(value);

            return (*method)->returns_type !=
                   NULL;
        }

        if (strcmp(key, "throws") == 0) {
            free((*method)->throws_type);

            (*method)->throws_type =
                copy_string(value);

            return (*method)->throws_type !=
                   NULL;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported method property '%s'",
            line_number,
            key);

        return false;
    }

    if (indent == 10u &&
        *method != NULL &&
        *body == API_BODY_RETURN_OBJECT) {

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value) ||
            strcmp(key, "fields") != 0 ||
            *value != '\0') {

            set_error(
                error,
                error_capacity,
                "line %zu: unsupported anonymous return property",
                line_number);

            return false;
        }

        *body =
            API_BODY_RETURN_FIELDS;

        return true;
    }

    if (indent == 12u &&
        *method != NULL &&
        *body == API_BODY_RETURN_FIELDS) {

        char *name = NULL;
        char *field_type = NULL;

        if (!split_mapping(
                text,
                &name,
                &field_type) ||
            *field_type == '\0') {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid anonymous return field",
                line_number);

            return false;
        }

        adsl_field_t *next =
            realloc(
                (*method)->return_fields,
                ((*method)->return_field_count + 1u) *
                    sizeof(*next));

        if (next == NULL) {
            set_error(
                error,
                error_capacity,
                "out of memory");

            return false;
        }

        (*method)->return_fields =
            next;

        adsl_field_t *field =
            &next[(*method)->return_field_count];

        memset(
            field,
            0,
            sizeof(*field));

        field->name =
            copy_string(name);

        field->type =
            copy_string(field_type);

        if (field->name == NULL ||
            field->type == NULL) {

            free(field->name);
            free(field->type);
            field->name = NULL;
            field->type = NULL;

            set_error(
                error,
                error_capacity,
                "out of memory");

            return false;
        }

        (*method)->return_field_count++;

        return true;
    }

    if (indent == 10u &&
        *method != NULL &&
        (*body == API_BODY_PARAMS ||
         *body == API_BODY_PARAM_OBJECT ||
         *body == API_BODY_PARAM_STREAM ||
         *body == API_BODY_PARAM_STREAM_APIS ||
         *body == API_BODY_PARAM_STREAM_KEYS)) {

        char *name = NULL;
        char *param_type = NULL;

        if (!split_mapping(
                text,
                &name,
                &param_type)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid parameter",
                line_number);

            return false;
        }

        adsl_param_t *param =
            append_param(
                *method,
                name,
                param_type);

        if (param == NULL) {
            set_error(
                error,
                error_capacity,
                "out of memory");

            return false;
        }

        *body =
            *param_type == '\0'
                ? API_BODY_PARAM_OBJECT
                : API_BODY_PARAMS;

        return true;
    }

    if (indent == 12u &&
        *method != NULL &&
        *body == API_BODY_PARAM_OBJECT &&
        (*method)->param_count > 0u) {

        adsl_param_t *param =
            &(*method)->params[
                (*method)->param_count - 1u];

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            return false;
        }

        if (strcmp(key, "name") == 0 &&
            *value != '\0') {

            if (!replace_owned_string(
                    &param->type,
                    value)) {

                set_error(
                    error,
                    error_capacity,
                    "out of memory");

                return false;
            }

            return true;
        }

        if (strcmp(key, "stream") == 0 &&
            *value == '\0') {

            if (ensure_stream(
                    &param->stream) == NULL) {

                set_error(
                    error,
                    error_capacity,
                    "out of memory");

                return false;
            }

            *body =
                API_BODY_PARAM_STREAM;

            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported parameter object property '%s'",
            line_number,
            key);

        return false;
    }

    if (indent == 14u &&
        *method != NULL &&
        (*method)->param_count > 0u &&
        (*body == API_BODY_PARAM_STREAM ||
         *body == API_BODY_PARAM_STREAM_APIS ||
         *body == API_BODY_PARAM_STREAM_KEYS)) {

        adsl_param_t *param =
            &(*method)->params[
                (*method)->param_count - 1u];

        if (param->stream == NULL) {
            return false;
        }

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            return false;
        }

        if (strcmp(key, "apis") == 0 &&
            *value == '\0') {

            *body =
                API_BODY_PARAM_STREAM_APIS;

            return true;
        }

        if (strcmp(key, "keys") == 0 &&
            *value == '\0') {

            *body =
                API_BODY_PARAM_STREAM_KEYS;

            return true;
        }

        if (parse_stream_scalar_property(
                param->stream,
                key,
                value,
                error,
                error_capacity,
                line_number)) {

            *body =
                API_BODY_PARAM_STREAM;

            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: unsupported inline stream property '%s'",
            line_number,
            key);

        return false;
    }

    if (indent == 16u &&
        *method != NULL &&
        (*method)->param_count > 0u) {

        adsl_param_t *param =
            &(*method)->params[
                (*method)->param_count - 1u];

        if (param->stream != NULL &&
            *body == API_BODY_PARAM_STREAM_APIS &&
            text[0] == '-') {

            char *mapping =
                trim(text + 1);

            char *key = NULL;
            char *value = NULL;

            if (!split_mapping(
                    mapping,
                    &key,
                    &value) ||
                strcmp(key, "api") != 0 ||
                *value == '\0' ||
                append_stream_api(
                    param->stream,
                    value) == NULL) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid inline stream API binding",
                    line_number);

                return false;
            }

            return true;
        }

        if (param->stream != NULL &&
            *body == API_BODY_PARAM_STREAM_KEYS) {

            char *name = NULL;
            char *key_type = NULL;

            if (!split_mapping(
                    text,
                    &name,
                    &key_type) ||
                *key_type == '\0' ||
                append_stream_key(
                    param->stream,
                    name,
                    key_type) == NULL) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid inline stream key",
                    line_number);

                return false;
            }

            return true;
        }
    }

    if (indent == 18u &&
        *method != NULL &&
        (*method)->param_count > 0u &&
        *body == API_BODY_PARAM_STREAM_APIS) {

        adsl_param_t *param =
            &(*method)->params[
                (*method)->param_count - 1u];

        if (param->stream == NULL ||
            param->stream->api_count == 0u) {

            return false;
        }

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value) ||
            strcmp(key, "remoteApi") != 0 ||
            *value == '\0' ||
            !replace_owned_string(
                &param->stream
                     ->apis[param->stream->api_count - 1u]
                     .remote_api,
                value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: invalid inline stream remoteApi binding",
                line_number);

            return false;
        }

        return true;
    }

    set_error(
        error,
        error_capacity,
        "line %zu: unsupported API indentation",
        line_number);

    return false;
}

void adsl_document_init(
    adsl_document_t *document) {

    if (document != NULL) {
        memset(
            document,
            0,
            sizeof(*document));
    }
}

void adsl_document_free(
    adsl_document_t *document) {

    if (document == NULL) {
        return;
    }

    free(document->name);

    for (size_t i = 0u;
         i < document->include_count;
         ++i) {

        free(document->includes[i]);
    }

    free(document->includes);

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        adsl_type_t *type =
            &document->types[i];

        free(type->name);
        free(type->parent);

        for (size_t f = 0u;
             f < type->field_count;
             ++f) {

            free(type->fields[f].name);
            free(type->fields[f].type);
        }

        free(type->fields);

        for (size_t e = 0u;
             e < type->enum_count;
             ++e) {

            free(type->enum_values[e]);
        }

        free(type->enum_values);
        free_stream(type->stream);
    }

    free(document->types);

    for (size_t i = 0u;
         i < document->api_count;
         ++i) {

        adsl_api_t *api =
            &document->apis[i];

        free(api->name);

        for (size_t p = 0u;
             p < api->parent_count;
             ++p) {

            free(api->parents[p]);
        }

        free(api->parents);

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            adsl_method_t *method =
                &api->methods[m];

            free(method->name);
            free(method->returns_type);
            free(method->throws_type);

            for (size_t f = 0u;
                 f < method->return_field_count;
                 ++f) {

                free(method->return_fields[f].name);
                free(method->return_fields[f].type);
            }

            free(method->return_fields);

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                free(method->params[p].name);
                free(method->params[p].type);
                free_stream(method->params[p].stream);
            }

            free(method->params);
        }

        free(api->methods);
    }

    free(document->apis);

    adsl_document_init(document);
}

bool adsl_parse_file(
    const char *path,
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    if (path == NULL ||
        document == NULL) {

        set_error(
            error,
            error_capacity,
            "invalid parser arguments");

        return false;
    }

    FILE *input =
        fopen(path, "rb");

    if (input == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot open '%s'",
            path);

        return false;
    }

    section_t section =
        SECTION_NONE;

    type_body_t type_body =
        TYPE_BODY_NONE;

    api_body_t api_body =
        API_BODY_NONE;

    adsl_type_t *current_type =
        NULL;

    adsl_api_t *current_api =
        NULL;

    adsl_method_t *current_method =
        NULL;

    char line[4096];
    size_t line_number = 0u;
    bool ok = true;

    while (fgets(
               line,
               sizeof(line),
               input) != NULL) {

        line_number++;

        strip_comment(line);
        trim_right(line);

        bool has_tab = false;

        size_t indent =
            leading_spaces(
                line,
                &has_tab);

        if (has_tab) {
            set_error(
                error,
                error_capacity,
                "line %zu: tabs are not supported",
                line_number);

            ok = false;
            break;
        }

        char *text =
            trim(line);

        if (*text == '\0') {
            continue;
        }

        if ((indent & 1u) != 0u) {
            set_error(
                error,
                error_capacity,
                "line %zu: indentation must use two-space levels",
                line_number);

            ok = false;
            break;
        }


if (indent == 0u) {
            current_type = NULL;
            current_api = NULL;
            current_method = NULL;
            type_body = TYPE_BODY_NONE;
            api_body = API_BODY_NONE;

            if (strcmp(text, "include:") == 0 ||
                strcmp(text, "includes:") == 0) {

                section = SECTION_INCLUDE;
                continue;
            }

            if (strcmp(text, "types:") == 0) {
                section = SECTION_TYPES;
                continue;
            }

            if (strcmp(text, "api:") == 0 ||
                strcmp(text, "services:") == 0) {

                section = SECTION_API;
                continue;
            }

            char *top_key = NULL;
            char *top_value = NULL;

            if (split_mapping(
                    text,
                    &top_key,
                    &top_value) &&
                strcmp(top_key, "name") == 0 &&
                *top_value != '\0') {

                char *name_copy =
                    copy_string(top_value);

                if (name_copy == NULL) {
                    set_error(
                        error,
                        error_capacity,
                        "out of memory");

                    ok = false;
                    break;
                }

                free(document->name);
                document->name = name_copy;

                section =
                    SECTION_NONE;

                continue;
            }

            set_error(
                error,
                error_capacity,
                "line %zu: unsupported top-level declaration '%s'",
                line_number,
                text);

            ok = false;
            break;
        }


        if (section == SECTION_INCLUDE &&
            indent == 2u &&
            text[0] == '-') {

            char *include_name =
                trim(text + 1);

            unquote(include_name);

            if (!append_string(
                    &document->includes,
                    &document->include_count,
                    include_name)) {

                set_error(
                    error,
                    error_capacity,
                    "out of memory");

                ok = false;
                break;
            }

            continue;
        }

        if (section == SECTION_TYPES &&
            indent == 2u) {

            char *name = NULL;
            char *value = NULL;

            if (!split_mapping(
                    text,
                    &name,
                    &value) ||
                *value != '\0') {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid type declaration",
                    line_number);

                ok = false;
                break;
            }

            current_type =
                append_type(
                    document,
                    name);

            type_body =
                TYPE_BODY_NONE;

            if (current_type == NULL) {
                set_error(
                    error,
                    error_capacity,
                    "out of memory");

                ok = false;
                break;
            }

            continue;
        }

        if (section == SECTION_TYPES &&
            current_type != NULL) {

            if (!parse_type_body(
                    current_type,
                    &type_body,
                    indent,
                    text,
                    error,
                    error_capacity,
                    line_number)) {

                ok = false;
                break;
            }

            continue;
        }

        if (section == SECTION_API &&
            indent == 2u) {

            char *name = NULL;
            char *value = NULL;

            if (!split_mapping(
                    text,
                    &name,
                    &value) ||
                *value != '\0') {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid API declaration",
                    line_number);

                ok = false;
                break;
            }

            current_api =
                append_api(
                    document,
                    name);

            current_method =
                NULL;

            api_body =
                API_BODY_NONE;

            if (current_api == NULL) {
                set_error(
                    error,
                    error_capacity,
                    "out of memory");

                ok = false;
                break;
            }

            continue;
        }

        if (section == SECTION_API &&
            current_api != NULL) {

            if (!parse_api_body(
                    current_api,
                    &current_method,
                    &api_body,
                    indent,
                    text,
                    error,
                    error_capacity,
                    line_number)) {

                ok = false;
                break;
            }

            continue;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: declaration outside a supported section",
            line_number);

        ok = false;
        break;
    }

    if (ferror(input)) {
        set_error(
            error,
            error_capacity,
            "I/O error while reading '%s'",
            path);

        ok = false;
    }

    fclose(input);

    return ok;
}

static bool same_name(
    const char *left,
    const char *right) {

    while (*left != '\0' &&
           *right != '\0') {

        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {

            return false;
        }

        left++;
        right++;
    }

    return *left == '\0' &&
           *right == '\0';
}

static bool primitive_type(
    const char *type_name) {

    static const char *const primitives[] = {
        "void",
        "bool",
        "boolean",
        "byte",
        "short",
        "int",
        "long",
        "intpack",
        "float",
        "double",
        "string",
        "uuid",
        "uri",
        "date"
    };

    for (size_t i = 0u;
         i < sizeof(primitives) /
                 sizeof(primitives[0]);
         ++i) {

        if (same_name(
                type_name,
                primitives[i])) {

            return true;
        }
    }

    return false;
}

static void base_type(
    const char *source,
    char *target,
    size_t capacity) {

    if (capacity == 0u) {
        return;
    }

    size_t length =
        strlen(source);

    while (length > 0u &&
           (source[length - 1u] == '?' ||
            source[length - 1u] == '~')) {

        length--;
    }

    const char *array =
        memchr(
            source,
            '[',
            length);

    if (array != NULL) {
        length =
            (size_t)(array - source);
    }

    if (length >= capacity) {
        length =
            capacity - 1u;
    }

    memcpy(
        target,
        source,
        length);

    target[length] =
        '\0';

    if (length > 9u &&
        strcmp(
            target + length - 9u,
            "(intpack)") == 0) {

        target[length - 9u] =
            '\0';
    }
}

static const adsl_type_t *find_type(
    const adsl_document_t *document,
    const char *name) {

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        if (same_name(
                document->types[i].name,
                name)) {

            return &document->types[i];
        }
    }

    return NULL;
}

static adsl_api_t *find_api_mutable(
    adsl_document_t *document,
    const char *name) {

    for (size_t i = 0u;
         i < document->api_count;
         ++i) {

        if (same_name(
                document->apis[i].name,
                name)) {

            return &document->apis[i];
        }
    }

    return NULL;
}

static bool validate_type_reference(
    const adsl_document_t *document,
    const char *reference,
    char *error,
    size_t error_capacity) {

    if (reference == NULL ||
        *reference == '\0') {

        return true;
    }

    char canonical[256];

    base_type(
        reference,
        canonical,
        sizeof(canonical));

    if (primitive_type(canonical) ||
        find_type(
            document,
            canonical) != NULL) {

        return true;
    }

    set_error(
        error,
        error_capacity,
        "unknown type reference '%s'",
        reference);

    return false;
}

static bool api_total_methods(
    adsl_document_t *document,
    adsl_api_t *api,
    size_t depth,
    size_t *count,
    char *error,
    size_t error_capacity) {

    if (depth >
        document->api_count) {

        set_error(
            error,
            error_capacity,
            "API inheritance cycle near '%s'",
            api->name);

        return false;
    }

    size_t total =
        api->method_count;

    for (size_t i = 0u;
         i < api->parent_count;
         ++i) {

        adsl_api_t *parent =
            find_api_mutable(
                document,
                api->parents[i]);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "API '%s' has unknown parent '%s'",
                api->name,
                api->parents[i]);

            return false;
        }

        size_t parent_count =
            0u;

        if (!api_total_methods(
                document,
                parent,
                depth + 1u,
                &parent_count,
                error,
                error_capacity)) {

            return false;
        }

        total +=
            parent_count;
    }

    *count =
        total;

    return true;
}

static bool assign_method_ids(
    adsl_document_t *document,
    adsl_api_t *api,
    char *error,
    size_t error_capacity) {

    size_t inherited =
        0u;

    for (size_t i = 0u;
         i < api->parent_count;
         ++i) {

        adsl_api_t *parent =
            find_api_mutable(
                document,
                api->parents[i]);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "API '%s' has unknown parent '%s'",
                api->name,
                api->parents[i]);

            return false;
        }

        size_t parent_methods =
            0u;

        if (!api_total_methods(
                document,
                parent,
                1u,
                &parent_methods,
                error,
                error_capacity)) {

            return false;
        }

        inherited +=
            parent_methods;
    }

    for (size_t i = 0u;
         i < api->method_count;
         ++i) {

        adsl_method_t *method =
            &api->methods[i];

        if (!method->has_id) {
            size_t generated =
                inherited + i + 3u;

            if (generated > 255u) {
                set_error(
                    error,
                    error_capacity,
                    "method id overflow in API '%s'",
                    api->name);

                return false;
            }

            method->id =
                (uint8_t)generated;

            method->has_id =
                true;
        }
    }

    return true;
}

bool adsl_validate(
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    if (document == NULL) {
        return false;
    }

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        for (size_t j = i + 1u;
             j < document->type_count;
             ++j) {

            if (same_name(
                    document->types[i].name,
                    document->types[j].name)) {

                set_error(
                    error,
                    error_capacity,
                    "type declarations differ only by case: '%s' and '%s'",
                    document->types[i].name,
                    document->types[j].name);

                return false;
            }
        }

        adsl_type_t *type =
            &document->types[i];

        if (type->parent != NULL &&
            find_type(
                document,
                type->parent) == NULL) {

            set_error(
                error,
                error_capacity,
                "type '%s' has unknown parent '%s'",
                type->name,
                type->parent);

            return false;
        }

        size_t nullable_count =
            0u;

        for (size_t f = 0u;
             f < type->field_count;
             ++f) {

            adsl_field_t *field =
                &type->fields[f];

            if (!validate_type_reference(
                    document,
                    field->type,
                    error,
                    error_capacity)) {

                return false;
            }

            if (strchr(
                    field->type,
                    '?') != NULL) {

                nullable_count++;
            }
        }

        if (type->stream != NULL) {
            for (size_t k = 0u;
                 k < type->stream->key_count;
                 ++k) {

                if (!validate_type_reference(
                        document,
                        type->stream->keys[k].type,
                        error,
                        error_capacity)) {

                    return false;
                }
            }
        }

        if (nullable_count > 64u) {
            set_error(
                error,
                error_capacity,
                "type '%s' has more than 64 nullable fields",
                type->name);

            return false;
        }
    }

    for (size_t i = 0u;
         i < document->api_count;
         ++i) {

        for (size_t j = i + 1u;
             j < document->api_count;
             ++j) {

            if (same_name(
                    document->apis[i].name,
                    document->apis[j].name)) {

                set_error(
                    error,
                    error_capacity,
                    "API declarations differ only by case: '%s' and '%s'",
                    document->apis[i].name,
                    document->apis[j].name);

                return false;
            }
        }

        adsl_api_t *api =
            &document->apis[i];

        if (!assign_method_ids(
                document,
                api,
                error,
                error_capacity)) {

            return false;
        }

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            adsl_method_t *method =
                &api->methods[m];

            if (method->id < 3u) {
                set_error(
                    error,
                    error_capacity,
                    "API '%s' method '%s' uses reserved id %u",
                    api->name,
                    method->name,
                    (unsigned int)method->id);

                return false;
            }

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                adsl_param_t *param =
                    &method->params[p];

                if (param->stream != NULL) {
                    continue;
                }

                if (!validate_type_reference(
                        document,
                        param->type,
                        error,
                        error_capacity)) {

                    return false;
                }
            }

            if (!validate_type_reference(
                    document,
                    method->returns_type,
                    error,
                    error_capacity) ||
                !validate_type_reference(
                    document,
                    method->throws_type,
                    error,
                    error_capacity)) {

                return false;
            }

            for (size_t f = 0u;
                 f < method->return_field_count;
                 ++f) {

                if (!validate_type_reference(
                        document,
                        method->return_fields[f].type,
                        error,
                        error_capacity)) {

                    return false;
                }
            }

            for (size_t n = m + 1u;
                 n < api->method_count;
                 ++n) {

                if (method->id ==
                    api->methods[n].id) {

                    set_error(
                        error,
                        error_capacity,
                        "API '%s' has duplicate method id %u",
                        api->name,
                        (unsigned int)method->id);

                    return false;
                }
            }
        }
    }

    return true;
}


typedef struct {
    char **paths;
    size_t count;
} adsl_load_state_t;

static void adsl_load_state_free(
    adsl_load_state_t *state) {

    if (state == NULL) {
        return;
    }

    for (size_t i = 0u;
         i < state->count;
         ++i) {

        free(state->paths[i]);
    }

    free(state->paths);

    state->paths = NULL;
    state->count = 0u;
}

static bool adsl_file_exists(
    const char *path) {

    FILE *input =
        fopen(path, "rb");

    if (input == NULL) {
        return false;
    }

    fclose(input);

    return true;
}

static bool adsl_normalize_path(
    const char *path,
    char *output,
    size_t capacity) {

    if (path == NULL ||
        output == NULL ||
        capacity == 0u) {

        return false;
    }

    char work[4096];

    size_t length =
        strlen(path);

    if (length >= sizeof(work)) {
        return false;
    }

    for (size_t i = 0u;
         i <= length;
         ++i) {

        work[i] =
            path[i] == '\\'
                ? '/'
                : path[i];
    }

    bool absolute =
        work[0] == '/';

    char *segments[512];
    size_t segment_count =
        0u;

    char *cursor =
        work;

    while (*cursor != '\0') {
        while (*cursor == '/') {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        char *segment =
            cursor;

        while (*cursor != '\0' &&
               *cursor != '/') {

            cursor++;
        }

        if (*cursor == '/') {
            *cursor =
                '\0';

            cursor++;
        }

        if (*segment == '\0' ||
            strcmp(segment, ".") == 0) {

            continue;
        }

        if (strcmp(segment, "..") == 0) {
            if (segment_count > 0u &&
                strcmp(
                    segments[segment_count - 1u],
                    "..") != 0) {

                segment_count--;
            } else if (!absolute) {
                if (segment_count >=
                    sizeof(segments) /
                        sizeof(segments[0])) {

                    return false;
                }

                segments[segment_count++] =
                    segment;
            }

            continue;
        }

        if (segment_count >=
            sizeof(segments) /
                sizeof(segments[0])) {

            return false;
        }

        segments[segment_count++] =
            segment;
    }

    size_t pos =
        0u;

    if (absolute) {
        if (capacity < 2u) {
            return false;
        }

        output[pos++] =
            '/';
    }

    for (size_t i = 0u;
         i < segment_count;
         ++i) {

        size_t segment_length =
            strlen(segments[i]);

        bool separator =
            pos > 0u &&
            output[pos - 1u] != '/';

        size_t required =
            pos +
            (separator ? 1u : 0u) +
            segment_length +
            1u;

        if (required > capacity) {
            return false;
        }

        if (separator) {
            output[pos++] =
                '/';
        }

        memcpy(
            output + pos,
            segments[i],
            segment_length);

        pos +=
            segment_length;
    }

    if (pos == 0u) {
        if (capacity < 2u) {
            return false;
        }

        output[pos++] =
            absolute
                ? '/'
                : '.';
    }

    output[pos] =
        '\0';

    return true;
}

static bool adsl_path_directory(
    const char *path,
    char *output,
    size_t capacity) {

    if (path == NULL ||
        output == NULL ||
        capacity == 0u) {

        return false;
    }

    const char *last =
        NULL;

    for (const char *p = path;
         *p != '\0';
         ++p) {

        if (*p == '/' ||
            *p == '\\') {

            last =
                p;
        }
    }

    if (last == NULL) {
        if (capacity < 2u) {
            return false;
        }

        output[0] =
            '.';

        output[1] =
            '\0';

        return true;
    }

    size_t length =
        (size_t)(last - path);

    if (length == 0u) {
        length =
            1u;
    }

    if (length + 1u >
        capacity) {

        return false;
    }

    memcpy(
        output,
        path,
        length);

    output[length] =
        '\0';

    return true;
}

static bool adsl_path_join(
    const char *left,
    const char *right,
    char *output,
    size_t capacity) {

    char joined[4096];

    int written =
        snprintf(
            joined,
            sizeof(joined),
            "%s/%s",
            left,
            right);

    if (written < 0 ||
        (size_t)written >=
            sizeof(joined)) {

        return false;
    }

    return adsl_normalize_path(
        joined,
        output,
        capacity);
}

static bool adsl_has_suffix(
    const char *text,
    const char *suffix) {

    size_t text_length =
        strlen(text);

    size_t suffix_length =
        strlen(suffix);

    return
        text_length >= suffix_length &&
        strcmp(
            text + text_length - suffix_length,
            suffix) == 0;
}

static bool adsl_include_stem(
    const char *include_name,
    char *stem,
    size_t capacity) {

    size_t length =
        strlen(include_name);

    if (adsl_has_suffix(
            include_name,
            ".adsl.yaml")) {

        length -=
            strlen(".adsl.yaml");
    } else if (adsl_has_suffix(
                   include_name,
                   ".adsl.yml")) {

        length -=
            strlen(".adsl.yml");
    }

    if (length == 0u ||
        length + 1u > capacity) {

        return false;
    }

    memcpy(
        stem,
        include_name,
        length);

    stem[length] =
        '\0';

    return true;
}

static bool adsl_include_file_name(
    const char *include_name,
    const char *suffix,
    char *output,
    size_t capacity) {

    if (adsl_has_suffix(
            include_name,
            ".adsl.yaml") ||
        adsl_has_suffix(
            include_name,
            ".adsl.yml")) {

        int written =
            snprintf(
                output,
                capacity,
                "%s",
                include_name);

        return
            written >= 0 &&
            (size_t)written < capacity;
    }

    int written =
        snprintf(
            output,
            capacity,
            "%s%s",
            include_name,
            suffix);

    return
        written >= 0 &&
        (size_t)written < capacity;
}

static bool adsl_resolve_include(
    const char *source_path,
    const char *include_name,
    const char *const *include_paths,
    size_t include_path_count,
    char *resolved,
    size_t resolved_capacity) {

    char directory[4096];

    if (!adsl_path_directory(
            source_path,
            directory,
            sizeof(directory))) {

        return false;
    }

    char stem[1024];

    if (!adsl_include_stem(
            include_name,
            stem,
            sizeof(stem))) {

        return false;
    }

    static const char *const suffixes[] = {
        ".adsl.yaml",
        ".adsl.yml"
    };

    for (size_t i = 0u;
         i < sizeof(suffixes) /
                 sizeof(suffixes[0]);
         ++i) {

        char file_name[1024];

        if (!adsl_include_file_name(
                include_name,
                suffixes[i],
                file_name,
                sizeof(file_name))) {

            continue;
        }

        char candidate[4096];

        if (adsl_path_join(
                directory,
                file_name,
                candidate,
                sizeof(candidate)) &&
            adsl_file_exists(candidate)) {

            return adsl_normalize_path(
                candidate,
                resolved,
                resolved_capacity);
        }

        char sibling[2048];

        int written =
            snprintf(
                sibling,
                sizeof(sibling),
                "../%s/%s",
                stem,
                file_name);

        if (written >= 0 &&
            (size_t)written <
                sizeof(sibling) &&
            adsl_path_join(
                directory,
                sibling,
                candidate,
                sizeof(candidate)) &&
            adsl_file_exists(candidate)) {

            return adsl_normalize_path(
                candidate,
                resolved,
                resolved_capacity);
        }

        for (size_t p = 0u;
             p < include_path_count;
             ++p) {

            if (include_paths == NULL ||
                include_paths[p] == NULL ||
                *include_paths[p] == '\0') {

                continue;
            }

            if (adsl_path_join(
                    include_paths[p],
                    file_name,
                    candidate,
                    sizeof(candidate)) &&
                adsl_file_exists(candidate)) {

                return adsl_normalize_path(
                    candidate,
                    resolved,
                    resolved_capacity);
            }
        }
    }

    return false;
}

static bool adsl_load_state_contains(
    const adsl_load_state_t *state,
    const char *path) {

    for (size_t i = 0u;
         i < state->count;
         ++i) {

        if (strcmp(
                state->paths[i],
                path) == 0) {

            return true;
        }
    }

    return false;
}

static bool adsl_load_state_add(
    adsl_load_state_t *state,
    const char *path) {

    char **next =
        realloc(
            state->paths,
            (state->count + 1u) *
                sizeof(*next));

    if (next == NULL) {
        return false;
    }

    state->paths =
        next;

    next[state->count] =
        copy_string(path);

    if (next[state->count] == NULL) {
        return false;
    }

    state->count++;

    return true;
}

static bool adsl_move_types(
    adsl_document_t *destination,
    adsl_document_t *source) {

    if (source->type_count == 0u) {
        return true;
    }

    size_t old_count =
        destination->type_count;

    size_t new_count =
        old_count +
        source->type_count;

    adsl_type_t *next =
        realloc(
            destination->types,
            new_count *
                sizeof(*next));

    if (next == NULL) {
        return false;
    }

    destination->types =
        next;

    memcpy(
        destination->types +
            old_count,
        source->types,
        source->type_count *
            sizeof(*source->types));

    destination->type_count =
        new_count;

    free(source->types);

    source->types =
        NULL;

    source->type_count =
        0u;

    return true;
}

static bool adsl_move_apis(
    adsl_document_t *destination,
    adsl_document_t *source) {

    if (source->api_count == 0u) {
        return true;
    }

    size_t old_count =
        destination->api_count;

    size_t new_count =
        old_count +
        source->api_count;

    adsl_api_t *next =
        realloc(
            destination->apis,
            new_count *
                sizeof(*next));

    if (next == NULL) {
        return false;
    }

    destination->apis =
        next;

    memcpy(
        destination->apis +
            old_count,
        source->apis,
        source->api_count *
            sizeof(*source->apis));

    destination->api_count =
        new_count;

    free(source->apis);

    source->apis =
        NULL;

    source->api_count =
        0u;

    return true;
}

static bool adsl_merge_loaded_document(
    adsl_document_t *destination,
    adsl_document_t *source) {

    return
        adsl_move_types(
            destination,
            source) &&
        adsl_move_apis(
            destination,
            source);
}

static bool adsl_load_recursive(
    const char *path,
    const char *const *include_paths,
    size_t include_path_count,
    adsl_document_t *destination,
    adsl_load_state_t *state,
    size_t depth,
    char *error,
    size_t error_capacity) {

    if (depth > 128u) {
        set_error(
            error,
            error_capacity,
            "include nesting is too deep near '%s'",
            path);

        return false;
    }

    char normalized[4096];

    if (!adsl_normalize_path(
            path,
            normalized,
            sizeof(normalized))) {

        set_error(
            error,
            error_capacity,
            "cannot normalize path '%s'",
            path);

        return false;
    }

    if (adsl_load_state_contains(
            state,
            normalized)) {

        return true;
    }

    if (!adsl_load_state_add(
            state,
            normalized)) {

        set_error(
            error,
            error_capacity,
            "out of memory while tracking includes");

        return false;
    }

    adsl_document_t current;
    adsl_document_init(&current);

    if (!adsl_parse_file(
            normalized,
            &current,
            error,
            error_capacity)) {

        adsl_document_free(&current);
        return false;
    }

    for (size_t i = 0u;
         i < current.include_count;
         ++i) {

        char include_path[4096];

        if (!adsl_resolve_include(
                normalized,
                current.includes[i],
                include_paths,
                include_path_count,
                include_path,
                sizeof(include_path))) {

            set_error(
                error,
                error_capacity,
                "cannot resolve include '%s' from '%s'",
                current.includes[i],
                normalized);

            adsl_document_free(&current);
            return false;
        }

        if (!adsl_load_recursive(
                include_path,
                include_paths,
                include_path_count,
                destination,
                state,
                depth + 1u,
                error,
                error_capacity)) {

            adsl_document_free(&current);
            return false;
        }
    }

    if (!adsl_merge_loaded_document(
            destination,
            &current)) {

        set_error(
            error,
            error_capacity,
            "out of memory while merging '%s'",
            normalized);

        adsl_document_free(&current);
        return false;
    }

    adsl_document_free(&current);

    return true;
}

bool adsl_load_file(
    const char *path,
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    return adsl_load_file_with_include_paths(
        path,
        NULL,
        0u,
        document,
        error,
        error_capacity);
}


bool adsl_load_file_with_include_paths(
    const char *path,
    const char *const *include_paths,
    size_t include_path_count,
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    if (path == NULL ||
        document == NULL ||
        (include_path_count > 0u &&
         include_paths == NULL)) {

        set_error(
            error,
            error_capacity,
            "invalid loader arguments");

        return false;
    }

    adsl_load_state_t state = {
        NULL,
        0u
    };

    bool loaded =
        adsl_load_recursive(
            path,
            include_paths,
            include_path_count,
            document,
            &state,
            0u,
            error,
            error_capacity);

    adsl_load_state_free(&state);

    return loaded;
}