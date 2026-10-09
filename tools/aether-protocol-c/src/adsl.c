

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

#include <dirent.h>
#include <sys/stat.h>



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

    TYPE_BODY_TIERED_INT,
    TYPE_BODY_TIERED_INT_LIMITS,

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



static bool parse_u32(
    const char *value,
    uint32_t *out) {

    if (value == NULL ||
        out == NULL ||
        value[0] == '-') {

        return false;
    }

    errno = 0;

    char *end = NULL;

    unsigned long parsed =
        strtoul(
            value,
            &end,
            10);

    if (errno != 0 ||
        end == value ||
        *end != '\0' ||
        parsed > UINT32_MAX) {

        return false;
    }

    *out = (uint32_t)parsed;
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


        if (strcmp(key, "tieredInt") == 0) {
            if (*value != '\0') {
                set_error(
                    error,
                    error_capacity,
                    "line %zu: tieredInt definition must be a map",
                    line_number);

                return false;
            }

            type->has_tiered_int = true;
            *body = TYPE_BODY_TIERED_INT;
            return true;
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
        (*body == TYPE_BODY_TIERED_INT ||
         *body == TYPE_BODY_TIERED_INT_LIMITS)) {

        char *key = NULL;
        char *value = NULL;

        if (!split_mapping(
                text,
                &key,
                &value)) {

            set_error(
                error,
                error_capacity,
                "line %zu: expected tieredInt property",
                line_number);

            return false;
        }

        if (strcmp(key, "minTierBytes") == 0) {
            if (!parse_u8(
                    value,
                    &type->tiered_int_min_bytes)) {

                set_error(
                    error,
                    error_capacity,
                    "line %zu: tieredInt minTierBytes must be an integer",
                    line_number);

                return false;
            }

            *body = TYPE_BODY_TIERED_INT;
            return true;
        }

        if (strcmp(key, "limits") == 0) {
            if (*value != '\0') {
                set_error(
                    error,
                    error_capacity,
                    "line %zu: tieredInt limits must use a YAML list",
                    line_number);

                return false;
            }

            *body = TYPE_BODY_TIERED_INT_LIMITS;
            return true;
        }

        set_error(
            error,
            error_capacity,
            "line %zu: tieredInt supports only minTierBytes and limits",
            line_number);

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


    if (indent == 8u &&
        *body == TYPE_BODY_TIERED_INT_LIMITS &&
        text[0] == '-') {

        if (type->tiered_int_limit_count >=
            sizeof(type->tiered_int_limits) /
                sizeof(type->tiered_int_limits[0])) {

            set_error(
                error,
                error_capacity,
                "line %zu: tieredInt configuration exceeds the canonical four-tier / 8-byte wire limit",
                line_number);

            return false;
        }

        char *limit_text =
            trim(text + 1);

        uint32_t limit = 0u;

        if (!parse_u32(
                limit_text,
                &limit)) {

            set_error(
                error,
                error_capacity,
                "line %zu: tieredInt limits must be uint32 integers",
                line_number);

            return false;
        }

        type->tiered_int_limits[
            type->tiered_int_limit_count++] =
                limit;

        return true;
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


        /*
         * Flow-style parents list: `parents: [ A, B ]`. The block form
         * `parents:` followed by `- A` lines is handled below. Both forms
         * are used by production schemas, so accept both here.
         */
        if (strcmp(key, "parents") == 0 &&
            value[0] == '[') {

            char *cursor = value + 1;

            while (*cursor != '\0' && *cursor != ']') {

                while (*cursor == ' ' || *cursor == '\t' ||
                       *cursor == ',') {
                    cursor++;
                }

                if (*cursor == ']' || *cursor == '\0') {
                    break;
                }

                char *start = cursor;

                while (*cursor != '\0' && *cursor != ',' &&
                       *cursor != ']') {
                    cursor++;
                }

                char saved = *cursor;
                *cursor = '\0';

                char *name = trim(start);
                unquote(name);

                if (*name != '\0' &&
                    !append_string(
                        &api->parents,
                        &api->parent_count,
                        name)) {
                    return false;
                }

                *cursor = saved;
            }

            *body = API_BODY_PARENTS;
            return true;
        }

        if (strcmp(key, "parents") == 0) {
            *body = API_BODY_PARENTS;
            return true;
        }



        if (strcmp(key, "methods") == 0) {
            *body = API_BODY_METHODS;
            return true;
        }

        if (strcmp(key, "abstract") == 0) {
            bool parsed = false;
            if (!parse_boolean(value, &parsed)) {
                set_error(
                    error,
                    error_capacity,
                    "line %zu: invalid API abstract flag",
                    line_number);

                return false;
            }

            api->is_abstract = parsed;
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


        if (*method != NULL) {
            (*method)->source_path =
                copy_string(api->source_path);
            (*method)->source_line =
                line_number;

            if ((*method)->source_path == NULL) {
                return false;
            }
        }


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



static void free_type_fields(
    adsl_type_t *type) {

    free(type->name);
    free(type->source_path);
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

    memset(type, 0, sizeof(*type));
}


static void free_api_fields(
    adsl_api_t *api) {

    free(api->name);
    free(api->source_path);

    for (size_t p = 0u;
         p < api->parent_count;
         ++p) {

        free(api->parents[p]);
    }

    free(api->parents);

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        adsl_method_t *method = &api->methods[m];

        free(method->name);
        free(method->source_path);
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

    memset(api, 0, sizeof(*api));
}



static void free_type_array(
    adsl_type_t *types,
    size_t count) {

    if (types == NULL) {
        return;
    }

    for (size_t i = 0u; i < count; ++i) {
        free_type_fields(&types[i]);
    }

    free(types);
}


static void free_api_array(
    adsl_api_t *apis,
    size_t count) {

    if (apis == NULL) {
        return;
    }

    for (size_t i = 0u; i < count; ++i) {
        free_api_fields(&apis[i]);
    }

    free(apis);
}


void adsl_document_free(
    adsl_document_t *document) {

    if (document == NULL) {
        return;
    }

    free(document->root_source_path);
    free(document->name);

    for (size_t i = 0u;
         i < document->include_count;
         ++i) {
        free(document->includes[i]);
    }

    free(document->includes);

    free_type_array(
        document->types,
        document->type_count);

    free_api_array(
        document->apis,
        document->api_count);


    /*
     * resolved arrays are deep copies of the include closure plus the
     * root declarations, in that order. They own their contents and are
     * released with the same helpers used for the root arrays.
     */
    free_type_array(
        document->resolved_types,
        document->resolved_type_count);

    free_api_array(
        document->resolved_apis,
        document->resolved_api_count);


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


            current_type->source_path =
                copy_string(path);
            current_type->source_line =
                line_number;

            if (current_type->source_path == NULL) {
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


            current_api->source_path =
                copy_string(path);
            current_api->source_line =
                line_number;

            if (current_api->source_path == NULL) {
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

    if (left == NULL || right == NULL) {
        return left == right;
    }

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
         i < document->resolved_type_count;
         ++i) {

        if (same_name(
                document->resolved_types[i].name,
                name)) {

            return &document->resolved_types[i];
        }
    }

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


/*
 * Validate collapsible METHOD key shapes whose Java CollapsibleKey
 * Objects.equals semantics have an exact representation in the current C ABI.
 *
 * Do not silently substitute byte equality for java.net.URI.equals, content
 * equality for Java array identity, or raw-struct comparison for generated
 * DTO equals semantics.
 */
static bool validate_collapsible_key_parameter(
    const adsl_document_t *document,
    const adsl_param_t *param,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    if (param == NULL) {
        return true;
    }

    if (param->stream != NULL) {
        set_error(
            error,
            error_capacity,
            "collapsible API method '%s' stream key cannot reproduce Java Objects.equals semantics in C",
            method_name);
        return false;
    }

    if (param->type == NULL) {
        return true;
    }

    if (strchr(param->type, '[') != NULL) {
        set_error(
            error,
            error_capacity,
            "collapsible API method '%s' array key cannot reproduce Java array identity semantics in C",
            method_name);
        return false;
    }

    char canonical[256];
    base_type(
        param->type,
        canonical,
        sizeof(canonical));

    if (same_name(canonical, "uri")) {
        set_error(
            error,
            error_capacity,
            "collapsible API method '%s' URI key cannot reproduce java.net.URI.equals semantics in C",
            method_name);
        return false;
    }

    if (primitive_type(canonical)) {
        if (same_name(canonical, "void")) {
            set_error(
                error,
                error_capacity,
                "collapsible API method '%s' has invalid void key parameter",
                method_name);
            return false;
        }

        return true;
    }

    const adsl_type_t *declared =
        find_type(document, canonical);

    if (declared != NULL &&
        (declared->enum_count > 0u ||
         declared->has_tiered_int)) {

        return true;
    }

    set_error(
        error,
        error_capacity,
        "collapsible API method '%s' complex key type '%s' cannot reproduce Java Objects.equals semantics in C",
        method_name,
        canonical);

    return false;
}



static const adsl_api_t *find_api_const(
    const adsl_document_t *document,
    const char *name) {

    for (size_t i = 0u;
         i < document->resolved_api_count;
         ++i) {

        if (same_name(
                document->resolved_apis[i].name,
                name)) {

            return &document->resolved_apis[i];
        }
    }

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



bool adsl_collect_effective_methods(
    const adsl_document_t *document,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,
    size_t error_capacity);








static bool effective_append(
    adsl_effective_method_t *methods,
    size_t *count,
    const adsl_api_t *owner,
    const adsl_method_t *method,
    uint8_t id) {

    methods[*count].owner = owner;
    methods[*count].method = method;
    methods[*count].id = id;
    *count += 1u;

    return true;
}



static bool effective_collect_recursive(
    const adsl_document_t *document,
    const adsl_api_t *api,
    size_t depth,
    adsl_effective_method_t *methods,
    size_t *count,
    char *error,
    size_t error_capacity) {

if (depth > document->resolved_api_count) {

        set_error(
            error,
            error_capacity,
            "API inheritance cycle near '%s'",
            api->name);

        return false;
    }

    for (size_t i = 0u; i < api->parent_count; ++i) {

const adsl_api_t *parent =
            find_api_const(document, api->parents[i]);

        if (parent == NULL) {

            set_error(
                error,
                error_capacity,
                "API '%s' has unknown parent '%s'",
                api->name,
                api->parents[i]);

            return false;
        }

        if (!effective_collect_recursive(
                document,
                parent,
                depth + 1u,
                methods,
                count,
                error,
                error_capacity)) {

            return false;
        }
    }


    for (size_t m = 0u; m < api->method_count; ++m) {

        const adsl_method_t *method = &api->methods[m];

        /*
         * Java ApiGenerator.getAllMethods() deduplicates by method NAME
         * only; the first effective method with a given name wins. Keep
         * this identical so automatic command IDs match Java exactly.
         */
        bool duplicate = false;

        for (size_t e = 0u; e < *count; ++e) {

            if (strcmp(methods[e].method->name,
                       method->name) == 0) {

                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            continue;
        }


        /*
         * Contextual command id, computed during this single parent-first
         * pass exactly like Java:
         *
         *   explicit id  -> that id
         *   otherwise    -> current effective count + 3
         *
         * The automatic id therefore depends on the effective API being
         * generated, not on the declaring parent. It must never be written
         * back into method->id.
         */
        size_t generated =
            method->has_id
                ? (size_t)method->id
                : (*count + 3u);

        if (generated > 255u) {

            set_error(
                error,
                error_capacity,
                "method id overflow in API '%s'",
                api->name);

            return false;
        }

        effective_append(
            methods,
            count,
            api,
            method,
            (uint8_t)generated);

    }


    return true;
}


bool adsl_collect_effective_methods(
    const adsl_document_t *document,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,
    size_t error_capacity) {

    if (out_methods == NULL || out_count == NULL) {

        set_error(error, error_capacity, "invalid effective method collector arguments");
        return false;
    }
*out_methods = NULL;
    *out_count = 0u;

    if (document == NULL || api == NULL) {

        set_error(error, error_capacity, "selected API not found");
        return false;
    }


/*
     * Upper bound: total methods declared across every resolved API.
     * Parent-first traversal that deduplicates by name cannot exceed this.
     */
    size_t capacity = 0u;

    for (size_t i = 0u; i < document->resolved_api_count; ++i) {

        capacity += document->resolved_apis[i].method_count;
    }

    if (capacity == 0u) {

        return true;
    }


    adsl_effective_method_t *methods =
        (adsl_effective_method_t *)calloc(
            capacity,
            sizeof(adsl_effective_method_t));

    if (methods == NULL) {

        set_error(error, error_capacity, "out of memory collecting effective methods");
        return false;
    }

    size_t count = 0u;

    if (!effective_collect_recursive(
            document,
            api,
            0u,
            methods,
            &count,
            error,
            error_capacity)) {

        free(methods);
        return false;
    }


    /*
     * Effective command id collision is a hard error. Contextual ids
     * include explicit ids and automatic ids computed from the effective
     * list, so this catches parent vs parent, parent vs child, and any
     * duplicate explicit id.
     */
    for (size_t i = 0u; i < count; ++i) {

        for (size_t j = i + 1u; j < count; ++j) {

            if (methods[i].id == methods[j].id) {

                set_error(
                    error,
                    error_capacity,
                    "API '%s' has duplicate effective method id %u ('%s' vs '%s')",
                    api->name,
                    (unsigned int)methods[i].id,
                    methods[i].method->name,
                    methods[j].method->name);

                free(methods);
                return false;
            }
        }
    }


    *out_methods = methods;
    *out_count = count;

    return true;
}



static bool validate_tiered_int(
    const adsl_type_t *type,
    char *error,
    size_t error_capacity) {

    if (type == NULL ||
        !type->has_tiered_int) {

        return true;
    }

    if (type->field_count != 0u ||
        type->enum_count != 0u ||
        type->stream != NULL ||
        type->parent != NULL ||
        type->is_abstract ||
        type->has_id) {

        set_error(
            error,
            error_capacity,
            "type '%s' tieredInt declaration cannot contain structure or other type-definition keys",
            type->name);

        return false;
    }

    const uint8_t base_bytes =
        type->tiered_int_min_bytes;

    if (base_bytes != 1u &&
        base_bytes != 2u &&
        base_bytes != 4u) {

        set_error(
            error,
            error_capacity,
            "type '%s' tieredInt minTierBytes must be 1, 2, or 4",
            type->name);

        return false;
    }

    const size_t limit_count =
        type->tiered_int_limit_count;

    if (limit_count == 0u) {
        set_error(
            error,
            error_capacity,
            "type '%s' tieredInt limits must be a non-empty array of integers",
            type->name);

        return false;
    }

    if (limit_count > 3u ||
        ((size_t)base_bytes << limit_count) > 8u) {

        set_error(
            error,
            error_capacity,
            "type '%s' tieredInt configuration exceeds the canonical four-tier / 8-byte wire limit",
            type->name);

        return false;
    }

    for (size_t i = 1u;
         i < limit_count;
         ++i) {

        if (type->tiered_int_limits[i] <=
            type->tiered_int_limits[i - 1u]) {

            set_error(
                error,
                error_capacity,
                "type '%s' tieredInt limits must be strictly increasing",
                type->name);

            return false;
        }
    }

    const uint64_t word =
        UINT64_C(1) <<
        (8u * base_bytes);

    const uint64_t header_max =
        word - UINT64_C(1);

    if ((uint64_t)type->tiered_int_limits[0] >=
        header_max) {

        set_error(
            error,
            error_capacity,
            "type '%s' tieredInt first limit must leave extension header space",
            type->name);

        return false;
    }

    uint64_t span =
        header_max -
        (uint64_t)type->tiered_int_limits[0] -
        UINT64_C(1);

    uint64_t tail =
        (uint64_t)type->tiered_int_limits[0] +
        UINT64_C(1) +
        header_max;

    uint64_t max_encodable =
        span != 0u &&
        word > (UINT64_MAX - tail) / span
            ? UINT64_MAX
            : span * word + tail;

    for (size_t i = 1u;
         i < limit_count;
         ++i) {

        const uint64_t limit =
            (uint64_t)type->tiered_int_limits[i];

        if (limit > max_encodable) {
            set_error(
                error,
                error_capacity,
                "type '%s' tieredInt limit %u cannot be represented by the previous wire tier",
                type->name,
                (unsigned int)type->tiered_int_limits[i]);

            return false;
        }

        const unsigned int low_bytes =
            (unsigned int)base_bytes << i;

        const uint64_t modulus =
            UINT64_C(1) <<
            (8u * low_bytes);

        span =
            max_encodable -
            limit -
            UINT64_C(1);

        tail =
            limit +
            UINT64_C(1) +
            modulus -
            UINT64_C(1);

        max_encodable =
            span != 0u &&
            modulus >
                (UINT64_MAX - tail) / span
                    ? UINT64_MAX
                    : span * modulus + tail;
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
i < document->resolved_type_count;
         ++i) {


        for (size_t j = i + 1u;
             j < document->resolved_type_count;
             ++j) {


            if (same_name(
                    document->resolved_types[i].name,
                    document->resolved_types[j].name)) {

                set_error(
                    error,
                    error_capacity,
                    "type declarations differ only by case: '%s' and '%s'",
                    document->resolved_types[i].name,
                    document->resolved_types[j].name);

                return false;
            }
        }

        adsl_type_t *type =
&document->resolved_types[i];


        if (type->has_tiered_int) {
            if (!validate_tiered_int(
                    type,
                    error,
                    error_capacity)) {

                return false;
            }

            continue;
        }



        if (type->parent != NULL) {
            const adsl_type_t *parent_type =
                find_type(
                    document,
                    type->parent);

            if (parent_type == NULL) {
                set_error(
                    error,
                    error_capacity,
                    "type '%s' has unknown parent '%s'",
                    type->name,
                    type->parent);

                return false;
            }

            /*
             * tieredInt is a named scalar, not a structural inheritance node.
             * Allowing a struct to inherit from it would make the semantic
             * model disagree with the generated uint64_t ABI.
             */
            if (parent_type->has_tiered_int) {
                set_error(
                    error,
                    error_capacity,
                    "type '%s' cannot inherit from scalar tieredInt type '%s'",
                    type->name,
                    type->parent);

                return false;
            }
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

         i < document->resolved_api_count;
         ++i) {



        for (size_t j = i + 1u;
             j < document->resolved_api_count;
             ++j) {


            if (same_name(
                    document->resolved_apis[i].name,
                    document->resolved_apis[j].name)) {

                set_error(
                    error,
                    error_capacity,
                    "API declarations differ only by case: '%s' and '%s'",
                    document->resolved_apis[i].name,
                    document->resolved_apis[j].name);

                return false;
            }
        }


        adsl_api_t *api =
            &document->resolved_apis[i];


        /*
         * Declaration validation: EVERY method declared directly on this
         * API must pass parameter/returns/throws/anonymous-result type
         * validation, regardless of whether it will later be hidden by
         * inheritance name dedup. The Java Validator also validates all
         * declared methods, so a shadowed bad declaration is still an error.
         *
         * This phase deliberately does NOT assign automatic command ids and
         * does NOT compute the effective set.
         */

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            size_t collapsible_param_count =
                0u;

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                const adsl_param_t *param =
                    &method->params[p];

                if (param->type != NULL) {
                    size_t type_length =
                        strlen(param->type);

                    if (type_length > 0u &&
                        param->type[type_length - 1u] == '~') {

                        collapsible_param_count++;
                    }
                }

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

            if (collapsible_param_count > 0u) {
                if (method->returns_type != NULL ||
                    method->return_field_count > 0u ||
                    method->throws_type != NULL) {

                    set_error(
                        error,
                        error_capacity,
                        "collapsible API method '%s' cannot declare returns or throws",
                        method->name);

                    return false;
                }

                if (collapsible_param_count ==
                    method->param_count) {

                    set_error(
                        error,
                        error_capacity,
                        "collapsible API method '%s' requires at least one non-collapsible key parameter",
                        method->name);

                    return false;
                }
                for (size_t p = 0u;
                     p < method->param_count;
                     ++p) {

                    const adsl_param_t *param =
                        &method->params[p];

                    if (param->type != NULL) {
                        size_t type_length =
                            strlen(param->type);

                        if (type_length > 0u &&
                            param->type[type_length - 1u] == '~') {

                            continue;
                        }
                    }

                    if (!validate_collapsible_key_parameter(
                            document,
                            param,
                            method->name,
                            error,
                            error_capacity)) {

                        return false;
                    }
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
        }

        /*
         * Effective-method resolution and effective-command-id validation
         * are a separate phase. adsl_collect_effective_methods() implements
         * Java getAllMethods() semantics: parent-first, first-name-wins,
         * contextual automatic ids. The collector already rejects explicit
         * id collisions in the effective set.
         */
        {
            adsl_effective_method_t *effective = NULL;
            size_t effective_count = 0u;

            if (!adsl_collect_effective_methods(
                    document,
                    api,
                    &effective,
                    &effective_count,
                    error,
                    error_capacity)) {

                return false;
            }

            for (size_t m = 0u; m < effective_count; ++m) {

                if (effective[m].id < 3u) {
                    set_error(
                        error,
                        error_capacity,
                        "API '%s' method '%s' uses reserved id %u",
                        api->name,
                        effective[m].method->name,
                        (unsigned int)effective[m].id);

                    free(effective);
                    return false;
                }
            }

            free(effective);
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









static void adsl_remove_resolved_type_at(
    adsl_document_t *document,
    size_t index) {

    free_type_fields(&document->resolved_types[index]);

    memmove(
        &document->resolved_types[index],
        &document->resolved_types[index + 1u],
        (document->resolved_type_count - index - 1u) *
            sizeof(document->resolved_types[0]));

    document->resolved_type_count--;
}


static void adsl_remove_resolved_api_at(
    adsl_document_t *document,
    size_t index) {

    free_api_fields(&document->resolved_apis[index]);

    memmove(
        &document->resolved_apis[index],
        &document->resolved_apis[index + 1u],
        (document->resolved_api_count - index - 1u) *
            sizeof(document->resolved_apis[0]));

    document->resolved_api_count--;
}





static bool adsl_same_type_definition(
    const adsl_type_t *left,
    const adsl_type_t *right);

static bool adsl_same_api_definition(
    const adsl_api_t *left,
    const adsl_api_t *right);


static bool adsl_validate_unique_declarations(
    const adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    for (size_t i = 0u;
         i < document->resolved_type_count;
         ++i) {

        for (size_t j = i + 1u;
             j < document->resolved_type_count;
             ++j) {

            if (same_name(
document->resolved_types[i].name,
                     document->resolved_types[j].name) &&
                !adsl_same_type_definition(
                    &document->resolved_types[i],
                    &document->resolved_types[j])) {

                set_error(
                    error,
                    error_capacity,
                    "conflicting type declaration '%s'",
                    document->resolved_types[i].name);

                return false;
            }
        }
    }

    for (size_t i = 0u;
         i < document->resolved_api_count;
         ++i) {

        for (size_t j = i + 1u;
             j < document->resolved_api_count;
             ++j) {

            if (same_name(
document->resolved_apis[i].name,
                     document->resolved_apis[j].name) &&
                !adsl_same_api_definition(
                    &document->resolved_apis[i],
                    &document->resolved_apis[j])) {

                set_error(
                    error,
                    error_capacity,
                    "conflicting api declaration '%s'",
                    document->resolved_apis[i].name);

                return false;
            }
        }
    }

    return true;
}



static bool adsl_dup_string(
    const char *source,
    char **destination) {

    *destination = NULL;

    if (source == NULL) {
        return true;
    }

    *destination = copy_string(source);

    return *destination != NULL;
}


static bool adsl_dup_stream(
    const adsl_stream_t *source,
    adsl_stream_t **destination) {

    *destination = NULL;

    if (source == NULL) {
        return true;
    }

    adsl_stream_t *stream =
        calloc(1u, sizeof(*stream));

    if (stream == NULL) {
        return false;
    }

    stream->has_crypto = source->has_crypto;
    stream->crypto = source->crypto;

    if (!adsl_dup_string(source->api, &stream->api) ||
        !adsl_dup_string(source->remote_api, &stream->remote_api)) {

        free_stream(stream);
        return false;
    }

    if (source->api_count > 0u) {

        stream->apis =
            calloc(source->api_count, sizeof(*stream->apis));

        if (stream->apis == NULL) {
            free_stream(stream);
            return false;
        }

        stream->api_count = source->api_count;

        for (size_t i = 0u; i < source->api_count; ++i) {

            if (!adsl_dup_string(
                    source->apis[i].api,
                    &stream->apis[i].api) ||
                !adsl_dup_string(
                    source->apis[i].remote_api,
                    &stream->apis[i].remote_api)) {

                free_stream(stream);
                return false;
            }
        }
    }

    if (source->key_count > 0u) {

        stream->keys =
            calloc(source->key_count, sizeof(*stream->keys));

        if (stream->keys == NULL) {
            free_stream(stream);
            return false;
        }

        stream->key_count = source->key_count;

        for (size_t i = 0u; i < source->key_count; ++i) {

            if (!adsl_dup_string(
                    source->keys[i].name,
                    &stream->keys[i].name) ||
                !adsl_dup_string(
                    source->keys[i].type,
                    &stream->keys[i].type)) {

                free_stream(stream);
                return false;
            }
        }
    }

    *destination = stream;

    return true;
}


static bool adsl_dup_type_fields(
    const adsl_type_t *source,
    adsl_type_t *destination) {

    memset(destination, 0, sizeof(*destination));

    destination->source_line = source->source_line;
    destination->has_id = source->has_id;
    destination->id = source->id;
    destination->is_abstract = source->is_abstract;
    destination->has_tiered_int = source->has_tiered_int;
    destination->tiered_int_min_bytes = source->tiered_int_min_bytes;
    destination->tiered_int_limit_count = source->tiered_int_limit_count;

    for (size_t i = 0u; i < 3u; ++i) {
        destination->tiered_int_limits[i] = source->tiered_int_limits[i];
    }

    if (!adsl_dup_string(source->name, &destination->name) ||
        !adsl_dup_string(source->source_path, &destination->source_path) ||
        !adsl_dup_string(source->parent, &destination->parent)) {

        return false;
    }

    if (source->field_count > 0u) {

        destination->fields =
            calloc(source->field_count, sizeof(*destination->fields));

        if (destination->fields == NULL) {
            return false;
        }

        destination->field_count = source->field_count;

        for (size_t i = 0u; i < source->field_count; ++i) {

            if (!adsl_dup_string(
                    source->fields[i].name,
                    &destination->fields[i].name) ||
                !adsl_dup_string(
                    source->fields[i].type,
                    &destination->fields[i].type)) {

                return false;
            }
        }
    }

    if (source->enum_count > 0u) {

        destination->enum_values =
            calloc(source->enum_count, sizeof(*destination->enum_values));

        if (destination->enum_values == NULL) {
            return false;
        }

        destination->enum_count = source->enum_count;

        for (size_t i = 0u; i < source->enum_count; ++i) {

            if (!adsl_dup_string(
                    source->enum_values[i],
                    &destination->enum_values[i])) {

                return false;
            }
        }
    }

    if (!adsl_dup_stream(source->stream, &destination->stream)) {
        return false;
    }

    return true;
}


static bool adsl_dup_api_fields(
    const adsl_api_t *source,
    adsl_api_t *destination) {

    memset(destination, 0, sizeof(*destination));

    destination->source_line = source->source_line;
    destination->is_abstract = source->is_abstract;

    if (!adsl_dup_string(source->name, &destination->name) ||
        !adsl_dup_string(source->source_path, &destination->source_path)) {

        return false;
    }

    if (source->parent_count > 0u) {

        destination->parents =
            calloc(source->parent_count, sizeof(*destination->parents));

        if (destination->parents == NULL) {
            return false;
        }

        destination->parent_count = source->parent_count;

        for (size_t i = 0u; i < source->parent_count; ++i) {

            if (!adsl_dup_string(
                    source->parents[i],
                    &destination->parents[i])) {

                return false;
            }
        }
    }

    if (source->method_count > 0u) {

        destination->methods =
            calloc(source->method_count, sizeof(*destination->methods));

        if (destination->methods == NULL) {
            return false;
        }

        destination->method_count = source->method_count;

        for (size_t i = 0u; i < source->method_count; ++i) {

            const adsl_method_t *src = &source->methods[i];
            adsl_method_t *dst = &destination->methods[i];

            dst->source_line = src->source_line;
            dst->has_id = src->has_id;
            dst->id = src->id;

            if (!adsl_dup_string(src->name, &dst->name) ||
                !adsl_dup_string(src->source_path, &dst->source_path) ||
                !adsl_dup_string(src->returns_type, &dst->returns_type) ||
                !adsl_dup_string(src->throws_type, &dst->throws_type)) {

                return false;
            }

            if (src->param_count > 0u) {

                dst->params =
                    calloc(src->param_count, sizeof(*dst->params));

                if (dst->params == NULL) {
                    return false;
                }

                dst->param_count = src->param_count;

                for (size_t p = 0u; p < src->param_count; ++p) {

                    if (!adsl_dup_string(
                            src->params[p].name,
                            &dst->params[p].name) ||
                        !adsl_dup_string(
                            src->params[p].type,
                            &dst->params[p].type) ||
                        !adsl_dup_stream(
                            src->params[p].stream,
                            &dst->params[p].stream)) {

                        return false;
                    }
                }
            }

            if (src->return_field_count > 0u) {

                dst->return_fields = calloc(
                    src->return_field_count,
                    sizeof(*dst->return_fields));

                if (dst->return_fields == NULL) {
                    return false;
                }

                dst->return_field_count = src->return_field_count;

                for (size_t f = 0u; f < src->return_field_count; ++f) {

                    if (!adsl_dup_string(
                            src->return_fields[f].name,
                            &dst->return_fields[f].name) ||
                        !adsl_dup_string(
                            src->return_fields[f].type,
                            &dst->return_fields[f].type)) {

                        return false;
                    }
                }
            }
        }
    }

    return true;
}




/*
 * Build the resolved declaration arrays from the include closure plus the
 * root-owned declarations.
 *
 * Ordering contract: resolved_types[] and resolved_apis[] are laid out as
 *
 *     includes (deepest-first, in load order) ++ root declarations
 *
 * This matches the historical recursive-merge order and keeps generated C
 * byte-for-byte identical to the pre-split behavior. Changing it to
 * root-first would reorder typedefs and reorder emitted code.
 *
 * resolved_* are deep copies and own their contents independently of the
 * include buffer and the root arrays. All three arrays (types, apis,
 * includes) are released separately.
 */

static bool adsl_build_resolved_context(
    adsl_document_t *document,
    const adsl_document_t *include_destination) {

    document->resolved_types = NULL;
    document->resolved_type_count = 0u;
    document->resolved_apis = NULL;
    document->resolved_api_count = 0u;

    size_t include_type_count =
        include_destination != NULL
            ? include_destination->type_count
            : 0u;

    size_t include_api_count =
        include_destination != NULL
            ? include_destination->api_count
            : 0u;

    size_t total_types =
        include_type_count + document->type_count;

    size_t total_apis =
        include_api_count + document->api_count;

    if (total_types > 0u) {

        document->resolved_types = calloc(
            total_types,
            sizeof(*document->resolved_types));

        if (document->resolved_types == NULL) {
            return false;
        }

        document->resolved_type_count = total_types;

        for (size_t i = 0u; i < include_type_count; ++i) {

            if (!adsl_dup_type_fields(
                    &include_destination->types[i],
                    &document->resolved_types[i])) {

                free_type_array(
                    document->resolved_types,
                    document->resolved_type_count);

                document->resolved_types = NULL;
                document->resolved_type_count = 0u;

                return false;
            }
        }

        for (size_t i = 0u; i < document->type_count; ++i) {

            if (!adsl_dup_type_fields(
                    &document->types[i],
                    &document->resolved_types[include_type_count + i])) {

                free_type_array(
                    document->resolved_types,
                    document->resolved_type_count);

                document->resolved_types = NULL;
                document->resolved_type_count = 0u;

                return false;
            }
        }
    }

    if (total_apis > 0u) {

        document->resolved_apis = calloc(
            total_apis,
            sizeof(*document->resolved_apis));

        if (document->resolved_apis == NULL) {

            free_type_array(
                document->resolved_types,
                document->resolved_type_count);

            document->resolved_types = NULL;
            document->resolved_type_count = 0u;

            return false;
        }

        document->resolved_api_count = total_apis;

        for (size_t i = 0u; i < include_api_count; ++i) {

            if (!adsl_dup_api_fields(
                    &include_destination->apis[i],
                    &document->resolved_apis[i])) {

                free_api_array(
                    document->resolved_apis,
                    document->resolved_api_count);

                document->resolved_apis = NULL;
                document->resolved_api_count = 0u;

                return false;
            }
        }

        for (size_t i = 0u; i < document->api_count; ++i) {

            if (!adsl_dup_api_fields(
                    &document->apis[i],
                    &document->resolved_apis[include_api_count + i])) {

                free_api_array(
                    document->resolved_apis,
                    document->resolved_api_count);

                document->resolved_apis = NULL;
                document->resolved_api_count = 0u;

                return false;
            }
        }
    }

    return true;
}



static bool adsl_normalize_document(
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

for (size_t i = 0u; i < document->resolved_type_count; ++i) {
        for (size_t j = i + 1u; j < document->resolved_type_count; ) {

            if (!same_name(
document->resolved_types[i].name,
                     document->resolved_types[j].name)) {

                ++j;
                continue;
            }

                if (!adsl_same_type_definition(
                    &document->resolved_types[i],
                    &document->resolved_types[j])) {

                set_error(
                    error,
                    error_capacity,
                    "conflicting type declaration '%s'",
                    document->resolved_types[i].name);

                return false;
            }

            adsl_remove_resolved_type_at(
                document,
                j);
        }
    }

    for (size_t i = 0u; i < document->resolved_api_count; ++i) {
        for (size_t j = i + 1u; j < document->resolved_api_count; ) {

            if (!same_name(
document->resolved_apis[i].name,
                     document->resolved_apis[j].name)) {

                ++j;
                continue;
            }

                if (!adsl_same_api_definition(
                    &document->resolved_apis[i],
                    &document->resolved_apis[j])) {

                set_error(
                    error,
                    error_capacity,
                    "conflicting api declaration '%s'",
                    document->resolved_apis[i].name);

                return false;
            }

            adsl_remove_resolved_api_at(
                document,
                j);
        }
    }

    return true;
}


static bool adsl_merge_loaded_document(
    adsl_document_t *destination,
    adsl_document_t *source,
    bool root_declaration,
    char *error,
    size_t error_capacity) {

    (void)error;
    (void)error_capacity;



    (void)root_declaration;

    if (source->type_count > 0u) {
        size_t old_count = destination->type_count;

        adsl_type_t *types =
            realloc(
                destination->types,
                (old_count + source->type_count) * sizeof(*types));

        if (types == NULL) {
            return false;
        }

        destination->types = types;

        memcpy(
            destination->types + old_count,
            source->types,
            source->type_count * sizeof(*source->types));

        destination->type_count =
            old_count + source->type_count;

        free(source->types);
        source->types = NULL;
        source->type_count = 0u;
    }

    if (source->api_count > 0u) {
        size_t old_count = destination->api_count;

        adsl_api_t *apis =
            realloc(
                destination->apis,
                (old_count + source->api_count) * sizeof(*apis));

        if (apis == NULL) {
            return false;
        }

        destination->apis = apis;

        memcpy(
            destination->apis + old_count,
            source->apis,
            source->api_count * sizeof(*source->apis));

        destination->api_count =
            old_count + source->api_count;

        free(source->apis);
        source->apis = NULL;
        source->api_count = 0u;
    }

    return true;
}


static bool adsl_load_recursive(
    const char *path,
    const char *const *include_paths,
    size_t include_path_count,

    adsl_document_t *destination,
    adsl_document_t *include_destination,
    adsl_load_state_t *state,

    size_t depth,
    bool root_declaration,
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
                include_destination,
                state,

                depth + 1u,
                false,
                error,
                error_capacity)) {


            adsl_document_free(&current);
            return false;
        }
    }




    adsl_document_t *merge_target =
        root_declaration
            ? destination
            : include_destination;

    if (!adsl_merge_loaded_document(
        merge_target,

            &current,
            root_declaration,
            error,
            error_capacity)) {



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

    
    document->root_source_path =
        strdup(path);

    
    if (document->root_source_path == NULL) {
        set_error(
            error,
            error_capacity,
            "out of memory");
        return false;
    }






    adsl_load_state_t state = {
        NULL,
        0u
    };

    adsl_document_t include_destination;
    adsl_document_init(&include_destination);

    bool loaded =
        adsl_load_recursive(
            path,
            include_paths,
            include_path_count,
            document,
            &include_destination,
            &state,
            0u,
            true,
            error,
            error_capacity);








    if (loaded) {

        if (!adsl_build_resolved_context(document, &include_destination)) {

            set_error(
                error,
                error_capacity,
                "out of memory while building resolved context");

            loaded = false;

        } else if (!adsl_normalize_document(document, error, error_capacity) ||
                   !adsl_validate_unique_declarations(document, error, error_capacity)) {

            loaded = false;
        }
    }





    adsl_document_free(&include_destination);


    adsl_load_state_free(&state);


    return loaded;
}


void adsl_project_init(
    adsl_project_t *project) {

    if (project != NULL) {
        memset(
            project,
            0,
            sizeof(*project));
    }
}


void adsl_project_free(
    adsl_project_t *project) {

    if (project == NULL) {
        return;
    }

    for (size_t i = 0u;
         i < project->document_count;
         ++i) {

        adsl_document_free(
            &project->documents[i]);
    }

    free(project->documents);

    adsl_project_init(project);
}


bool adsl_project_load_files(
    const char *const *paths,
    size_t path_count,
    const char *const *include_paths,
    size_t include_path_count,
    adsl_project_t *project,
    char *error,
    size_t error_capacity) {

    if (paths == NULL ||
        path_count == 0u ||
        project == NULL ||
        (include_path_count > 0u &&
         include_paths == NULL)) {

        set_error(
            error,
            error_capacity,
            "invalid project loader arguments");

        return false;
    }

    if (project->documents != NULL ||
        project->document_count != 0u) {

        set_error(
            error,
            error_capacity,
            "project is not empty");

        return false;
    }

    adsl_document_t *documents =
        calloc(
            path_count,
            sizeof(*documents));

    if (documents == NULL) {

        set_error(
            error,
            error_capacity,
            "out of memory");

        return false;
    }

    for (size_t i = 0u; i < path_count; ++i) {

        adsl_document_init(&documents[i]);

        if (!adsl_load_file_with_include_paths(
                paths[i],
                include_paths,
                include_path_count,
                &documents[i],
                error,
                error_capacity)) {

            for (size_t j = 0u; j <= i; ++j) {
                adsl_document_free(&documents[j]);
            }

            free(documents);

            return false;
        }
    }

    project->documents = documents;
    project->document_count = path_count;

    return true;
}


/* Deduplicated, owned string list used by the include-directory scanner. */
typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} adsl_string_list_t;

static bool adsl_string_list_contains(
    const adsl_string_list_t *list,
    const char *value) {

    for (size_t i = 0u; i < list->count; ++i) {
        if (strcmp(list->items[i], value) == 0) {
            return true;
        }
    }

    return false;
}

static bool adsl_string_list_add(
    adsl_string_list_t *list,
    const char *value) {

    if (adsl_string_list_contains(list, value)) {
        return true;
    }

    if (list->count == list->capacity) {

        size_t next_capacity =
            list->capacity == 0u
                ? 8u
                : list->capacity * 2u;

        char **items =
            realloc(
                list->items,
                next_capacity * sizeof(*items));

        if (items == NULL) {
            return false;
        }

        list->items = items;
        list->capacity = next_capacity;
    }

    char *copy =
        copy_string(value);

    if (copy == NULL) {
        return false;
    }

    list->items[list->count++] = copy;

    return true;
}

static void adsl_string_list_free(
    adsl_string_list_t *list) {

    for (size_t i = 0u; i < list->count; ++i) {
        free(list->items[i]);
    }

    free(list->items);

    list->items = NULL;
    list->count = 0u;
    list->capacity = 0u;
}

static int adsl_compare_canonical_paths(
    const void *left,
    const void *right) {

    const char *const *a =
        (const char *const *)left;

    const char *const *b =
        (const char *const *)right;

    return strcmp(*a, *b);
}

static bool adsl_canonical_path(
    const char *path,
    char *output,
    size_t capacity) {

    char resolved[4096];

    if (realpath(path, resolved) == NULL) {
        return false;
    }

    size_t length =
        strlen(resolved);

    if (length >= capacity) {
        return false;
    }

    memcpy(output, resolved, length + 1u);

    return true;
}

static bool adsl_path_is_directory(
    const char *path) {

    struct stat info;

    if (stat(path, &info) != 0) {
        return false;
    }

    return S_ISDIR(info.st_mode) != 0;
}

static bool adsl_has_adsl_suffix(
    const char *name) {

    static const char suffix[] = ".adsl.yaml";
    size_t name_length = strlen(name);
    size_t suffix_length = sizeof(suffix) - 1u;

    if (name_length <= suffix_length) {
        return false;
    }

    return strcmp(
        name + name_length - suffix_length,
        suffix) == 0;
}

static bool adsl_scan_directory_recursive(
    const char *directory,
    adsl_string_list_t *directories_seen,
    adsl_string_list_t *files_found,
    char *error,
    size_t error_capacity) {

    DIR *dir =
        opendir(directory);

    if (dir == NULL) {

        set_error(
            error,
            error_capacity,
            "cannot open directory '%s'",
            directory);

        return false;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {

            continue;
        }

        char full[4096];

        if (!adsl_path_join(
                directory,
                entry->d_name,
                full,
                sizeof(full))) {

            continue;
        }

        struct stat info;

        if (stat(full, &info) != 0) {
            continue;
        }

        if (S_ISDIR(info.st_mode)) {

            char canonical[4096];

            if (!adsl_canonical_path(
                    full,
                    canonical,
                    sizeof(canonical))) {

                continue;
            }

            if (adsl_string_list_contains(
                    directories_seen,
                    canonical)) {

                continue;
            }

            if (!adsl_string_list_add(
                    directories_seen,
                    canonical)) {

                closedir(dir);

                set_error(
                    error,
                    error_capacity,
                    "out of memory while scanning '%s'",
                    directory);

                return false;
            }

            if (!adsl_scan_directory_recursive(
                    canonical,
                    directories_seen,
                    files_found,
                    error,
                    error_capacity)) {

                closedir(dir);

                return false;
            }

            continue;
        }

        if (!S_ISREG(info.st_mode)) {
            continue;
        }

        if (!adsl_has_adsl_suffix(entry->d_name)) {
            continue;
        }

        char canonical[4096];

        if (!adsl_canonical_path(
                full,
                canonical,
                sizeof(canonical))) {

            continue;
        }

        if (!adsl_string_list_add(
                files_found,
                canonical)) {

            closedir(dir);

            set_error(
                error,
                error_capacity,
                "out of memory while scanning '%s'",
                directory);

            return false;
        }
    }

    closedir(dir);

    return true;
}

bool adsl_find_files(
    const char *const *include_paths,
    size_t include_path_count,
    char ***files,
    size_t *file_count,
    char *error,
    size_t error_capacity) {

    if (files != NULL) {
        *files = NULL;
    }

    if (file_count != NULL) {
        *file_count = 0u;
    }

    if (files == NULL ||
        file_count == NULL ||
        (include_path_count > 0u &&
         include_paths == NULL)) {

        set_error(
            error,
            error_capacity,
            "invalid find arguments");

        return false;
    }

    adsl_string_list_t directories_seen = {0};
    adsl_string_list_t files_found = {0};

    for (size_t i = 0u; i < include_path_count; ++i) {

        if (include_paths[i] == NULL ||
            include_paths[i][0] == '\0') {

            continue;
        }

        char canonical[4096];

        if (!adsl_canonical_path(
                include_paths[i],
                canonical,
                sizeof(canonical))) {

            adsl_string_list_free(&directories_seen);
            adsl_string_list_free(&files_found);

            set_error(
                error,
                error_capacity,
                "cannot resolve include directory '%s'",
                include_paths[i]);

            return false;
        }

        if (!adsl_path_is_directory(canonical)) {

            adsl_string_list_free(&directories_seen);
            adsl_string_list_free(&files_found);

            set_error(
                error,
                error_capacity,
                "'%s' is not a directory",
                include_paths[i]);

            return false;
        }

        if (adsl_string_list_contains(
                &directories_seen,
                canonical)) {

            continue;
        }

        if (!adsl_string_list_add(
                &directories_seen,
                canonical)) {

            adsl_string_list_free(&directories_seen);
            adsl_string_list_free(&files_found);

            set_error(
                error,
                error_capacity,
                "out of memory");

            return false;
        }

        if (!adsl_scan_directory_recursive(
                canonical,
                &directories_seen,
                &files_found,
                error,
                error_capacity)) {

            adsl_string_list_free(&directories_seen);
            adsl_string_list_free(&files_found);

            return false;
        }
    }

    adsl_string_list_free(&directories_seen);

    if (files_found.count > 1u) {

        qsort(
            files_found.items,
            files_found.count,
            sizeof(*files_found.items),
            adsl_compare_canonical_paths);
    }

    *files = files_found.items;
    *file_count = files_found.count;

    return true;
}



static bool adsl_strings_equal_or_both_null(
    const char *left,
    const char *right) {

    if (left == NULL && right == NULL) {
        return true;
    }

    if (left == NULL || right == NULL) {
        return false;
    }

    return strcmp(left, right) == 0;
}

static bool adsl_module_name_from_root(
    const char *root_source_path,
    char *module,
    size_t capacity) {

    if (root_source_path == NULL ||
        module == NULL ||
        capacity == 0u) {

        return false;
    }

    const char *base = root_source_path;

    for (const char *p = root_source_path; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            base = p + 1;
        }
    }

    static const char suffix_yaml[] = ".adsl.yaml";
    static const char suffix_yml[] = ".adsl.yml";

    size_t base_length = strlen(base);
    size_t yaml_length = sizeof(suffix_yaml) - 1u;
    size_t yml_length = sizeof(suffix_yml) - 1u;

    size_t stem_length = 0u;

    if (base_length > yaml_length &&
        strcmp(base + base_length - yaml_length, suffix_yaml) == 0) {

        stem_length = base_length - yaml_length;
    } else if (base_length > yml_length &&
               strcmp(base + base_length - yml_length, suffix_yml) == 0) {

        stem_length = base_length - yml_length;
    } else {

        return false;
    }

    if (stem_length == 0u || stem_length >= capacity) {
        return false;
    }

    memcpy(module, base, stem_length);
    module[stem_length] = '\0';

    return true;
}

static bool adsl_type_registry_insert(
    adsl_type_registry_t *registry,
    const char *module,
    const char *name,
    adsl_type_kind_t kind,
    const adsl_type_t *declaration,
    char *error,
    size_t error_capacity) {

    for (size_t i = 0u; i < registry->entry_count; ++i) {

        const adsl_registry_entry_t *existing = &registry->entries[i];

        if (!adsl_strings_equal_or_both_null(existing->module, module) ||
            !adsl_strings_equal_or_both_null(existing->name, name)) {

            continue;
        }

        if (existing->kind == kind) {

            if (kind == ADSL_TYPE_DECLARED) {

                if (adsl_same_type_definition(
                        existing->declaration,
                        declaration)) {

                    return true;
                }

                set_error(
                    error,
                    error_capacity,
                    "conflicting type declaration '%s.%s'",
                    module != NULL ? module : "<null>",
                    name);

                return false;
            }

            /* Duplicate primitive entry: nothing to do. */
            return true;
        }

        set_error(
            error,
            error_capacity,
            "conflicting type declaration '%s.%s'",
            module != NULL ? module : "<null>",
            name);

        return false;
    }

    adsl_registry_entry_t *entries =
        realloc(
            registry->entries,
            (registry->entry_count + 1u) * sizeof(*entries));

    if (entries == NULL) {

        set_error(error, error_capacity, "out of memory");
        return false;
    }

    registry->entries = entries;

    adsl_registry_entry_t *entry =
        &registry->entries[registry->entry_count];

    memset(entry, 0, sizeof(*entry));

    entry->name = copy_string(name);

    if (module != NULL) {
        entry->module = copy_string(module);
    }

    entry->kind = kind;
    entry->declaration = declaration;

    if (entry->name == NULL ||
        (module != NULL && entry->module == NULL)) {

        free(entry->module);
        free(entry->name);

        entry->module = NULL;
        entry->name = NULL;

        set_error(error, error_capacity, "out of memory");
        return false;
    }

    registry->entry_count++;

    return true;
}

static bool adsl_register_builtin_types(
    adsl_type_registry_t *registry,
    char *error,
    size_t error_capacity) {

    /*
     * This list must stay in sync with primitive_c_type() in emitter.c.
     * Aliases are stored exactly as ADSL accepts them (e.g. both "bool" and
     * "boolean" are registered); they are intentionally NOT normalized to a
     * canonical form.
     */
    static const char *const builtin_names[] = {
        "bool",
        "boolean",
        "byte",
        "short",
        "int",
        "long",
        "intpack",
        "date",
        "float",
        "double",
        "uuid",
        "string",
        "uri",
    };

    for (size_t i = 0u;
         i < sizeof(builtin_names) / sizeof(builtin_names[0]);
         ++i) {

        if (!adsl_type_registry_insert(
                registry,
                NULL,
                builtin_names[i],
                ADSL_TYPE_PRIMITIVE,
                NULL,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}

void adsl_type_registry_init(
    adsl_type_registry_t *registry) {

    if (registry != NULL) {
        memset(registry, 0, sizeof(*registry));
    }
}

void adsl_type_registry_free(
    adsl_type_registry_t *registry) {

    if (registry == NULL) {
        return;
    }

    for (size_t i = 0u; i < registry->entry_count; ++i) {
        free(registry->entries[i].module);
        free(registry->entries[i].name);
    }

    free(registry->entries);

    adsl_type_registry_init(registry);
}

bool adsl_type_registry_build(
    const adsl_project_t *project,
    adsl_type_registry_t *registry,
    char *error,
    size_t error_capacity) {

    if (project == NULL || registry == NULL) {

        set_error(error, error_capacity, "invalid registry arguments");
        return false;
    }

    if (registry->entries != NULL || registry->entry_count != 0u) {

        set_error(error, error_capacity, "registry is not empty");
        return false;
    }

    adsl_string_list_t seen_modules = {0};

    for (size_t d = 0u; d < project->document_count; ++d) {

        const adsl_document_t *document = &project->documents[d];

        char module[1024];

        if (!adsl_module_name_from_root(
                document->root_source_path,
                module,
                sizeof(module))) {

            adsl_string_list_free(&seen_modules);
            adsl_type_registry_free(registry);

            set_error(
                error,
                error_capacity,
                "cannot derive module name from '%s'",
                document->root_source_path != NULL
                    ? document->root_source_path
                    : "<null>");

            return false;
        }

        if (adsl_string_list_contains(&seen_modules, module)) {

            adsl_string_list_free(&seen_modules);
            adsl_type_registry_free(registry);

            set_error(
                error,
                error_capacity,
                "duplicate module '%s' in project",
                module);

            return false;
        }

        if (!adsl_string_list_add(&seen_modules, module)) {

            adsl_string_list_free(&seen_modules);
            adsl_type_registry_free(registry);

            set_error(error, error_capacity, "out of memory");
            return false;
        }

        for (size_t t = 0u; t < document->type_count; ++t) {

            const adsl_type_t *type = &document->types[t];

            if (!adsl_type_registry_insert(
                    registry,
                    module,
                    type->name,
                    ADSL_TYPE_DECLARED,
                    type,
                    error,
                    error_capacity)) {

                adsl_string_list_free(&seen_modules);
                adsl_type_registry_free(registry);
                return false;
            }
        }
    }

    adsl_string_list_free(&seen_modules);

    if (!adsl_register_builtin_types(
            registry,
            error,
            error_capacity)) {

        adsl_type_registry_free(registry);
        return false;
    }

    return true;
}

const adsl_registry_entry_t *adsl_type_registry_find(
    const adsl_type_registry_t *registry,
    const char *module,
    const char *name) {

    if (registry == NULL || name == NULL) {
        return NULL;
    }

    for (size_t i = 0u; i < registry->entry_count; ++i) {

        const adsl_registry_entry_t *entry = &registry->entries[i];

        if (!adsl_strings_equal_or_both_null(entry->module, module) ||
            !adsl_strings_equal_or_both_null(entry->name, name)) {

            continue;
        }

        return entry;
    }

    return NULL;
}





static bool adsl_same_type_definition(
    const adsl_type_t *left,
    const adsl_type_t *right) {

    if (!same_name(left->name, right->name) ||
        !same_name(left->parent, right->parent) ||
        left->has_id != right->has_id ||

        (left->has_id &&
         left->id != right->id) ||

        left->is_abstract != right->is_abstract ||
        left->field_count != right->field_count ||
        left->enum_count != right->enum_count ||
        left->has_tiered_int != right->has_tiered_int ||
        left->tiered_int_min_bytes != right->tiered_int_min_bytes ||
        left->tiered_int_limit_count != right->tiered_int_limit_count) {

        return false;
    }

    for (size_t i = 0u;
         i < left->field_count;
         ++i) {

        if (!same_name(
                left->fields[i].name,
                right->fields[i].name) ||
            !same_name(
                left->fields[i].type,
                right->fields[i].type)) {

            return false;
        }
    }

    for (size_t i = 0u;
         i < left->enum_count;
         ++i) {

        if (!same_name(
                left->enum_values[i],
                right->enum_values[i])) {

            return false;
        }
    }

    if (left->has_tiered_int) {
        for (size_t i = 0u;
             i < left->tiered_int_limit_count;
             ++i) {

            if (left->tiered_int_limits[i] !=
                right->tiered_int_limits[i]) {

                return false;
            }
        }
    }

    if ((left->stream == NULL) !=
        (right->stream == NULL)) {

        return false;
    }

    if (left->stream != NULL) {
        if (left->stream->has_crypto != right->stream->has_crypto ||
            left->stream->crypto != right->stream->crypto ||
            !same_name(
                left->stream->api,
                right->stream->api) ||
            !same_name(
                left->stream->remote_api,
                right->stream->remote_api) ||
            left->stream->api_count != right->stream->api_count ||
            left->stream->key_count != right->stream->key_count) {

            return false;
        }
    }

    return true;
}

static bool adsl_same_api_definition(
    const adsl_api_t *left,
    const adsl_api_t *right) {

    if (!same_name(left->name, right->name) ||
        left->is_abstract != right->is_abstract ||
        left->parent_count != right->parent_count ||
        left->method_count != right->method_count) {

        return false;
    }

    for (size_t i = 0u;
         i < left->parent_count;
         ++i) {

        if (!same_name(
                left->parents[i],
                right->parents[i])) {

            return false;
        }
    }

    for (size_t i = 0u;
         i < left->method_count;
         ++i) {

        const adsl_method_t *left_method =
            &left->methods[i];

        const adsl_method_t *right_method =
            &right->methods[i];

        if (!same_name(
                left_method->name,
                right_method->name) ||
            left_method->has_id != right_method->has_id ||

            (left_method->has_id &&
             left_method->id != right_method->id) ||

            !same_name(
                left_method->returns_type,
                right_method->returns_type) ||
            !same_name(
                left_method->throws_type,
                right_method->throws_type) ||
            left_method->param_count != right_method->param_count ||
            left_method->return_field_count != right_method->return_field_count) {

            return false;
        }

        for (size_t p = 0u;
             p < left_method->param_count;
             ++p) {

            if (!same_name(
                    left_method->params[p].name,
                    right_method->params[p].name) ||
                !same_name(
                    left_method->params[p].type,
                    right_method->params[p].type)) {

                return false;
            }
        }

        for (size_t f = 0u;
             f < left_method->return_field_count;
             ++f) {

            if (!same_name(
                    left_method->return_fields[f].name,
                    right_method->return_fields[f].name) ||
                !same_name(
                    left_method->return_fields[f].type,
                    right_method->return_fields[f].type)) {

                return false;
            }
        }
    }

    return true;
}