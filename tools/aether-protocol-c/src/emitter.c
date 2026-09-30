

/*
 * Static C emitter for a validated ADSL semantic document.
 *
 * Generated firmware is schema-free at runtime: inheritance, nullability,
 * arrays, streams, local APIs and concrete wire expressions are resolved here
 * ahead of time.
 *
 * MCU size is a design constraint. Share helpers only when linked measurements
 * demonstrate a win; reducing generated source lines can increase firmware.
 *
 * Fix generated client_server_* defects here and add focused regression
 * fixtures instead of manually patching generated bindings.
 */
#include "adslc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void set_error(
    char *error,
    size_t capacity,
    const char *message) {

    if (error != NULL &&
        capacity > 0u) {

        (void)snprintf(
            error,
            capacity,
            "%s",
            message);
    }
}

static void snake_name(
    const char *source,
    char *target,
    size_t capacity) {

    if (capacity == 0u) {
        return;
    }

    size_t out =
        0u;

    for (size_t i = 0u;
         source[i] != '\0' &&
         out + 1u < capacity;
         ++i) {

        unsigned char c =
            (unsigned char)source[i];

        if (isupper(c)) {
            if (out > 0u &&
                target[out - 1u] != '_' &&
                out + 2u < capacity) {

                target[out++] =
                    '_';
            }

            target[out++] =
                (char)tolower(c);

            continue;
        }

        if (isalnum(c)) {
            target[out++] =
                (char)tolower(c);
        } else if (out > 0u &&
                   target[out - 1u] != '_') {

            target[out++] =
                '_';
        }
    }

    target[out] =
        '\0';

}

static void declared_c_name(
    const char *source,
    char *target,
    size_t capacity) {

    if (capacity == 0u) {
        return;
    }

    char generated[256];

    snake_name(
        source,
        generated,
        sizeof(generated));

    const char *prefix = "";
    size_t prefix_length = 0u;

    if (strncmp(
            generated,
            "aether_",
            strlen("aether_")) == 0) {

        prefix = "adsl_";
        prefix_length =
            strlen(prefix);
    }

    size_t pos = 0u;

    for (size_t i = 0u;
         i < prefix_length &&
         pos + 1u < capacity;
         ++i) {

        target[pos++] =
            prefix[i];
    }

    for (size_t i = 0u;
         generated[i] != '\0' &&
         pos + 1u < capacity;
         ++i) {

        target[pos++] =
            generated[i];
    }

    target[pos] = '\0';
}


static void upper_name(
    const char *source,
    char *target,
    size_t capacity) {

    if (capacity == 0u) {
        return;
    }

    size_t i = 0u;

    for (;
         source[i] != '\0' &&
         i + 1u < capacity;
         ++i) {

        unsigned char c =
            (unsigned char)source[i];

        target[i] =
            isalnum(c)
                ? (char)toupper(c)
                : '_';
    }

    target[i] =
        '\0';
}

static const char *primitive_c_type(
    const char *type) {

    if (strcmp(type, "bool") == 0 ||
        strcmp(type, "boolean") == 0) {

        return "bool";
    }

    if (strcmp(type, "byte") == 0) {
        return "int8_t";
    }

    if (strcmp(type, "short") == 0) {
        return "int16_t";
    }

    if (strcmp(type, "int") == 0) {
        return "int32_t";
    }

    if (strcmp(type, "long") == 0 ||
        strcmp(type, "intpack") == 0 ||
        strcmp(type, "date") == 0) {

        return "int64_t";
    }

    if (strcmp(type, "float") == 0) {
        return "float";
    }

    if (strcmp(type, "double") == 0) {
        return "double";
    }

    if (strcmp(type, "uuid") == 0) {
        return "aether_uuid_t";
    }

    if (strcmp(type, "string") == 0 ||
        strcmp(type, "uri") == 0) {

        return "aether_bytes_view_t";
    }

    return NULL;
}

static bool scalar_type_name(
    const char *type,
    char *base,
    size_t capacity) {

    size_t length =
        strlen(type);

    while (length > 0u &&
           (type[length - 1u] == '?' ||
            type[length - 1u] == '~')) {

        length--;
    }

    for (size_t i = 0u;
         i < length;
         ++i) {

        if (type[i] == '[') {
            return false;
        }
    }

    if (length >= capacity) {
        return false;
    }

    memcpy(
        base,
        type,
        length);

    base[length] =
        '\0';

    for (size_t i = 0u;
         base[i] != '\0';
         ++i) {

        base[i] =
            (char)tolower(
                (unsigned char)base[i]);
    }

    return true;


}


static const adsl_type_t *find_declared_type(
    const adsl_document_t *document,
    const char *name);

static bool type_has_children(
    const adsl_document_t *document,
    const adsl_type_t *type);

static bool type_is_polymorphic_reference(
    const adsl_document_t *document,
    const adsl_type_t *type);

static bool type_is_descendant_or_self(
    const adsl_document_t *document,
    const adsl_type_t *candidate,
    const adsl_type_t *ancestor);

static bool emit_hierarchy_ref(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    char *error,
    size_t error_capacity);

static bool emit_hierarchy_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *static_type,
    const char *expression,
    const char *prefix,
    bool reference_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_all_struct_dependencies(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    uint8_t *states,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_all_struct_field_declarations(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool count_all_nullable_fields(
    const adsl_document_t *document,
    const adsl_type_t *type,
    unsigned int depth,
    size_t *count,
    char *error,
    size_t error_capacity);

static bool emit_all_nullable_mask_bits(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    size_t *bit_index,
    char *error,
    size_t error_capacity);

static bool emit_all_struct_payload(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool parse_array_type(


    const char *type,
    char *element,
    size_t element_capacity,
    bool *dynamic,
    size_t *static_size) {

    if (type == NULL ||
        element == NULL ||
        element_capacity == 0u ||
        dynamic == NULL ||
        static_size == NULL) {

        return false;
    }

    size_t length =
        strlen(type);

    while (length > 0u &&
           (type[length - 1u] == '?' ||
            type[length - 1u] == '~')) {

        length--;
    }

    if (length < 3u ||
        type[length - 1u] != ']') {

        return false;
    }

    const char *open =
        memchr(
            type,
            '[',
            length);

    if (open == NULL) {
        return false;
    }

    size_t element_length =
        (size_t)(open - type);

    if (element_length == 0u ||
        element_length >= element_capacity) {

        return false;
    }

    if (memchr(
            open + 1,
            '[',
            length - element_length - 1u) != NULL) {

        return false;
    }

    memcpy(
        element,
        type,
        element_length);

    element[element_length] =
        '\0';

    for (size_t i = 0u;
         element[i] != '\0';
         ++i) {

        element[i] =
            (char)tolower(
                (unsigned char)element[i]);
    }

    size_t size_length =
        length -
        element_length -
        2u;

    if (size_length == 0u) {
        *dynamic =
            true;

        *static_size =
            0u;

        return true;
    }

    size_t value =
        0u;

    for (size_t i = 0u;
         i < size_length;
         ++i) {

        char c =
            open[1u + i];

        if (c < '0' ||
            c > '9') {

            return false;
        }

        size_t digit =
            (size_t)(c - '0');

        if (value >
            (SIZE_MAX - digit) / 10u) {

            return false;
        }

        value =
            value * 10u +
            digit;
    }

    /*
     * Canonical Java TypeInfo uses arrayStaticSize == 0 to mean
     * dynamic serialization, including the unusual explicit [0] form.
     */
    *dynamic =
        value == 0u;

    *static_size =
        value;

    return true;
}

static bool strip_nullable_suffix(
    const char *type,
    char *result,
    size_t capacity) {

    if (type == NULL ||
        result == NULL ||
        capacity == 0u) {

        return false;
    }

    size_t length =
        strlen(type);

    if (length > 0u &&
        type[length - 1u] == '?') {

        length--;
    }

    if (length >= capacity) {
        return false;
    }

    memcpy(
        result,
        type,
        length);

    result[length] =
        '\0';

    return true;
}

static bool array_alias_name(
    const adsl_document_t *document,
    const char *type,
    char *name,
    size_t capacity) {

    char element[128];
    bool dynamic;
    size_t static_size;

    if (!parse_array_type(
            type,
            element,
            sizeof(element),
            &dynamic,
            &static_size)) {

        return false;
    }

    if (dynamic &&
        strcmp(element, "byte") == 0) {

        if (capacity <=
            strlen("aether_bytes_view_t")) {

            return false;
        }

        strcpy(
            name,
            "aether_bytes_view_t");

        return true;
    }

    char stem[256];

    const adsl_type_t *declared =
        find_declared_type(
            document,
            element);

    if (declared != NULL) {

declared_c_name(
            declared->name,
            stem,
            sizeof(stem));

    } else {
        snake_name(
            element,
            stem,
            sizeof(stem));
    }

    int written;

    if (dynamic) {
        written =
            snprintf(
                name,
                capacity,
                "%s_array_view_t",
                stem);
    } else {
        written =
            snprintf(
                name,
                capacity,
                "%s_array_%zu_t",
                stem,
                static_size);
    }

    return written >= 0 &&
           (size_t)written < capacity;
}

static bool emit_array_element_c_type(
    FILE *output,
    const adsl_document_t *document,
    const char *element,
    char *error,
    size_t error_capacity) {

    const char *primitive =
        primitive_c_type(
            element);

    if (primitive != NULL) {
        fputs(
            primitive,
            output);

        return true;
    }

    const adsl_type_t *declared =
        find_declared_type(
            document,
            element);

    if (declared == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated array element type");

        return false;
    }

    char generated_name[256];


declared_c_name(
        declared->name,
        generated_name,
        sizeof(generated_name));


    if (declared->enum_count > 0u) {
        fprintf(
            output,
            "%s_t",
            generated_name);

        return true;
    }

    if (type_is_polymorphic_reference(
            document,
            declared)) {

        fprintf(
            output,
            "%s_ref_t",
            generated_name);

        return true;
    }

    if (declared->is_abstract) {
        set_error(
            error,
            error_capacity,
            "abstract array element has no concrete hierarchy descendants");

        return false;
    }

    fprintf(
        output,
        "%s_t",
        generated_name);

    return true;
}

static bool emit_array_alias(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    char *error,
    size_t error_capacity) {

    char element[128];
    bool dynamic;
    size_t static_size;

    if (!parse_array_type(
            type,
            element,
            sizeof(element),
            &dynamic,
            &static_size)) {

        return true;
    }

    if (dynamic &&
        strcmp(element, "byte") == 0) {

        return true;
    }

    const adsl_type_t *declared =
        find_declared_type(
            document,
            element);

    if (!dynamic &&
        declared != NULL &&
        declared->enum_count == 0u &&
        !type_is_polymorphic_reference(
            document,
            declared)) {

        set_error(
            error,
            error_capacity,
            "milestone-2 static arrays of concrete generated structs are not yet supported");

        return false;
    }

    char alias[256];

    if (!array_alias_name(
            document,
            type,
            alias,
            sizeof(alias))) {

        set_error(
            error,
            error_capacity,
            "generated array alias name is too long");

        return false;
    }

    char guard[320];

    upper_name(
        alias,
        guard,
        sizeof(guard));

    fprintf(
        output,
        "#ifndef AETHER_ADSLC_%s_DEFINED\n"
        "#define AETHER_ADSLC_%s_DEFINED\n",
        guard,
        guard);

    if (dynamic) {
        fputs(
            "typedef struct {\n"
            "    const ",
            output);

        if (!emit_array_element_c_type(
                output,
                document,
                element,
                error,
                error_capacity)) {

            return false;
        }

        fprintf(
            output,
            " *data;\n"
            "    size_t length;\n"
            "} %s;\n"
            "#endif\n\n",
            alias);

        return true;
    }

    fputs(
        "typedef struct {\n"
        "    ",
        output);

    if (!emit_array_element_c_type(
            output,
            document,
            element,
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        " data[%zu];\n"
        "} %s;\n"
        "#endif\n\n",
        static_size,
        alias);

    return true;
}

static bool emit_array_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *expression,
    const char *prefix,
    bool array_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_struct_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *declared_type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_parameter_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    if (param == NULL ||
        param->type == NULL) {

        set_error(
            error,
            error_capacity,
            "invalid C parameter metadata");

        return false;
    }

    const char *type =
        param->type;

    if (param->stream != NULL) {
        if (*type == '\0') {
            set_error(
                error,
                error_capacity,
                "stream parameter is missing a generated name");

            return false;
        }

        char stream_name[256];


declared_c_name(
            type,
            stream_name,
            sizeof(stream_name));


        fprintf(
            output,
            "const %s_t *",
            stream_name);

        return true;
    }

    if (strchr(type, '?') != NULL ||
        strchr(type, '~') != NULL) {

        set_error(
            error,
            error_capacity,
            "milestone-2 emitter does not yet support nullable/collapsible parameters");

        return false;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            type,
            array_element,
            sizeof(array_element),
            &array_dynamic,
            &array_static_size)) {

        (void)array_element;
        (void)array_static_size;

        char alias[256];

        if (!array_alias_name(
                document,
                type,
                alias,
                sizeof(alias))) {

            set_error(
                error,
                error_capacity,
                "generated array parameter type name is too long");

            return false;
        }

        if (!array_dynamic) {
            fputs(
                "const ",
                output);
        }

        fputs(
            alias,
            output);

        if (!array_dynamic) {
            fputs(
                " *",
                output);
        }

        return true;
    }

    char base[128];

    if (!scalar_type_name(
            type,
            base,
            sizeof(base))) {

        set_error(
            error,
            error_capacity,
            "milestone-2 emitter supports scalar or one-dimensional array parameters only");

        return false;
    }

    const char *c_type =
        primitive_c_type(base);

    if (c_type != NULL) {
        fputs(
            c_type,
            output);

        return true;
    }

    const adsl_type_t *declared_type =
        find_declared_type(
            document,
            base);

    if (declared_type == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown C parameter type");

        return false;
    }

    char generated_name[256];


declared_c_name(
        declared_type->name,
        generated_name,
        sizeof(generated_name));


    if (declared_type->stream != NULL) {
        fprintf(
            output,
            "const %s_t *",
            generated_name);

        return true;
    }

    if (declared_type->enum_count > 0u) {
        fprintf(
            output,
            "%s_t",
            generated_name);

        return true;
    }

    if (type_is_polymorphic_reference(
            document,
            declared_type)) {

        fprintf(
            output,
            "const %s_ref_t *",
            generated_name);

        return true;
    }

    if (declared_type->is_abstract) {
        set_error(
            error,
            error_capacity,
            "abstract parameter type has no concrete hierarchy descendants");

        return false;
    }

    fprintf(
        output,
        "const %s_t *",
        generated_name);

    return true;
}


static const adsl_type_t *find_declared_type(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    size_t name_length =
        strlen(name);

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *candidate =
            &document->types[i];

        size_t candidate_length =
            strlen(candidate->name);

        if (candidate_length !=
            name_length) {

            continue;
        }

        bool equal =
            true;

        for (size_t c = 0u;
             c < name_length;
             ++c) {

            if (tolower(
                    (unsigned char)candidate->name[c]) !=
                tolower(
                    (unsigned char)name[c])) {

                equal =
                    false;

                break;
            }
        }

        if (equal) {
            return candidate;
        }
    }

    return NULL;
}

static bool emit_fixed_expression(
    FILE *output,
    const char *base,
    const char *expression,
    const char *prefix) {

    (void)prefix;

    const char *runtime_fn = NULL;
    const char *cast_type = NULL;

    if (strcmp(base, "bool") == 0 ||
        strcmp(base, "boolean") == 0 ||
        strcmp(base, "byte") == 0) {

        runtime_fn = "aether_meta_write_u8";
        cast_type = "uint8_t";
    } else if (strcmp(base, "short") == 0) {
        runtime_fn = "aether_meta_write_u16le";
        cast_type = "uint16_t";
    } else if (strcmp(base, "int") == 0) {
        runtime_fn = "aether_meta_write_u32le";
        cast_type = "uint32_t";
    } else if (strcmp(base, "long") == 0 ||
               strcmp(base, "date") == 0) {

        runtime_fn = "aether_meta_write_u64le";
        cast_type = "uint64_t";
    } else if (strcmp(base, "float") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint32_t bits;\n"
            "        memcpy(&bits, &%s, sizeof(bits));\n"
            "        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, bits);\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            expression);

        return true;
    } else if (strcmp(base, "double") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint64_t bits;\n"
            "        memcpy(&bits, &%s, sizeof(bits));\n"
            "        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            expression);

        return true;
    } else {
        return false;
    }

    fprintf(
        output,
        "    {\n"
        "        aether_status_t meta_status = %s(tx, tx_capacity, &pos, (%s)%s);\n"
        "        if (meta_status != AETHER_OK) return meta_status;\n"
        "    }\n",
        runtime_fn,
        cast_type,
        expression);

    return true;
}

static void emit_uuid_expression(
    FILE *output,
    const char *expression) {

    fprintf(
        output,
        "    {\n"

        "        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, %s);\n"
        "        if (meta_status != AETHER_OK) return meta_status;\n"
        "    }\n",
        expression);
}

static void emit_pack_value(
    FILE *output,
    const char *value_name,
    const char *prefix) {

    (void)prefix;

    fprintf(
        output,
        "    {\n"
        "        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, %s);\n"
        "        if (meta_status != AETHER_OK) return meta_status;\n"
        "    }\n",
        value_name);
}

static void emit_bytes_view_expression(
    FILE *output,
    const char *expression,
    const char *prefix) {

    (void)prefix;

    fprintf(
        output,
        "    {\n"
        "        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, %s.data, %s.length);\n"

        "        if (meta_status != AETHER_OK) return meta_status;\n"
        "    }\n",
        expression,
        expression);
}


static bool emit_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_struct_definition(
    FILE *output,
    const adsl_document_t *document,
    size_t type_index,
    uint8_t *states,
    char *error,
    size_t error_capacity);


static bool emit_serialize_param(
    FILE *output,
    const adsl_document_t *document,
    const char *base_snake,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {


    if (param->stream != NULL) {
        if (param->type == NULL ||
            *param->type == '\0') {

            set_error(
                error,
                error_capacity,
                "stream parameter is missing a generated name");

            return false;
        }

        fprintf(
            output,
            "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n"
            "    if (%s->length > %s->capacity) return AETHER_ERR_ARGUMENT;\n",
            param->name,
            param->name,
            param->name);

        char stream_expression[512];

        int stream_expression_size =
            snprintf(
                stream_expression,
                sizeof(stream_expression),
                "(*%s)",
                param->name);

        if (stream_expression_size < 0 ||
            (size_t)stream_expression_size >=
                sizeof(stream_expression)) {

            set_error(
                error,
                error_capacity,
                "generated stream parameter expression is too long");

            return false;
        }

        emit_bytes_view_expression(
            output,
            stream_expression,
            param->name);

        return true;
    }

    if (strchr(param->type, '?') != NULL ||
        strchr(param->type, '~') != NULL) {

        set_error(
            error,
            error_capacity,
            "milestone-2 emitter does not yet serialize nullable/collapsible parameters");

        return false;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            param->type,
            array_element,
            sizeof(array_element),
            &array_dynamic,
            &array_static_size)) {

        (void)array_element;
        (void)array_static_size;

        return emit_array_expression(
            output,
            document,
            param->type,
            param->name,
            param->name,
            !array_dynamic,
            0u,
            error,
            error_capacity);
    }

    char base[128];

    if (!scalar_type_name(
            param->type,
            base,
            sizeof(base))) {

        set_error(
            error,
            error_capacity,
            "milestone-2 emitter supports scalar or one-dimensional array parameters only");

        return false;
    }

    const adsl_type_t *declared_type =
        find_declared_type(
            document,
            base);




    if (declared_type != NULL &&
        type_is_polymorphic_reference(
            document,
            declared_type)) {



        char type_name[256];

        declared_c_name(
            declared_type->name,
            type_name,
            sizeof(type_name));

        fprintf(
            output,
            "    {\n"
            "        aether_status_t meta_status = %s_serialize_%s(\n"
            "            tx,\n"
            "            tx_capacity,\n"
            "            &pos,\n"
            "            %s);\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            base_snake,
            type_name,
            param->name);

        return true;
    }


    bool struct_pointer =
        declared_type != NULL &&
        declared_type->enum_count == 0u;

    return emit_value_expression(
        output,
        document,
        param->type,
        param->name,
        param->name,
        struct_pointer,
        0u,
        error,
        error_capacity);
}

static bool emit_stream_typedef(
    FILE *output,
    const char *name,
    char *error,
    size_t error_capacity) {

    if (name == NULL ||
        *name == '\0') {

        set_error(
            error,
            error_capacity,
            "stream declaration is missing a generated name");

        return false;
    }

    char generated_name[256];


declared_c_name(
        name,
        generated_name,
        sizeof(generated_name));


    fprintf(
        output,
        "typedef struct {\n"
        "    uint8_t *data;\n"
        "    size_t capacity;\n"
        "    size_t length;\n"
        "} %s_t;\n\n",
        generated_name);

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated stream type");

        return false;
    }

    return true;
}

static bool emit_types_header(

    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(path, "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated types header");

        return false;
    }

    char guard[256];

    upper_name(
        base_name,
        guard,
        sizeof(guard));

    fprintf(
        output,
        "#ifndef %s_TYPES_H\n"
        "#define %s_TYPES_H\n\n"
        "#include \"aether_client.h\"\n"
        "#include <stdbool.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n",
        guard,
        guard);


for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        if (type->stream == NULL) {
            continue;
        }

        if (!emit_stream_typedef(
                output,
                type->name,
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                const adsl_param_t *param =
                    &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }

                if (!emit_stream_typedef(
                        output,
                        param->type,
                        error,
                        error_capacity)) {

                    fclose(output);
                    return false;
                }
            }
        }
    }

    for (size_t i = 0u;

         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        if (type->enum_count == 0u) {
            continue;
        }

        char type_name[256];


declared_c_name(
            type->name,
            type_name,
            sizeof(type_name));


        fprintf(
            output,
            "typedef enum {\n");

        for (size_t e = 0u;
             e < type->enum_count;
             ++e) {

            char enum_name[256];

            upper_name(
                type->enum_values[e],
                enum_name,
                sizeof(enum_name));

            fprintf(
                output,
                "    %s_%s = %zu%s\n",
                type_name,
                enum_name,
                e,
                e + 1u == type->enum_count
                    ? ""
                    : ",");
        }

        fprintf(
            output,
            "} %s_t;\n\n",
            type_name);
    }

    /*
     * Forward-declare every concrete payload. Hierarchy refs contain only
     * typed pointers, so they can be defined before concrete bodies.
     */
    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];


if (type->enum_count > 0u ||
            type->is_abstract ||
            type->stream != NULL) {


            continue;
        }

        char type_name[256];


declared_c_name(
            type->name,
            type_name,
            sizeof(type_name));


        fprintf(
            output,
            "typedef struct %s_t %s_t;\n",
            type_name,
            type_name);
    }

    if (document->type_count > 0u) {
        fputc(
            '\n',
            output);
    }

    /*
     * A polymorphic static type is represented as an explicit discriminator
     * plus a union of typed pointers to all concrete descendants.
     */
    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        if (!emit_hierarchy_ref(
                output,
                document,
                &document->types[i],
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }
    }

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        for (size_t f = 0u;
             f < type->field_count;
             ++f) {

            if (!emit_array_alias(
                    output,
                    document,
                    type->fields[f].type,
                    error,
                    error_capacity)) {

                fclose(output);
                return false;
            }
        }
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                if (!emit_array_alias(
                        output,
                        document,
                        method->params[p].type,
                        error,
                        error_capacity)) {

                    fclose(output);
                    return false;
                }
            }

            if (!emit_array_alias(
                    output,
                    document,
                    method->returns_type,
                    error,
                    error_capacity) ||
                !emit_array_alias(
                    output,
                    document,
                    method->throws_type,
                    error,
                    error_capacity)) {

                fclose(output);
                return false;
            }
        }
    }

    size_t state_count =
        document->type_count == 0u
            ? 1u
            : document->type_count;

    uint8_t states[state_count];

    memset(
        states,
        0,
        state_count);

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];


if (type->enum_count > 0u ||
            type->is_abstract ||
            type->stream != NULL) {


            continue;
        }

        if (!emit_struct_definition(
                output,
                document,
                i,
                states,
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }
    }


    /*
     * Reusable serializers are declared only after all concrete payload
     * definitions are complete, so hierarchy ref and payload types are fully
     * visible here.
     */
    char serializer_base[256];
    snake_name(
        base_name,
        serializer_base,
        sizeof(serializer_base));


    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        bool hierarchy =
            type_is_polymorphic_reference(
                document,
                type);

        bool concrete_struct =
            type->enum_count == 0u &&
            !type->is_abstract &&
            type->stream == NULL &&
            !hierarchy;

        if (!hierarchy &&
            !concrete_struct) {

            continue;
        }

        char type_name[256];

        declared_c_name(
            type->name,
            type_name,
            sizeof(type_name));

        if (hierarchy) {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_ref_t *value);\n\n",
                serializer_base,
                type_name,
                type_name);
        } else {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_t *value);\n\n",
                serializer_base,
                type_name,
                type_name);
        }
    }



    fprintf(
        output,
        "#endif\n");

    fclose(output);

    return true;
}



static bool emit_types_source(
    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(path, "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated types source");

        return false;
    }

    char base_snake[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    fprintf(
        output,
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n",
        base_snake);


    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        bool hierarchy =
            type_is_polymorphic_reference(
                document,
                type);

        bool concrete_struct =
            type->enum_count == 0u &&
            !type->is_abstract &&
            type->stream == NULL &&
            !hierarchy;

        if (!hierarchy &&
            !concrete_struct) {

            continue;
        }

        char type_name[256];

        declared_c_name(
            type->name,
            type_name,
            sizeof(type_name));

        if (hierarchy) {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_ref_t *value) {\n\n",
                base_snake,
                type_name,
                type_name);
        } else {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_t *value) {\n\n",
                base_snake,
                type_name,
                type_name);
        }

        fputs(
            "    if (position == NULL || value == NULL ||\n"
            "        (tx_capacity != 0u && tx == NULL)) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    if (*position > tx_capacity) {\n"
            "        return AETHER_ERR_OVERFLOW;\n"
            "    }\n\n"
            "    size_t pos = *position;\n",
            output);

        bool emitted;

        if (hierarchy) {
            emitted =
                emit_hierarchy_expression(
                    output,
                    document,
                    type,
                    "value",
                    "value",
                    true,
                    0u,
                    error,
                    error_capacity);
        } else {
            emitted =
                emit_value_expression(
                    output,
                    document,
                    type->name,
                    "value",
                    "value",
                    true,
                    0u,
                    error,
                    error_capacity);
        }

        if (!emitted) {
            fclose(output);
            return false;
        }

        fputs(
            "\n"
            "    *position = pos;\n"
            "    return AETHER_OK;\n"
            "}\n\n",
            output);
    }


    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated types source");

        fclose(output);
        return false;
    }

    fclose(output);
    return true;
}



static const adsl_api_t *find_api_definition(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u;
         i < document->api_count;
         ++i) {

        if (strcmp(
                document->apis[i].name,
                name) == 0) {

            return &document->apis[i];
        }
    }

    return NULL;
}

static bool stream_has_builder_api(
    const adsl_stream_t *stream) {

    if (stream == NULL) {
        return false;
    }

    if (stream->api != NULL &&
        *stream->api != '\0') {

        return true;
    }

    for (size_t i = 0u;
         i < stream->api_count;
         ++i) {

        if (stream->apis[i].api != NULL &&
            *stream->apis[i].api != '\0') {

            return true;
        }
    }

    return false;
}

static bool document_has_stream_builders(
    const adsl_document_t *document) {

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        if (stream_has_builder_api(
                document->types[i].stream)) {

            return true;
        }
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                if (stream_has_builder_api(
                        method->params[p].stream)) {

                    return true;
                }
            }
        }
    }

    return false;
}

static bool emit_stream_builder_declarations_for_api(
    FILE *output,
    const adsl_document_t *document,
    const char *stream_name,
    const char *api_reference,
    char *error,
    size_t error_capacity) {

    const adsl_api_t *api =
        find_api_definition(
            document,
            api_reference);

    if (api == NULL) {
        set_error(
            error,
            error_capacity,
            "stream api does not resolve to a declared API");

        return false;
    }

    char stream_c_name[256];
    char api_name[256];

    declared_c_name(
        stream_name,
        stream_c_name,
        sizeof(stream_c_name));

    snake_name(
        api->name,
        api_name,
        sizeof(api_name));

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        const adsl_method_t *method =
            &api->methods[m];

        bool has_response =
            method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL;

        char method_name[256];

        snake_name(
            method->name,
            method_name,
            sizeof(method_name));

        fprintf(
            output,

            "aether_status_t %s_%s_%s(\n"
            "    %s_t *builder_stream",
            stream_c_name,
            api_name,
            method_name,
            stream_c_name);


        if (has_response) {
            fputs(
                ",\n    uint32_t request_id",
                output);
        }

        for (size_t p = 0u;
             p < method->param_count;
             ++p) {

            fputs(
                ",\n    ",
                output);

            if (!emit_parameter_type(
                    output,
                    document,
                    &method->params[p],
                    error,
                    error_capacity)) {

                return false;
            }

            fprintf(
                output,
                " %s",
                method->params[p].name);
        }

        fputs(
            ");\n\n",
            output);
    }

    return !ferror(output);
}

static bool emit_stream_builder_declarations(
    FILE *output,
    const adsl_document_t *document,
    const char *stream_name,
    const adsl_stream_t *stream,
    char *error,
    size_t error_capacity) {

    if (stream == NULL) {
        return true;
    }

    if (stream->api != NULL &&
        *stream->api != '\0' &&
        !emit_stream_builder_declarations_for_api(
            output,
            document,
            stream_name,
            stream->api,
            error,
            error_capacity)) {

        return false;
    }

    for (size_t i = 0u;
         i < stream->api_count;
         ++i) {

        if (stream->apis[i].api == NULL ||
            *stream->apis[i].api == '\0') {

            continue;
        }

        if (!emit_stream_builder_declarations_for_api(
                output,
                document,
                stream_name,
                stream->apis[i].api,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}

static bool emit_all_stream_builder_declarations(
    FILE *output,
    const adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        if (!emit_stream_builder_declarations(
                output,
                document,
                type->name,
                type->stream,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                const adsl_param_t *param =
                    &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }

                if (!emit_stream_builder_declarations(
                        output,
                        document,
                        param->type,
                        param->stream,
                        error,
                        error_capacity)) {

                    return false;
                }
            }
        }
    }

    return true;
}

static bool emit_stream_builder_definitions_for_api(
    FILE *output,
    const adsl_document_t *document,
    const char *base_snake,
    const char *stream_name,
    const char *api_reference,
    char *error,
    size_t error_capacity) {

    const adsl_api_t *api =
        find_api_definition(
            document,
            api_reference);

    if (api == NULL) {
        set_error(
            error,
            error_capacity,
            "stream api does not resolve to a declared API");

        return false;
    }

    char stream_c_name[256];
    char api_name[256];

    declared_c_name(
        stream_name,
        stream_c_name,
        sizeof(stream_c_name));

    snake_name(
        api->name,
        api_name,
        sizeof(api_name));

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        const adsl_method_t *method =
            &api->methods[m];

        bool has_response =
            method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL;

        char method_name[256];

        snake_name(
            method->name,
            method_name,
            sizeof(method_name));

        fprintf(
            output,

            "aether_status_t %s_%s_%s(\n"
            "    %s_t *builder_stream",
            stream_c_name,
            api_name,
            method_name,
            stream_c_name);

        if (has_response) {
            fputs(
                ",\n    uint32_t request_id",
                output);
        }

        for (size_t p = 0u;
             p < method->param_count;
             ++p) {

            fputs(
                ",\n    ",
                output);

            if (!emit_parameter_type(
                    output,
                    document,
                    &method->params[p],
                    error,
                    error_capacity)) {

                return false;
            }

            fprintf(
                output,
                " %s",
                method->params[p].name);
        }

        fprintf(
            output,
            ") {\n\n"
            "    if (builder_stream == NULL ||\n"
            "        builder_stream->length > builder_stream->capacity) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    if (builder_stream->capacity > 0u &&\n"
            "        builder_stream->data == NULL) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    size_t remaining =\n"
            "        builder_stream->capacity - builder_stream->length;\n\n"
            "    if (remaining == 0u) {\n"
            "        return AETHER_ERR_OVERFLOW;\n"
            "    }\n\n"
            "    %s_stream_commit_t commit = {\n"
            "        &builder_stream->length,\n"
            "        remaining\n"
            "    };\n\n"
            "    %s_remote_t remote = {\n"
            "        &commit,\n"
            "        %s_stream_commit,\n"
            "        builder_stream->data + builder_stream->length,\n"
            "        remaining\n"
            "    };\n\n"
            "    return %s_%s(\n"
            "        &remote",
            base_snake,
            api_name,
            base_snake,
            api_name,
            method_name);



        if (has_response) {
            fputs(
                ",\n        request_id",
                output);
        }

        for (size_t p = 0u;
             p < method->param_count;
             ++p) {

            fprintf(
                output,
                ",\n        %s",
                method->params[p].name);
        }

        fputs(
            ");\n"
            "}\n\n",
            output);
    }

    return !ferror(output);
}

static bool emit_stream_builder_definitions(
    FILE *output,
    const adsl_document_t *document,
    const char *base_snake,
    const char *stream_name,
    const adsl_stream_t *stream,
    char *error,
    size_t error_capacity) {

    if (stream == NULL) {
        return true;
    }

    if (stream->api != NULL &&
        *stream->api != '\0' &&
        !emit_stream_builder_definitions_for_api(
            output,
            document,
            base_snake,
            stream_name,
            stream->api,
            error,
            error_capacity)) {

        return false;
    }

    for (size_t i = 0u;
         i < stream->api_count;
         ++i) {

        if (stream->apis[i].api == NULL ||
            *stream->apis[i].api == '\0') {

            continue;
        }

        if (!emit_stream_builder_definitions_for_api(
                output,
                document,
                base_snake,
                stream_name,
                stream->apis[i].api,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}

static bool emit_all_stream_builder_definitions(
    FILE *output,
    const adsl_document_t *document,
    const char *base_snake,
    char *error,
    size_t error_capacity) {

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *type =
            &document->types[i];

        if (!emit_stream_builder_definitions(
                output,
                document,
                base_snake,
                type->name,
                type->stream,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                const adsl_param_t *param =
                    &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }

                if (!emit_stream_builder_definitions(
                        output,
                        document,
                        base_snake,
                        param->type,
                        param->stream,
                        error,
                        error_capacity)) {

                    return false;
                }
            }
        }
    }

    return true;
}

static bool emit_api_header(
    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(path, "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated API header");

        return false;
    }

    char guard[256];
    char base_snake[256];

    upper_name(
        base_name,
        guard,
        sizeof(guard));

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));



    fprintf(
        output,
        "#ifndef %s_API_H\n"
        "#define %s_API_H\n\n"
        "#include \"%s_types.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n"
        "typedef aether_status_t (*%s_send_fn)(\n"
        "    void *ctx,\n"
        "    const uint8_t *data,\n"
        "    size_t length);\n\n",
        guard,
        guard,
        base_snake,
        base_snake);



    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        if (selected_api_name != NULL &&
            strcmp(
                api->name,
                selected_api_name) != 0) {

            continue;
        }

        char api_name[256];

        snake_name(
            api->name,
            api_name,
            sizeof(api_name));



        fprintf(
            output,
            "typedef struct {\n"
            "    void *send_ctx;\n"
            "    %s_send_fn send;\n"
            "    uint8_t *tx;\n"
            "    size_t tx_capacity;\n"
            "} %s_remote_t;\n\n",
            base_snake,
            api_name);



        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            if (selected_method_name != NULL &&
                strcmp(
                    method->name,
                    selected_method_name) != 0) {

                continue;
            }

            bool has_response =
                method->returns_type != NULL ||
                method->return_field_count > 0u ||
                method->throws_type != NULL;

            char method_name[256];

            snake_name(
                method->name,
                method_name,
                sizeof(method_name));

            fprintf(
                output,
                "aether_status_t %s_%s(\n"
                "    %s_remote_t *api",
                api_name,
                method_name,
                api_name);

            if (has_response) {
                fputs(
                    ",\n    uint32_t request_id",
                    output);
            }

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                fputs(
                    ",\n    ",
                    output);

                if (!emit_parameter_type(
                        output,
                        document,
                        &method->params[p],
                        error,
                        error_capacity)) {

                    fclose(output);
                    return false;
                }

                fprintf(
                    output,
                    " %s",
                    method->params[p].name);
            }

            fputs(
                ");\n\n",
                output);
        }
    }

    if (selected_api_name == NULL &&
        selected_method_name == NULL &&
        !emit_all_stream_builder_declarations(
            output,
            document,
            error,
            error_capacity)) {

        fclose(output);
        return false;
    }

    fprintf(
        output,
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n");

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated API header");

        fclose(output);
        return false;
    }

    fclose(output);

    return true;
}

static bool emit_api_source(
    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(path, "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated API source");

        return false;
    }

    char base_snake[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    fprintf(
        output,

        "#include \"%s_api.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n",

        base_snake);

    if (selected_api_name == NULL &&
        selected_method_name == NULL &&
        document_has_stream_builders(
            document)) {

        fprintf(
            output,
            "typedef struct {\n"
            "    size_t *length;\n"
            "    size_t remaining;\n"
            "} %s_stream_commit_t;\n\n"
            "static aether_status_t %s_stream_commit(\n"
            "    void *ctx,\n"
            "    const uint8_t *data,\n"
            "    size_t length) {\n\n"
            "    (void)data;\n\n"
            "    %s_stream_commit_t *commit =\n"
            "        ctx;\n\n"
            "    if (commit == NULL ||\n"
            "        commit->length == NULL) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    if (length > commit->remaining) {\n"
            "        return AETHER_ERR_OVERFLOW;\n"
            "    }\n\n"
            "    *commit->length += length;\n\n"
            "    return AETHER_OK;\n"
            "}\n\n",
            base_snake,
            base_snake,
            base_snake);
    }

    for (size_t a = 0u;
         a < document->api_count;
         ++a) {

        const adsl_api_t *api =
            &document->apis[a];

        if (selected_api_name != NULL &&
            strcmp(
                api->name,
                selected_api_name) != 0) {

            continue;
        }

        char api_name[256];

        snake_name(
            api->name,
            api_name,
            sizeof(api_name));

        for (size_t m = 0u;
             m < api->method_count;
             ++m) {

            const adsl_method_t *method =
                &api->methods[m];

            if (selected_method_name != NULL &&
                strcmp(
                    method->name,
                    selected_method_name) != 0) {

                continue;
            }

            bool has_response =
                method->returns_type != NULL ||
                method->return_field_count > 0u ||
                method->throws_type != NULL;

            char method_name[256];

            snake_name(
                method->name,
                method_name,
                sizeof(method_name));

            fprintf(
                output,
                "aether_status_t %s_%s(\n"
                "    %s_remote_t *api",
                api_name,
                method_name,
                api_name);

            if (has_response) {
                fputs(
                    ",\n    uint32_t request_id",
                    output);
            }

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                fputs(
                    ",\n    ",
                    output);

                if (!emit_parameter_type(
                        output,
                        document,
                        &method->params[p],
                        error,
                        error_capacity)) {

                    fclose(output);
                    return false;
                }

                fprintf(
                    output,
                    " %s",
                    method->params[p].name);
            }

            fprintf(
                output,


                ") {\n\n"
                "    if (api == NULL ||\n"
                "        api->send == NULL ||\n"
                "        api->tx == NULL) {\n"
                "        return AETHER_ERR_ARGUMENT;\n"
                "    }\n\n"

                "    uint8_t *tx = api->tx;\n"
                "    size_t tx_capacity = api->tx_capacity;\n\n"

                "    if (api->tx_capacity < %uu) {\n"
                "        return AETHER_ERR_OVERFLOW;\n"
                "    }\n\n"
                "    size_t pos = 0u;\n"
                "    api->tx[pos++] = %uu;\n",


                has_response
                    ? 5u
                    : 1u,
                (unsigned int)method->id);

            if (has_response) {
                fputs(
                    "    api->tx[pos++] = "
                    "(uint8_t)((request_id >> 0u) & UINT32_C(0xff));\n"
                    "    api->tx[pos++] = "
                    "(uint8_t)((request_id >> 8u) & UINT32_C(0xff));\n"
                    "    api->tx[pos++] = "
                    "(uint8_t)((request_id >> 16u) & UINT32_C(0xff));\n"
                    "    api->tx[pos++] = "
                    "(uint8_t)((request_id >> 24u) & UINT32_C(0xff));\n",
                    output);
            }

            for (size_t p = 0u;
                 p < method->param_count;
                 ++p) {

                if (!emit_serialize_param(
                        output,
                        document,
                        base_snake,
                        &method->params[p],
                        error,
                        error_capacity)) {

                    fclose(output);
                    return false;
                }
            }



            fprintf(
                output,
                "\n"
                "    return api->send(\n"
                "        api->send_ctx,\n"
                "        api->tx,\n"
                "        pos);\n"
                "}\n\n");


        }
    }

    if (selected_api_name == NULL &&
        selected_method_name == NULL &&
        !emit_all_stream_builder_definitions(
            output,
            document,
            base_snake,
            error,
            error_capacity)) {

        fclose(output);
        return false;
    }

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated API source");

        fclose(output);
        return false;
    }

    fclose(output);

    return true;
}


bool adsl_emit_c_method(
    const adsl_document_t *document,
    const char *output_directory,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    if (document == NULL ||
        output_directory == NULL ||
        base_name == NULL ||
        (selected_method_name != NULL &&
         selected_api_name == NULL)) {

        set_error(
            error,
            error_capacity,
            "invalid emitter arguments");

        return false;
    }

    if (selected_api_name != NULL) {
        const adsl_api_t *selected_api =
            find_api_definition(
                document,
                selected_api_name);

        if (selected_api == NULL) {
            set_error(
                error,
                error_capacity,
                "selected API not found");

            return false;
        }

        if (selected_method_name != NULL) {
            bool method_found = false;

            for (size_t m = 0u;
                 m < selected_api->method_count;
                 ++m) {

                if (strcmp(
                        selected_api->methods[m].name,
                        selected_method_name) == 0) {

                    method_found = true;
                    break;
                }
            }

            if (!method_found) {
                set_error(
                    error,
                    error_capacity,
                    "selected API method not found");

                return false;
            }
        }
    }

    char base_snake[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    char types_h[1024];
    char types_c[1024];
    char api_h[1024];
    char api_c[1024];

    (void)snprintf(
        types_h,
        sizeof(types_h),
        "%s/%s_types.h",
        output_directory,
        base_snake);

    (void)snprintf(
        types_c,
        sizeof(types_c),
        "%s/%s_types.c",
        output_directory,
        base_snake);

    (void)snprintf(
        api_h,
        sizeof(api_h),
        "%s/%s_api.h",
        output_directory,
        base_snake);

    (void)snprintf(
        api_c,
        sizeof(api_c),
        "%s/%s_api.c",
        output_directory,
        base_snake);

    if (!emit_types_header(
            document,
            types_h,
            base_name,
            error,
            error_capacity)) {

        return false;
    }


    if (!emit_types_source(
            document,
            types_c,
            base_name,
            error,
            error_capacity)) {

        return false;
    }


    if (!emit_api_header(
            document,
            api_h,
            base_name,
            selected_api_name,
            selected_method_name,
            error,
            error_capacity)) {

        return false;
    }

    return emit_api_source(
        document,
        api_c,
        base_name,
        selected_api_name,
        selected_method_name,
        error,
        error_capacity);
}

bool adsl_emit_c(

    const adsl_document_t *document,
    const char *output_directory,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    return adsl_emit_c_method(
        document,
        output_directory,
        base_name,
        NULL,
        NULL,
        error,
        error_capacity);
}


static bool emit_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated value serialization depth exceeds 64");

        return false;
    }

    if (strchr(type, '~') != NULL) {
        set_error(
            error,
            error_capacity,
            "milestone-2 emitter does not yet serialize collapsible values");

        return false;
    }

    if (strchr(type, '?') != NULL) {
        set_error(
            error,
            error_capacity,
            "nullable values must be serialized by their containing field mask");

        return false;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            type,
            array_element,
            sizeof(array_element),
            &array_dynamic,
            &array_static_size)) {

        (void)array_element;
        (void)array_dynamic;
        (void)array_static_size;

        return emit_array_expression(
            output,
            document,
            type,
            expression,
            prefix,
            struct_pointer,
            depth,
            error,
            error_capacity);
    }

    char base[128];

    if (!scalar_type_name(
            type,
            base,
            sizeof(base))) {

        set_error(
            error,
            error_capacity,
            "milestone-2 emitter supports scalar or one-dimensional array values only");

        return false;
    }

    if (emit_fixed_expression(
            output,
            base,
            expression,
            prefix)) {

        return true;
    }

    if (strcmp(base, "uuid") == 0) {
        emit_uuid_expression(
            output,
            expression);

        return true;
    }

    if (strcmp(base, "string") == 0 ||
        strcmp(base, "uri") == 0) {

        emit_bytes_view_expression(
            output,
            expression,
            prefix);

        return true;
    }

    if (strcmp(base, "intpack") == 0) {
        fprintf(
            output,
            "    if (%s < 0) return AETHER_ERR_ARGUMENT;\n"
            "    uint64_t %s_pack_value = (uint64_t)%s;\n",
            expression,
            prefix,
            expression);

        char value_name[512];

        int value_name_size =
            snprintf(
                value_name,
                sizeof(value_name),
                "%s_pack_value",
                prefix);

        if (value_name_size < 0 ||
            (size_t)value_name_size >=
                sizeof(value_name)) {

            set_error(
                error,
                error_capacity,
                "generated intpack value name is too long");

            return false;
        }

        emit_pack_value(
            output,
            value_name,
            prefix);

        return true;
    }

    if (primitive_c_type(base) != NULL) {
        set_error(
            error,
            error_capacity,
            "milestone-2 emitter does not yet serialize this primitive");

        return false;
    }

    const adsl_type_t *declared_type =
        find_declared_type(
            document,
            base);

    if (declared_type == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated value type");

        return false;

}

    if (declared_type->stream != NULL) {
        if (struct_pointer) {
            fprintf(
                output,
                "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n"
                "    if (%s->length > %s->capacity) return AETHER_ERR_ARGUMENT;\n",
                expression,
                expression,
                expression);

            char stream_expression[512];

            int stream_expression_size =
                snprintf(
                    stream_expression,
                    sizeof(stream_expression),
                    "(*%s)",
                    expression);

            if (stream_expression_size < 0 ||
                (size_t)stream_expression_size >=
                    sizeof(stream_expression)) {

                set_error(
                    error,
                    error_capacity,
                    "generated stream expression is too long");

                return false;
            }

            emit_bytes_view_expression(
                output,
                stream_expression,
                prefix);
        } else {
            fprintf(
                output,
                "    if (%s.length > %s.capacity) return AETHER_ERR_ARGUMENT;\n",
                expression,
                expression);

            emit_bytes_view_expression(
                output,
                expression,
                prefix);
        }

        return true;
    }

    if (declared_type->enum_count > 0u) {

        fprintf(
            output,

            "    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;\n"
            "    tx[pos++] = (uint8_t)%s;\n",

            expression);

        return true;
    }

    if (type_is_polymorphic_reference(
            document,
            declared_type)) {

        return emit_hierarchy_expression(
            output,
            document,
            declared_type,
            expression,
            prefix,
            struct_pointer,
            depth,
            error,
            error_capacity);
    }

    if (declared_type->is_abstract) {
        set_error(
            error,
            error_capacity,
            "abstract generated value has no concrete hierarchy descendants");

        return false;
    }

    return emit_struct_value_expression(
        output,
        document,
        declared_type,
        expression,
        prefix,
        struct_pointer,
        depth,
        error,
        error_capacity);
}

static bool emit_struct_definition(
    FILE *output,
    const adsl_document_t *document,
    size_t type_index,
    uint8_t *states,
    char *error,
    size_t error_capacity) {

    if (states[type_index] == 2u) {
        return true;
    }

    if (states[type_index] == 1u) {
        set_error(
            error,
            error_capacity,
            "recursive generated struct dependency is not yet supported");

        return false;
    }

    const adsl_type_t *type =
        &document->types[type_index];


if (type->enum_count > 0u ||
        type->is_abstract ||
        type->stream != NULL) {


        states[type_index] =
            2u;

        return true;
    }

    states[type_index] =
        1u;

    if (!emit_all_struct_dependencies(
            output,
            document,
            type,
            states,
            0u,
            error,
            error_capacity)) {

        return false;
    }

    char type_name[256];


declared_c_name(
        type->name,
        type_name,
        sizeof(type_name));


    fprintf(
        output,
        "struct %s_t {\n",
        type_name);

    if (!emit_all_struct_field_declarations(
            output,
            document,
            type,
            0u,
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        "};\n\n");

    states[type_index] =
        2u;

    return true;
}


static bool emit_array_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *expression,
    const char *prefix,
    bool array_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated array serialization depth exceeds 64");

        return false;
    }

    char element[128];
    bool dynamic;
    size_t static_size;

    if (!parse_array_type(
            type,
            element,
            sizeof(element),
            &dynamic,
            &static_size)) {

        set_error(
            error,
            error_capacity,
            "invalid generated array type");

        return false;
    }

    if (array_pointer) {
        fprintf(
            output,
            "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n",
            expression);
    }

    char value_expression[512];

    int value_size;

    if (array_pointer) {
        value_size =
            snprintf(
                value_expression,
                sizeof(value_expression),
                "(*%s)",
                expression);
    } else {
        value_size =
            snprintf(
                value_expression,
                sizeof(value_expression),
                "%s",
                expression);
    }

    if (value_size < 0 ||
        (size_t)value_size >=
            sizeof(value_expression)) {

        set_error(
            error,
            error_capacity,
            "generated array expression is too long");

        return false;
    }

    if (dynamic &&
        strcmp(element, "byte") == 0) {

        emit_bytes_view_expression(
            output,
            value_expression,
            prefix);

        return true;
    }

    if (!dynamic &&
        strcmp(element, "byte") == 0) {

        fprintf(
            output,

            "    if (pos + %zuu > tx_capacity) return AETHER_ERR_OVERFLOW;\n"
            "    memcpy(tx + pos, %s.data, %zuu);\n"

            "    pos += %zuu;\n",
            static_size,
            value_expression,
            static_size,
            static_size);

        return true;
    }

    if (dynamic) {
        fprintf(
            output,
            "    if (%s.length > 0u && %s.data == NULL) return AETHER_ERR_ARGUMENT;\n"
            "    uint64_t %s_length = (uint64_t)%s.length;\n",
            value_expression,
            value_expression,
            prefix,
            value_expression);

        char length_name[512];

        int length_size =
            snprintf(
                length_name,
                sizeof(length_name),
                "%s_length",
                prefix);

        if (length_size < 0 ||
            (size_t)length_size >=
                sizeof(length_name)) {

            set_error(
                error,
                error_capacity,
                "generated array length name is too long");

            return false;
        }

        emit_pack_value(
            output,
            length_name,
            prefix);
    }

    char index_name[512];

    int index_size =
        snprintf(
            index_name,
            sizeof(index_name),
            "%s_index",
            prefix);

    if (index_size < 0 ||
        (size_t)index_size >=
            sizeof(index_name)) {

        set_error(
            error,
            error_capacity,
            "generated array index name is too long");

        return false;
    }

    if (dynamic) {
        fprintf(
            output,
            "    for (size_t %s = 0u; %s < %s.length; ++%s) {\n",
            index_name,
            index_name,
            value_expression,
            index_name);
    } else {
        fprintf(
            output,
            "    for (size_t %s = 0u; %s < %zuu; ++%s) {\n",
            index_name,
            index_name,
            static_size,
            index_name);
    }

    char element_expression[768];

    int element_size =
        snprintf(
            element_expression,
            sizeof(element_expression),
            "%s.data[%s]",
            value_expression,
            index_name);

    if (element_size < 0 ||
        (size_t)element_size >=
            sizeof(element_expression)) {

        set_error(
            error,
            error_capacity,
            "generated array element expression is too long");

        return false;
    }

    char element_prefix[768];

    int prefix_size =
        snprintf(
            element_prefix,
            sizeof(element_prefix),
            "%s_element",
            prefix);

    if (prefix_size < 0 ||
        (size_t)prefix_size >=
            sizeof(element_prefix)) {

        set_error(
            error,
            error_capacity,
            "generated array element prefix is too long");

        return false;
    }

    if (!emit_value_expression(
            output,
            document,
            element,
            element_expression,
            element_prefix,
            false,
            depth + 1u,
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        "    }\n");

    return true;
}

static bool emit_struct_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *declared_type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "nested struct serialization depth exceeds 64");

        return false;
    }

    if (struct_pointer) {
        fprintf(
            output,
            "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n",
            expression);
    }

    size_t nullable_count =
        0u;

    if (!count_all_nullable_fields(
            document,
            declared_type,
            0u,
            &nullable_count,
            error,
            error_capacity)) {

        return false;
    }

    if (nullable_count > 64u) {
        set_error(
            error,
            error_capacity,
            "generated struct hierarchy has more than 64 nullable fields");

        return false;
    }

    if (nullable_count > 0u) {
        size_t mask_width =
            nullable_count <= 8u
                ? 1u
                : nullable_count <= 16u
                    ? 2u
                    : nullable_count <= 32u
                        ? 4u
                        : 8u;

        fprintf(
            output,
            "    uint64_t %s_nullable_mask = UINT64_C(0);\n",
            prefix);

        size_t bit_index =
            0u;

        if (!emit_all_nullable_mask_bits(
                output,
                document,
                declared_type,
                expression,
                prefix,
                struct_pointer,
                0u,
                &bit_index,
                error,
                error_capacity)) {

            return false;
        }

        fprintf(
            output,
            "    if (pos + %zuu > tx_capacity) return AETHER_ERR_OVERFLOW;\n",
            mask_width);

        for (size_t b = 0u;
             b < mask_width;
             ++b) {

            fprintf(
                output,
                "    tx[pos++] = (uint8_t)"
                "((%s_nullable_mask >> %zuu) & UINT64_C(0xff));\n",
                prefix,
                b * 8u);
        }
    }

    return emit_all_struct_payload(
        output,
        document,
        declared_type,
        expression,
        prefix,
        struct_pointer,
        depth,
        error,
        error_capacity);
}


static bool type_has_children(
    const adsl_document_t *document,
    const adsl_type_t *type) {

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *candidate =
            &document->types[i];

        if (candidate->parent == NULL) {
            continue;
        }

        const adsl_type_t *parent =
            find_declared_type(
                document,
                candidate->parent);

        if (parent == type) {
            return true;
        }
    }

    return false;
}

static bool type_is_polymorphic_reference(
    const adsl_document_t *document,
    const adsl_type_t *type) {

    return type != NULL &&
           type->enum_count == 0u &&
           (type->is_abstract ||
            type_has_children(
                document,
                type));
}

static bool type_is_descendant_or_self(
    const adsl_document_t *document,
    const adsl_type_t *candidate,
    const adsl_type_t *ancestor) {

    const adsl_type_t *current =
        candidate;

    for (unsigned int depth = 0u;
         current != NULL &&
         depth <= 64u;
         ++depth) {

        if (current == ancestor) {
            return true;
        }

        if (current->parent == NULL) {
            return false;
        }

        current =
            find_declared_type(
                document,
                current->parent);
    }

    return false;
}

static bool emit_hierarchy_ref(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    char *error,
    size_t error_capacity) {

    if (!type_is_polymorphic_reference(
            document,
            type)) {

        return true;
    }

    char static_name[256];


declared_c_name(
        type->name,
        static_name,
        sizeof(static_name));


    size_t concrete_count =
        0u;

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *candidate =
            &document->types[i];

        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self(
                document,
                candidate,
                type)) {

            continue;
        }

        concrete_count++;

        if (!candidate->has_id ||
            candidate->id == 0u) {

            char message[512];

            int written =
                snprintf(
                    message,
                    sizeof(message),
                    "concrete hierarchy type '%s' must have a non-zero one-byte id",
                    candidate->name);

            if (written < 0 ||
                (size_t)written >= sizeof(message)) {

                set_error(
                    error,
                    error_capacity,
                    "concrete hierarchy type must have a non-zero one-byte id");
            } else {
                set_error(
                    error,
                    error_capacity,
                    message);
            }

            return false;
        }

        for (size_t j = i + 1u;
             j < document->type_count;
             ++j) {

            const adsl_type_t *other =
                &document->types[j];

            if (other->enum_count > 0u ||
                other->is_abstract ||
                !type_is_descendant_or_self(
                    document,
                    other,
                    type)) {

                continue;
            }

            if (other->has_id &&
                other->id == candidate->id) {

                char message[768];

                int written =
                    snprintf(
                        message,
                        sizeof(message),
                        "hierarchy '%s' has duplicate concrete type id %u for '%s' and '%s'",
                        type->name,
                        (unsigned int)candidate->id,
                        candidate->name,
                        other->name);

                if (written < 0 ||
                    (size_t)written >= sizeof(message)) {

                    set_error(
                        error,
                        error_capacity,
                        "hierarchy has duplicate concrete type ids");
                } else {
                    set_error(
                        error,
                        error_capacity,
                        message);
                }

                return false;
            }
        }
    }

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *candidate =
            &document->types[i];

        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self(
                document,
                candidate,
                type)) {

            continue;
        }


char static_upper[256];
        char candidate_upper[256];
        char static_c_name[256];
        char candidate_c_name[256];

        declared_c_name(
            type->name,
            static_c_name,
            sizeof(static_c_name));

        declared_c_name(
            candidate->name,
            candidate_c_name,
            sizeof(candidate_c_name));

        upper_name(
            static_c_name,
            static_upper,
            sizeof(static_upper));

        upper_name(
            candidate_c_name,
            candidate_upper,
            sizeof(candidate_upper));


        fprintf(
            output,
            "#define %s_TYPE_%s UINT8_C(%u)\n",
            static_upper,
            candidate_upper,
            (unsigned int)candidate->id);
    }

    if (concrete_count > 0u) {
        fputc(
            '\n',
            output);
    }

    fprintf(
        output,
        "typedef struct {\n"
        "    uint8_t type_id;\n");

    if (concrete_count == 0u) {
        fputs(
            "    const void *value;\n",
            output);
    } else {
        fputs(
            "    union {\n",
            output);

        for (size_t i = 0u;
             i < document->type_count;
             ++i) {

            const adsl_type_t *candidate =
                &document->types[i];

            if (candidate->enum_count > 0u ||
                candidate->is_abstract ||
                !type_is_descendant_or_self(
                    document,
                    candidate,
                    type)) {

                continue;
            }

            char candidate_name[256];


declared_c_name(
                candidate->name,
                candidate_name,
                sizeof(candidate_name));


            fprintf(
                output,
                "        const %s_t *%s;\n",
                candidate_name,
                candidate_name);
        }

        fputs(
            "    } as;\n",
            output);
    }

    fprintf(
        output,
        "} %s_ref_t;\n\n",
        static_name);

    return true;
}

static bool emit_hierarchy_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *static_type,
    const char *expression,
    const char *prefix,
    bool reference_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "hierarchy serialization depth exceeds 64");

        return false;
    }

    if (reference_pointer) {
        fprintf(
            output,
            "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n",
            expression);
    }

    char value_expression[768];

    int value_size =
        snprintf(
            value_expression,
            sizeof(value_expression),
            reference_pointer
                ? "(*%s)"
                : "%s",
            expression);

    if (value_size < 0 ||
        (size_t)value_size >=
            sizeof(value_expression)) {

        set_error(
            error,
            error_capacity,
            "generated hierarchy expression is too long");

        return false;
    }

    fprintf(
        output,

        "    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;\n"
        "    tx[pos++] = (uint8_t)%s.type_id;\n"

        "    switch (%s.type_id) {\n",
        value_expression,
        value_expression);

    size_t concrete_count =
        0u;

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        const adsl_type_t *candidate =
            &document->types[i];

        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self(
                document,
                candidate,
                static_type)) {

            continue;
        }

        concrete_count++;

        char candidate_name[256];


declared_c_name(
            candidate->name,
            candidate_name,
            sizeof(candidate_name));


        char child_prefix[768];

        int prefix_size =
            snprintf(
                child_prefix,
                sizeof(child_prefix),
                "%s_%s",
                prefix,
                candidate_name);

        if (prefix_size < 0 ||
            (size_t)prefix_size >=
                sizeof(child_prefix)) {

            set_error(
                error,
                error_capacity,
                "generated hierarchy child prefix is too long");

            return false;
        }

        char child_expression[1024];

        int child_size =
            snprintf(
                child_expression,
                sizeof(child_expression),
                "%s.as.%s",
                value_expression,
                candidate_name);

        if (child_size < 0 ||
            (size_t)child_size >=
                sizeof(child_expression)) {

            set_error(
                error,
                error_capacity,
                "generated hierarchy child expression is too long");

            return false;
        }

        fprintf(
            output,
            "    case %uu:\n"
            "        if (%s == NULL) return AETHER_ERR_ARGUMENT;\n",
            (unsigned int)candidate->id,
            child_expression);

        if (!emit_struct_value_expression(
                output,
                document,
                candidate,
                child_expression,
                child_prefix,
                true,
                depth + 1u,
                error,
                error_capacity)) {

            return false;
        }

        fputs(
            "        break;\n",
            output);
    }

    if (concrete_count == 0u) {
        fputs(
            "    default:\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n",
            output);

        return true;
    }

    fputs(
        "    default:\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n",
        output);

    return true;
}

static bool emit_all_struct_dependencies(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    uint8_t *states,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");

        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(
                document,
                type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");

            return false;
        }

        if (!emit_all_struct_dependencies(
                output,
                document,
                parent,
                states,
                depth + 1u,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u;
         f < type->field_count;
         ++f) {

        const adsl_field_t *field =
            &type->fields[f];

        if (strchr(field->type, '~') != NULL) {
            set_error(
                error,
                error_capacity,
                "milestone-2 emitter does not yet emit collapsible struct fields");

            return false;
        }

        char array_element[128];
        bool array_dynamic;
        size_t array_static_size;

        if (parse_array_type(
                field->type,
                array_element,
                sizeof(array_element),
                &array_dynamic,
                &array_static_size)) {

            const adsl_type_t *element_type =
                find_declared_type(
                    document,
                    array_element);

            if (!array_dynamic &&
                element_type != NULL &&
                element_type->enum_count == 0u &&
                !type_is_polymorphic_reference(
                    document,
                    element_type)) {

                set_error(
                    error,
                    error_capacity,
                    "milestone-2 static arrays of concrete generated structs are not yet supported");

                return false;
            }

            continue;
        }

        char base[128];

        if (!scalar_type_name(
                field->type,
                base,
                sizeof(base))) {

            set_error(
                error,
                error_capacity,
                "milestone-2 emitter supports scalar or one-dimensional array struct fields only");

            return false;
        }

        if (primitive_c_type(base) != NULL) {
            continue;
        }

        const adsl_type_t *field_type =
            find_declared_type(
                document,
                base);

        if (field_type == NULL) {
            set_error(
                error,
                error_capacity,
                "unknown generated struct field type");

            return false;
        }

        if (field_type->enum_count > 0u ||
            type_is_polymorphic_reference(
                document,
                field_type)) {

            continue;
        }

        if (field_type->is_abstract) {
            set_error(
                error,
                error_capacity,
                "abstract struct field has no concrete hierarchy descendants");

            return false;
        }

        size_t nested_index =
            (size_t)(
                field_type -
                document->types);

        if (!emit_struct_definition(
                output,
                document,
                nested_index,
                states,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}

static bool emit_all_struct_field_declarations(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");

        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(
                document,
                type->parent);

        if (parent == NULL ||
            !emit_all_struct_field_declarations(
                output,
                document,
                parent,
                depth + 1u,
                error,
                error_capacity)) {

            if (parent == NULL) {
                set_error(
                    error,
                    error_capacity,
                    "generated struct has unknown parent");
            }

            return false;
        }
    }

    for (size_t f = 0u;
         f < type->field_count;
         ++f) {

        const adsl_field_t *field =
            &type->fields[f];

        bool nullable =
            strchr(
                field->type,
                '?') != NULL;

        char array_element[128];
        bool array_dynamic;
        size_t array_static_size;

        if (parse_array_type(
                field->type,
                array_element,
                sizeof(array_element),
                &array_dynamic,
                &array_static_size)) {

            (void)array_element;
            (void)array_dynamic;
            (void)array_static_size;

            char alias[256];

            if (!array_alias_name(
                    document,
                    field->type,
                    alias,
                    sizeof(alias))) {

                set_error(
                    error,
                    error_capacity,
                    "generated struct array alias is too long");

                return false;
            }

            fprintf(
                output,
                nullable
                    ? "    const %s *%s;\n"
                    : "    %s %s;\n",
                alias,
                field->name);

            continue;
        }

        char base[128];

        if (!scalar_type_name(
                field->type,
                base,
                sizeof(base))) {

            set_error(
                error,
                error_capacity,
                "milestone-2 emitter supports scalar or one-dimensional array struct fields only");

            return false;
        }

        const char *c_type =
            primitive_c_type(base);

        if (c_type != NULL) {
            fprintf(
                output,
                nullable
                    ? "    const %s *%s;\n"
                    : "    %s %s;\n",
                c_type,
                field->name);

            continue;
        }

        const adsl_type_t *field_type =
            find_declared_type(
                document,
                base);

        if (field_type == NULL) {
            set_error(
                error,
                error_capacity,
                "unknown generated struct field type");

            return false;
        }

        char field_type_name[256];


declared_c_name(
            field_type->name,
            field_type_name,
            sizeof(field_type_name));


        if (field_type->enum_count > 0u) {
            fprintf(
                output,
                nullable
                    ? "    const %s_t *%s;\n"
                    : "    %s_t %s;\n",
                field_type_name,
                field->name);

            continue;
        }

        if (type_is_polymorphic_reference(
                document,
                field_type)) {

            fprintf(
                output,
                nullable
                    ? "    const %s_ref_t *%s;\n"
                    : "    %s_ref_t %s;\n",
                field_type_name,
                field->name);

            continue;
        }

        if (field_type->is_abstract) {
            set_error(
                error,
                error_capacity,
                "abstract struct field has no concrete hierarchy descendants");

            return false;
        }

        fprintf(
            output,
            nullable
                ? "    const %s_t *%s;\n"
                : "    %s_t %s;\n",
            field_type_name,
            field->name);
    }

    return true;
}

static bool count_all_nullable_fields(
    const adsl_document_t *document,
    const adsl_type_t *type,
    unsigned int depth,
    size_t *count,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");

        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(
                document,
                type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");

            return false;
        }

        if (!count_all_nullable_fields(
                document,
                parent,
                depth + 1u,
                count,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u;
         f < type->field_count;
         ++f) {

        if (strchr(
                type->fields[f].type,
                '?') != NULL) {

            (*count)++;
        }
    }

    return true;
}

static bool emit_all_nullable_mask_bits(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    size_t *bit_index,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");

        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(
                document,
                type->parent);

        if (parent == NULL ||
            !emit_all_nullable_mask_bits(
                output,
                document,
                parent,
                expression,
                prefix,
                struct_pointer,
                depth + 1u,
                bit_index,
                error,
                error_capacity)) {

            if (parent == NULL) {
                set_error(
                    error,
                    error_capacity,
                    "generated struct has unknown parent");
            }

            return false;
        }
    }

    for (size_t f = 0u;
         f < type->field_count;
         ++f) {

        const adsl_field_t *field =
            &type->fields[f];

        if (strchr(
                field->type,
                '?') == NULL) {

            continue;
        }

        char field_expression[768];

        int expression_size =
            snprintf(
                field_expression,
                sizeof(field_expression),
                "%s%s%s",
                expression,
                struct_pointer
                    ? "->"
                    : ".",
                field->name);

        if (expression_size < 0 ||
            (size_t)expression_size >=
                sizeof(field_expression)) {

            set_error(
                error,
                error_capacity,
                "generated nullable inherited field expression is too long");

            return false;
        }

        fprintf(
            output,
            "    if (%s == NULL) %s_nullable_mask |= "
            "(UINT64_C(1) << %zuu);\n",
            field_expression,
            prefix,
            *bit_index);

        (*bit_index)++;
    }

    return true;
}

static bool emit_all_struct_payload(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");

        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(
                document,
                type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");

            return false;
        }

        if (!emit_all_struct_payload(
                output,
                document,
                parent,
                expression,
                prefix,
                struct_pointer,
                depth + 1u,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u;
         f < type->field_count;
         ++f) {

        const adsl_field_t *field =
            &type->fields[f];

        if (strchr(field->type, '~') != NULL) {
            set_error(
                error,
                error_capacity,
                "milestone-2 emitter does not yet serialize collapsible struct fields");

            return false;
        }

        bool nullable =
            strchr(
                field->type,
                '?') != NULL;

        char field_expression[768];

        int expression_size =
            snprintf(
                field_expression,
                sizeof(field_expression),
                "%s%s%s",
                expression,
                struct_pointer
                    ? "->"
                    : ".",
                field->name);

        if (expression_size < 0 ||
            (size_t)expression_size >=
                sizeof(field_expression)) {

            set_error(
                error,
                error_capacity,
                "generated inherited field expression is too long");

            return false;
        }

        char field_prefix[768];

        int prefix_size =
            snprintf(
                field_prefix,
                sizeof(field_prefix),
                "%s_%s",
                prefix,
                field->name);

        if (prefix_size < 0 ||
            (size_t)prefix_size >=
                sizeof(field_prefix)) {

            set_error(
                error,
                error_capacity,
                "generated inherited field prefix is too long");

            return false;
        }

        if (!nullable) {
            if (!emit_value_expression(
                    output,
                    document,
                    field->type,
                    field_expression,
                    field_prefix,
                    false,
                    depth + 1u,
                    error,
                    error_capacity)) {

                return false;
            }

            continue;
        }

        char underlying_type[256];

        if (!strip_nullable_suffix(
                field->type,
                underlying_type,
                sizeof(underlying_type))) {

            set_error(
                error,
                error_capacity,
                "generated nullable inherited field type is too long");

            return false;
        }

        fprintf(
            output,
            "    if (%s != NULL) {\n",
            field_expression);

        char array_element[128];
        bool array_dynamic;
        size_t array_static_size;

        bool field_is_array =
            parse_array_type(
                underlying_type,
                array_element,
                sizeof(array_element),
                &array_dynamic,
                &array_static_size);

        (void)array_element;
        (void)array_dynamic;
        (void)array_static_size;

        bool pointer_value =
            field_is_array;

        if (!field_is_array) {
            char base[128];

            if (!scalar_type_name(
                    underlying_type,
                    base,
                    sizeof(base))) {

                set_error(
                    error,
                    error_capacity,
                    "milestone-2 nullable inherited field has unsupported type");

                return false;
            }

            const adsl_type_t *field_declared_type =
                find_declared_type(
                    document,
                    base);

            pointer_value =
                field_declared_type != NULL &&
                field_declared_type->enum_count == 0u;
        }

        char nullable_expression[1024];

        int nullable_size =
            snprintf(
                nullable_expression,
                sizeof(nullable_expression),
                pointer_value
                    ? "%s"
                    : "(*%s)",
                field_expression);

        if (nullable_size < 0 ||
            (size_t)nullable_size >=
                sizeof(nullable_expression)) {

            set_error(
                error,
                error_capacity,
                "generated nullable inherited value expression is too long");

            return false;
        }

        if (!emit_value_expression(
                output,
                document,
                underlying_type,
                nullable_expression,
                field_prefix,
                pointer_value,
                depth + 1u,
                error,
                error_capacity)) {

            return false;
        }

        fprintf(
            output,
            "    }\n");
    }

    return true;
}



static bool local_api_supported(
    const adsl_api_t *api,
    char *error,
    size_t error_capacity) {

    if (api == NULL) {
        set_error(
            error,
            error_capacity,
            "selected local API not found");

        return false;
    }

    if (api->method_count == 0u) {
        set_error(
            error,
            error_capacity,
            "selected local API has no methods");

        return false;
    }

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        const adsl_method_t *method =
            &api->methods[m];

        if (!method->has_id) {
            set_error(
                error,
                error_capacity,
                "local API method has no id");

            return false;
        }

        if (method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL) {

            set_error(
                error,
                error_capacity,
                "local stream dispatch currently supports fire-and-forget methods only");

            return false;
        }

        if (method->param_count != 1u ||
            method->params[0].stream == NULL ||
            method->params[0].type == NULL ||
            method->params[0].name == NULL) {

            set_error(
                error,
                error_capacity,
                "local stream dispatch requires exactly one stream parameter");

            return false;
        }
    }

    return true;
}


static bool emit_local_api_header(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    (void)document;

    FILE *output =
        fopen(
            path,
            "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated local API header");

        return false;
    }

    char guard[256];
    char base_snake[256];
    char api_name[256];

    upper_name(
        base_name,
        guard,
        sizeof(guard));

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    snake_name(
        api->name,
        api_name,
        sizeof(api_name));

    fprintf(
        output,
        "#ifndef %s_LOCAL_H\n"
        "#define %s_LOCAL_H\n\n"
        "#include \"%s_types.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n",
        guard,
        guard,
        base_snake);

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        const adsl_method_t *method =
            &api->methods[m];

        const adsl_param_t *param =
            &method->params[0];

        char method_name[256];
        char stream_name[256];

        snake_name(
            method->name,
            method_name,
            sizeof(method_name));

        declared_c_name(
            param->type,
            stream_name,
            sizeof(stream_name));

        fprintf(
            output,
            "typedef aether_status_t (*%s_%s_fn)(\n"
            "    void *ctx,\n"
            "    const %s_t *%s);\n\n",
            api_name,
            method_name,
            stream_name,
            param->name);
    }

    fprintf(
        output,
        "typedef struct {\n"
        "    void *ctx;\n");

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        char method_name[256];

        snake_name(
            api->methods[m].name,
            method_name,
            sizeof(method_name));

        fprintf(
            output,
            "    %s_%s_fn %s;\n",
            api_name,
            method_name,
            method_name);
    }

    fprintf(
        output,
        "} %s_local_t;\n\n"
        "aether_status_t %s_dispatch(\n"
        "    %s_local_t *api,\n"
        "    const uint8_t *data,\n"
        "    size_t length,\n"
        "    size_t *consumed);\n\n"
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n",
        api_name,
        api_name,
        api_name);

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated local API header");

        fclose(output);
        return false;
    }

    fclose(output);

    return true;
}


static bool emit_local_api_source(
    const adsl_api_t *api,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(
            path,
            "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated local API source");

        return false;
    }

    char base_snake[256];
    char api_name[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    snake_name(
        api->name,
        api_name,
        sizeof(api_name));

    fprintf(
        output,
        "#include \"%s_local.h\"\n\n"
        "#include <stdint.h>\n\n"
        "static bool %s_local_read_pack(\n"
        "    const uint8_t *data,\n"
        "    size_t length,\n"
        "    size_t *position,\n"
        "    uint64_t *value) {\n\n"
        "    const uint64_t u8 = UINT64_C(251);\n"
        "    const uint64_t u16 = UINT64_C(1515);\n"
        "    const uint64_t u32 = UINT64_C(1049835);\n"
        "    const uint64_t u64 =\n"
        "        u32 +\n"
        "        (UINT64_C(4294967296) * UINT64_C(256));\n\n"
        "    if (data == NULL ||\n"
        "        position == NULL ||\n"
        "        value == NULL ||\n"
        "        *position >= length) {\n"
        "        return false;\n"
        "    }\n\n"
        "    uint64_t current =\n"
        "        data[(*position)++];\n\n"
        "    if (current < u8) {\n"
        "        *value = current;\n"
        "        return true;\n"
        "    }\n\n"
        "    if (*position >= length) {\n"
        "        return false;\n"
        "    }\n\n"
        "    uint64_t next =\n"
        "        data[(*position)++];\n\n"
        "    current =\n"
        "        ((current - u8) << 8u) +\n"
        "        u8 +\n"
        "        next;\n\n"
        "    if (current < u16) {\n"
        "        *value = current;\n"
        "        return true;\n"
        "    }\n\n"
        "    if (length - *position < 2u) {\n"
        "        return false;\n"
        "    }\n\n"
        "    uint64_t fraction16 =\n"
        "        (uint64_t)data[*position] |\n"
        "        ((uint64_t)data[*position + 1u] << 8u);\n\n"
        "    *position += 2u;\n\n"
        "    current =\n"
        "        ((current - u16) << 16u) +\n"
        "        u16 +\n"
        "        fraction16;\n\n"
        "    if (current < u32) {\n"
        "        *value = current;\n"
        "        return true;\n"
        "    }\n\n"
        "    if (length - *position < 4u) {\n"
        "        return false;\n"
        "    }\n\n"
        "    uint64_t fraction32 =\n"
        "        (uint64_t)data[*position] |\n"
        "        ((uint64_t)data[*position + 1u] << 8u) |\n"
        "        ((uint64_t)data[*position + 2u] << 16u) |\n"
        "        ((uint64_t)data[*position + 3u] << 24u);\n\n"
        "    *position += 4u;\n\n"
        "    current =\n"
        "        ((current - u32) << 32u) +\n"
        "        u32 +\n"
        "        fraction32;\n\n"
        "    if (current >= u64) {\n"
        "        return false;\n"
        "    }\n\n"
        "    *value = current;\n"
        "    return true;\n"
        "}\n\n"
        "aether_status_t %s_dispatch(\n"
        "    %s_local_t *api,\n"
        "    const uint8_t *data,\n"
        "    size_t length,\n"
        "    size_t *consumed) {\n\n"
        "    if (api == NULL ||\n"
        "        consumed == NULL ||\n"
        "        (length > 0u && data == NULL)) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    *consumed = 0u;\n\n"
        "    if (length == 0u) {\n"
        "        return AETHER_ERR_PROTOCOL;\n"
        "    }\n\n"
        "    size_t position = 0u;\n"
        "    uint8_t command =\n"
        "        data[position++];\n\n"
        "    switch (command) {\n",
        base_snake,
        base_snake,
        api_name,
        api_name);

    for (size_t m = 0u;
         m < api->method_count;
         ++m) {

        const adsl_method_t *method =
            &api->methods[m];

        const adsl_param_t *param =
            &method->params[0];

        char method_name[256];
        char stream_name[256];

        snake_name(
            method->name,
            method_name,
            sizeof(method_name));

        declared_c_name(
            param->type,
            stream_name,
            sizeof(stream_name));

        fprintf(
            output,
            "    case %uu: {\n"
            "        if (api->%s == NULL) {\n"
            "            return AETHER_ERR_ARGUMENT;\n"
            "        }\n\n"
            "        uint64_t packed_length = 0u;\n\n"
            "        if (!%s_local_read_pack(\n"
            "                data,\n"
            "                length,\n"
            "                &position,\n"
            "                &packed_length) ||\n"
            "            packed_length > (uint64_t)SIZE_MAX) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n"
            "        size_t stream_length =\n"
            "            (size_t)packed_length;\n\n"
            "        if (stream_length >\n"
            "                length - position) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n"
            "        %s_t stream_value = {\n"
            "            .data =\n"
            "                (uint8_t *)(data + position),\n"
            "            .capacity = stream_length,\n"
            "            .length = stream_length\n"
            "        };\n\n"
            "        position +=\n"
            "            stream_length;\n\n"
            "        aether_status_t status =\n"
            "            api->%s(\n"
            "                api->ctx,\n"
            "                &stream_value);\n\n"
            "        if (status != AETHER_OK) {\n"
            "            return status;\n"
            "        }\n\n"
            "        *consumed = position;\n"
            "        return AETHER_OK;\n"
            "    }\n\n",
            (unsigned int)method->id,
            method_name,
            base_snake,
            stream_name,
            method_name);
    }

    fprintf(
        output,
        "    default:\n"
        "        return AETHER_ERR_PROTOCOL;\n"
        "    }\n"
        "}\n");

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated local API source");

        fclose(output);
        return false;
    }

    fclose(output);

    return true;
}


bool adsl_emit_c_local_api(
    const adsl_document_t *document,
    const char *output_directory,
    const char *base_name,
    const char *selected_api_name,
    char *error,
    size_t error_capacity) {

    if (document == NULL ||
        output_directory == NULL ||
        base_name == NULL ||
        selected_api_name == NULL ||
        *selected_api_name == '\0') {

        set_error(
            error,
            error_capacity,
            "invalid local emitter arguments");

        return false;
    }

    const adsl_api_t *api =
        find_api_definition(
            document,
            selected_api_name);

    if (!local_api_supported(
            api,
            error,
            error_capacity)) {

        return false;
    }

    char base_snake[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    char types_h[1024];
    char types_c[1024];
    char local_h[1024];
    char local_c[1024];

    (void)snprintf(
        types_h,
        sizeof(types_h),
        "%s/%s_types.h",
        output_directory,
        base_snake);

    (void)snprintf(
        types_c,
        sizeof(types_c),
        "%s/%s_types.c",
        output_directory,
        base_snake);

    (void)snprintf(
        local_h,
        sizeof(local_h),
        "%s/%s_local.h",
        output_directory,
        base_snake);

    (void)snprintf(
        local_c,
        sizeof(local_c),
        "%s/%s_local.c",
        output_directory,
        base_snake);

    if (!emit_types_header(
            document,
            types_h,
            base_name,
            error,
            error_capacity)) {

        return false;
    }


    if (!emit_types_source(
            document,
            types_c,
            base_name,
            error,
            error_capacity)) {

        return false;
    }


    if (!emit_local_api_header(
            document,
            api,
            local_h,
            base_name,
            error,
            error_capacity)) {

        return false;
    }

    return
        emit_local_api_source(
            api,
            local_c,
            base_name,
            error,
            error_capacity);
}



static const adsl_api_t *response_find_api(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u;
         i < document->api_count;
         ++i) {

        if (document->apis[i].name != NULL &&
            strcmp(
                document->apis[i].name,
                name) == 0) {

            return
                &document->apis[i];
        }
    }

    return NULL;
}


static const adsl_method_t *response_find_method(
    const adsl_api_t *api,
    const char *name) {

    if (api == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u;
         i < api->method_count;
         ++i) {

        if (api->methods[i].name != NULL &&
            strcmp(
                api->methods[i].name,
                name) == 0) {

            return
                &api->methods[i];
        }
    }

    return NULL;
}


static const adsl_type_t *response_find_type(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u;
         i < document->type_count;
         ++i) {

        if (document->types[i].name != NULL &&
            strcmp(
                document->types[i].name,
                name) == 0) {

            return
                &document->types[i];
        }
    }

    return NULL;
}


static bool response_dynamic_array_base(
    const char *type,
    char *base,
    size_t base_capacity) {

    if (type == NULL ||
        base == NULL ||
        base_capacity == 0u) {

        return false;
    }

    size_t length =
        strlen(type);

    if (length < 3u ||
        type[length - 2u] != '[' ||
        type[length - 1u] != ']') {

        return false;
    }

    size_t base_length =
        length - 2u;

    if (base_length == 0u ||
        base_length >=
            base_capacity) {

        return false;
    }

    memcpy(
        base,
        type,
        base_length);

    base[base_length] =
        '\0';

    return true;
}


static bool response_static_array_type(
    const char *type) {

    if (type == NULL) {
        return false;
    }

    const char *open =
        strchr(
            type,
            '[');

    if (open == NULL) {
        return false;
    }

    return
        strcmp(
            open,
            "[]") != 0;
}


static bool response_type_is(
    const char *type,
    const char *a,
    const char *b) {

    return
        type != NULL &&
        ((a != NULL &&
          strcmp(type, a) == 0) ||
         (b != NULL &&
          strcmp(type, b) == 0));
}


static const char *response_scalar_c_type(
    const char *type) {

    if (response_type_is(
            type,
            "byte",
            NULL)) {

        return "uint8_t";
    }

    if (response_type_is(
            type,
            "short",
            NULL)) {

        return "int16_t";
    }

    if (response_type_is(
            type,
            "int",
            NULL)) {

        return "int32_t";
    }

    if (response_type_is(
            type,
            "long",
            "Date")) {

        return "int64_t";
    }

    if (response_type_is(
            type,
            "boolean",
            "bool")) {

        return "bool";
    }

    if (response_type_is(
            type,
            "uuid",
            "UUID")) {

        return "aether_uuid_t";
    }

    return NULL;
}


static bool response_is_string(
    const char *type) {

    return
        response_type_is(
            type,
            "String",
            "string");
}


static bool response_is_byte_array(
    const char *type) {

    return
        type != NULL &&
        strcmp(
            type,
            "byte[]") == 0;
}


static bool response_path_append(
    const char *prefix,
    const char *name,
    char *result,
    size_t capacity) {

    if (name == NULL ||
        result == NULL ||
        capacity == 0u) {

        return false;
    }

    char name_snake[256];

    snake_name(
        name,
        name_snake,
        sizeof(name_snake));

    int written;

    if (prefix == NULL ||
        *prefix == '\0') {

        written =
            snprintf(
                result,
                capacity,
                "%s",
                name_snake);

    } else {
        written =
            snprintf(
                result,
                capacity,
                "%s_%s",
                prefix,
                name_snake);
    }

    return
        written >= 0 &&
        (size_t)written <
            capacity;
}


static bool response_supported_value(
    const adsl_document_t *document,
    const char *type,
    char *error,
    size_t error_capacity) {

    if (type == NULL) {
        set_error(
            error,
            error_capacity,
            "response field has no type");

        return false;
    }

    if (response_static_array_type(type)) {
        set_error(
            error,
            error_capacity,
            "response decoder does not yet support static arrays");

        return false;
    }

    if (response_is_string(type) ||
        response_is_byte_array(type) ||
        response_scalar_c_type(type) != NULL) {

        return true;
    }

    char array_base[256];

    if (response_dynamic_array_base(
            type,
            array_base,
            sizeof(array_base))) {

        if (response_type_is(
                array_base,
                "byte",
                NULL) ||
            response_type_is(
                array_base,
                "short",
                NULL) ||
            response_type_is(
                array_base,
                "int",
                NULL) ||
            response_type_is(
                array_base,
                "long",
                "Date")) {

            return true;
        }

        set_error(
            error,
            error_capacity,
            "response decoder currently supports byte/short/int/long dynamic arrays only");

        return false;
    }

    if (response_type_is(
            type,
            "float",
            NULL) ||
        response_type_is(
            type,
            "double",
            NULL)) {

        set_error(
            error,
            error_capacity,
            "response decoder does not yet support floating values");

        return false;
    }

    const adsl_type_t *declared =
        response_find_type(
            document,
            type);

    if (declared == NULL) {
        set_error(
            error,
            error_capacity,
            "response decoder cannot resolve result type");

        return false;
    }

    if (declared->enum_count > 0u) {
        set_error(
            error,
            error_capacity,
            "response decoder does not yet support enum result values");

        return false;
    }

    if (declared->is_abstract) {
        set_error(
            error,
            error_capacity,
            "response decoder does not yet support polymorphic result values");

        return false;
    }

    if (declared->parent != NULL) {
        set_error(
            error,
            error_capacity,
            "response decoder does not yet support inherited result structs");

        return false;
    }

    for (size_t i = 0u;
         i < declared->field_count;
         ++i) {

        if (!response_supported_value(
                document,
                declared->fields[i].type,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}


static bool response_emit_field_c_type(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    char *error,
    size_t error_capacity) {

    const char *scalar =
        response_scalar_c_type(
            type);

    if (scalar != NULL) {
        fprintf(
            output,
            "%s",
            scalar);

        return true;
    }

    if (response_is_string(type) ||
        response_is_byte_array(type)) {

        fprintf(
            output,
            "aether_bytes_view_t");

        return true;
    }

    char array_base[256];

    if (response_dynamic_array_base(
            type,
            array_base,
            sizeof(array_base))) {

        const char *array_scalar =
            response_scalar_c_type(
                array_base);

        if (array_scalar == NULL ||
            response_type_is(
                array_base,
                "boolean",
                "bool") ||
            response_type_is(
                array_base,
                "uuid",
                "UUID")) {

            set_error(
                error,
                error_capacity,
                "response public ABI does not yet support this dynamic array element type");

            return false;
        }

        fprintf(
            output,
            "struct { %s *data; size_t length; }",
            array_scalar);

        return true;
    }

    const adsl_type_t *declared =
        response_find_type(
            document,
            type);

    if (declared == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot emit response public field type");

        return false;
    }

    if (declared->enum_count > 0u) {
        set_error(
            error,
            error_capacity,
            "response public ABI does not yet support enum result fields");

        return false;
    }

    if (declared->is_abstract ||
        declared->parent != NULL) {

        set_error(
            error,
            error_capacity,
            "response public ABI does not yet support polymorphic or inherited result fields");

        return false;
    }

    fprintf(
        output,
        "struct {\n");

    for (size_t i = 0u;
         i < declared->field_count;
         ++i) {

        fprintf(
            output,
            "        ");

        if (!response_emit_field_c_type(
                output,
                document,
                declared->fields[i].type,
                error,
                error_capacity)) {

            return false;
        }

        fprintf(
            output,
            " %s;\n",
            declared->fields[i].name);
    }

    fprintf(
        output,
        "    }");

    return true;
}


static bool response_emit_storage_fields(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *path,
    char *error,
    size_t error_capacity) {

    char array_base[256];

    if (response_dynamic_array_base(
            type,
            array_base,
            sizeof(array_base))) {

        if (response_is_byte_array(type)) {
            return true;
        }

        const char *scalar =
            response_scalar_c_type(
                array_base);

        if (scalar == NULL) {
            set_error(
                error,
                error_capacity,
                "response storage requires scalar dynamic array");

            return false;
        }

        fprintf(
            output,
            "    %s *%s_storage;\n"
            "    size_t %s_capacity;\n",
            scalar,
            path,
            path);

        return true;
    }

    if (response_scalar_c_type(type) != NULL ||
        response_is_string(type)) {

        return true;
    }

    const adsl_type_t *declared =
        response_find_type(
            document,
            type);

    if (declared == NULL ||
        declared->enum_count > 0u) {

        return true;
    }

    for (size_t i = 0u;
         i < declared->field_count;
         ++i) {

        char child_path[512];

        if (!response_path_append(
                path,
                declared->fields[i].name,
                child_path,
                sizeof(child_path))) {

            set_error(
                error,
                error_capacity,
                "response storage path too long");

            return false;
        }

        if (!response_emit_storage_fields(
                output,
                document,
                declared->fields[i].type,
                child_path,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}


static bool response_emit_decode_value(
    FILE *output,
    const adsl_document_t *document,
    const char *base_snake,
    const char *type,
    const char *target,
    const char *path,
    char *error,
    size_t error_capacity) {

    if (response_type_is(
            type,
            "uuid",
            "UUID")) {

        fprintf(
            output,
            "        if (!%s_response_read_uuid(\n"
            "                &reader,\n"
            "                &%s)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_type_is(
            type,
            "byte",
            NULL)) {

        fprintf(
            output,
            "        if (!%s_response_read_u8(\n"
            "                &reader,\n"
            "                &%s)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_type_is(
            type,
            "boolean",
            "bool")) {

        fprintf(
            output,
            "        {\n"
            "            uint8_t value = 0u;\n"
            "            if (!%s_response_read_u8(\n"
            "                    &reader,\n"
            "                    &value) ||\n"
            "                value > 1u) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n"
            "            %s = value != 0u;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_type_is(
            type,
            "short",
            NULL)) {

        fprintf(
            output,
            "        if (!%s_response_read_i16le(\n"
            "                &reader,\n"
            "                &%s)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_type_is(
            type,
            "int",
            NULL)) {

        fprintf(
            output,
            "        if (!%s_response_read_i32le(\n"
            "                &reader,\n"
            "                &%s)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_type_is(
            type,
            "long",
            "Date")) {

        fprintf(
            output,
            "        if (!%s_response_read_i64le(\n"
            "                &reader,\n"
            "                &%s)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target);

        return true;
    }

    if (response_is_string(type) ||
        response_is_byte_array(type)) {

        fprintf(
            output,
            "        if (!%s_response_read_view(\n"
            "                &reader,\n"
            "                &%s.data,\n"
            "                &%s.length)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n\n",
            base_snake,
            target,
            target);

        return true;
    }

    char array_base[256];

    if (response_dynamic_array_base(
            type,
            array_base,
            sizeof(array_base))) {

        const char *scalar =
            response_scalar_c_type(
                array_base);

        if (scalar == NULL) {
            set_error(
                error,
                error_capacity,
                "cannot emit response dynamic array decoder");

            return false;
        }

        fprintf(
            output,
            "        {\n"
            "            uint64_t count64 = 0u;\n"
            "            if (!%s_response_read_pack(\n"
            "                    &reader,\n"
            "                    &count64) ||\n"
            "                count64 > (uint64_t)SIZE_MAX) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n\n"
            "            size_t count =\n"
            "                (size_t)count64;\n\n"
            "            if (count > api->%s_capacity ||\n"
            "                (count != 0u &&\n"
            "                 api->%s_storage == NULL)) {\n"
            "                return AETHER_ERR_OVERFLOW;\n"
            "            }\n\n"
            "            %s.data =\n"
            "                api->%s_storage;\n"
            "            %s.length =\n"
            "                count;\n\n"
            "            for (size_t i = 0u;\n"
            "                 i < count;\n"
            "                 ++i) {\n",
            base_snake,
            path,
            path,
            target,
            path,
            target);

        if (response_type_is(
                array_base,
                "short",
                NULL)) {

            fprintf(
                output,
                "                if (!%s_response_read_i16le(\n"
                "                        &reader,\n"
                "                        &api->%s_storage[i])) {\n"
                "                    return AETHER_ERR_PROTOCOL;\n"
                "                }\n",
                base_snake,
                path);

        } else if (response_type_is(
                       array_base,
                       "int",
                       NULL)) {

            fprintf(
                output,
                "                if (!%s_response_read_i32le(\n"
                "                        &reader,\n"
                "                        &api->%s_storage[i])) {\n"
                "                    return AETHER_ERR_PROTOCOL;\n"
                "                }\n",
                base_snake,
                path);

        } else if (response_type_is(
                       array_base,
                       "long",
                       "Date")) {

            fprintf(
                output,
                "                if (!%s_response_read_i64le(\n"
                "                        &reader,\n"
                "                        &api->%s_storage[i])) {\n"
                "                    return AETHER_ERR_PROTOCOL;\n"
                "                }\n",
                base_snake,
                path);

        } else if (response_type_is(
                       array_base,
                       "byte",
                       NULL)) {

            fprintf(
                output,
                "                if (!%s_response_read_u8(\n"
                "                        &reader,\n"
                "                        &api->%s_storage[i])) {\n"
                "                    return AETHER_ERR_PROTOCOL;\n"
                "                }\n",
                base_snake,
                path);

        } else {
            set_error(
                error,
                error_capacity,
                "unsupported scalar dynamic array decoder");

            return false;
        }

        fprintf(
            output,
            "            }\n"
            "        }\n\n");

        return true;
    }

    const adsl_type_t *declared =
        response_find_type(
            document,
            type);

    if (declared == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot emit response value decoder");

        return false;
    }

    if (declared->enum_count > 0u) {
        char enum_name[256];

        declared_c_name(
            declared->name,
            enum_name,
            sizeof(enum_name));

        fprintf(
            output,
            "        {\n"
            "            uint8_t value = 0u;\n"
            "            if (!%s_response_read_u8(\n"
            "                    &reader,\n"
            "                    &value) ||\n"
            "                value >= %zuu) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n"
            "            %s = (%s_t)value;\n"
            "        }\n\n",
            base_snake,
            declared->enum_count,
            target,
            enum_name);

        return true;
    }

    for (size_t i = 0u;
         i < declared->field_count;
         ++i) {

        char child_target[1024];
        char child_path[512];

        int target_written =
            snprintf(
                child_target,
                sizeof(child_target),
                "%s.%s",
                target,
                declared->fields[i].name);

        if (target_written < 0 ||
            (size_t)target_written >=
                sizeof(child_target) ||
            !response_path_append(
                path,
                declared->fields[i].name,
                child_path,
                sizeof(child_path))) {

            set_error(
                error,
                error_capacity,
                "response decoder expression too long");

            return false;
        }

        if (!response_emit_decode_value(
                output,
                document,
                base_snake,
                declared->fields[i].type,
                child_target,
                child_path,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}


static bool response_method_supported(
    const adsl_document_t *document,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    if (method == NULL) {
        set_error(
            error,
            error_capacity,
            "selected response method not found");

        return false;
    }

    if (method->throws_type != NULL) {
        set_error(
            error,
            error_capacity,
            "response decoder does not yet support typed throws");

        return false;
    }

    if (method->returns_type == NULL &&
        method->return_field_count == 0u) {

        set_error(
            error,
            error_capacity,
            "selected response method has no result");

        return false;
    }

    if (method->returns_type != NULL &&
        method->return_field_count > 0u) {

        set_error(
            error,
            error_capacity,
            "response method cannot mix named and anonymous result");

        return false;
    }

    if (method->returns_type != NULL) {
        return
            response_supported_value(
                document,
                method->returns_type,
                error,
                error_capacity);
    }

    for (size_t i = 0u;
         i < method->return_field_count;
         ++i) {

        if (!response_supported_value(
                document,
                method->return_fields[i].type,
                error,
                error_capacity)) {

            return false;
        }
    }

    return true;
}


/* direct response helper forward declarations */
static bool response_method_has_storage(
    const adsl_document_t *document,
    const adsl_method_t *method);

static bool response_emit_direct_result_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity);

static bool response_emit_direct_storage_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity);

static void response_emit_direct_frame_declaration(
    FILE *output,
    const char *api_name,
    const char *method_name,
    const char *api_upper,
    const char *method_upper,
    bool has_storage);

static void response_emit_direct_reader_integer_helpers(
    FILE *output,
    const char *base);

static void response_emit_direct_reader_signed_helpers(
    FILE *output,
    const char *base);

static void response_emit_direct_reader_pack_helper(
    FILE *output,
    const char *base);

static void response_emit_direct_reader_view_uuid_helpers(
    FILE *output,
    const char *base);


static bool emit_response_method_header(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const adsl_method_t *method,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(
            path,
            "wb");

    if (output == NULL) {
        set_error(
            error,
            error_capacity,
            "cannot create generated response header");

        return false;
    }

    char guard[256];
    char api_name[256];
    char method_name[256];
    char api_upper[256];
    char method_upper[256];

    upper_name(
        base_name,
        guard,
        sizeof(guard));

    snake_name(
        api->name,
        api_name,
        sizeof(api_name));

    snake_name(
        method->name,
        method_name,
        sizeof(method_name));

    upper_name(
        api_name,
        api_upper,
        sizeof(api_upper));

    upper_name(
        method_name,
        method_upper,
        sizeof(method_upper));

    fprintf(
        output,
        "#ifndef %s_RESPONSE_H\n"
        "#define %s_RESPONSE_H\n\n"
        "#include \"aether_client.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n",
        guard,
        guard);

    bool ok =
        response_emit_direct_result_type(
            output,
            document,
            method,
            api_name,
            method_name,
            error,
            error_capacity);

    bool has_storage =
        response_method_has_storage(
            document,
            method);

    if (ok &&
        has_storage) {

        ok =
            response_emit_direct_storage_type(
                output,
                document,
                method,
                api_name,
                method_name,
                error,
                error_capacity);
    }

    if (ok) {
        response_emit_direct_frame_declaration(
            output,
            api_name,
            method_name,
            api_upper,
            method_upper,
            has_storage);
    }

    fprintf(
        output,
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n");

    if (ferror(output)) {
        ok =
            false;

        set_error(
            error,
            error_capacity,
            "cannot write generated response header");
    }

    fclose(output);

    return ok;
}


static bool emit_response_method_source(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const adsl_method_t *method,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output =
        fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated response source");
        return false;
    }

    char base_snake[256];
    char api_name[256];
    char method_name[256];
    char api_upper[256];
    char method_upper[256];

    snake_name(base_name, base_snake, sizeof(base_snake));
    snake_name(api->name, api_name, sizeof(api_name));
    snake_name(method->name, method_name, sizeof(method_name));
    upper_name(api_name, api_upper, sizeof(api_upper));
    upper_name(method_name, method_upper, sizeof(method_upper));

    bool has_storage =
        response_method_has_storage(document, method);

    fprintf(
        output,

        "#include \"%s_response.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <stdbool.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n",

        base_snake);

    response_emit_direct_reader_integer_helpers(output, base_snake);
    response_emit_direct_reader_signed_helpers(output, base_snake);
    response_emit_direct_reader_pack_helper(output, base_snake);
    response_emit_direct_reader_view_uuid_helpers(output, base_snake);

    fprintf(
        output,
        "aether_status_t %s_%s_response_decode(\n",
        api_name,
        method_name);

    if (has_storage) {
        fprintf(
            output,
            "    const %s_%s_response_storage_t *api,\n",
            api_name,
            method_name);
    }

    fprintf(
        output,
        "    const uint8_t *data,\n"
        "    size_t length,\n"
        "    %s_%s_response_frame_t *frame,\n"
        "    size_t *consumed) {\n\n"
        "    if (frame == NULL || consumed == NULL || (length != 0u && data == NULL)%s) return AETHER_ERR_ARGUMENT;\n"
        "    *frame = (%s_%s_response_frame_t){0};\n"
        "    *consumed = 0u;\n"
        "    %s_response_reader_t reader = { data, length, 0u };\n"
        "    uint8_t command;\n"
        "    uint32_t request_id;\n"
        "    if (!%s_response_read_u8(&reader, &command) || !%s_response_read_u32le(&reader, &request_id)) return AETHER_ERR_PROTOCOL;\n"
        "    frame->request_id = request_id;\n"
        "    if (command == 1u) { frame->kind = %s_%s_RESPONSE_ERROR; *consumed = reader.position; return AETHER_OK; }\n"
        "    if (command != 0u) return AETHER_ERR_PROTOCOL;\n"
        "    %s_%s_result_t result = {0};\n\n",
        api_name,
        method_name,
        has_storage ? " || api == NULL" : "",
        api_name,
        method_name,
        base_snake,
        base_snake,
        base_snake,
        api_upper,
        method_upper,
        api_name,
        method_name);

    bool ok = true;

    if (method->returns_type != NULL) {
        ok =
            response_emit_decode_value(
                output,
                document,
                base_snake,
                method->returns_type,
                "result.value",
                "value",
                error,
                error_capacity);
    } else {
        for (size_t i = 0u; ok && i < method->return_field_count; ++i) {
            char target[1024];
            char field_path[512];

            int written =
                snprintf(
                    target,
                    sizeof(target),
                    "result.%s",
                    method->return_fields[i].name);

            ok =
                written >= 0 &&
                (size_t)written < sizeof(target) &&
                response_path_append(
                    NULL,
                    method->return_fields[i].name,
                    field_path,
                    sizeof(field_path)) &&
                response_emit_decode_value(
                    output,
                    document,
                    base_snake,
                    method->return_fields[i].type,
                    target,
                    field_path,
                    error,
                    error_capacity);
        }
    }

    if (ok) {
        fprintf(
            output,
            "    frame->kind = %s_%s_RESPONSE_RESULT;\n"
            "    frame->result = result;\n"
            "    *consumed = reader.position;\n"
            "    return AETHER_OK;\n"
            "}\n",
            api_upper,
            method_upper);
    }

    if (ferror(output)) {
        ok = false;
        set_error(error, error_capacity, "cannot write generated response source");
    }

    fclose(output);
    return ok;
}


bool adsl_emit_c_response_method(
    const adsl_document_t *document,
    const char *output_directory,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    if (document == NULL ||
        output_directory == NULL ||
        base_name == NULL ||
        selected_api_name == NULL ||
        selected_method_name == NULL) {

        set_error(
            error,
            error_capacity,
            "invalid response emitter arguments");

        return false;
    }

    const adsl_api_t *api =
        response_find_api(
            document,
            selected_api_name);

    const adsl_method_t *method =
        response_find_method(
            api,
            selected_method_name);

    if (!response_method_supported(
            document,
            method,
            error,
            error_capacity)) {

        return false;
    }

    char base_snake[256];

    snake_name(
        base_name,
        base_snake,
        sizeof(base_snake));

    char response_h[1024];
    char response_c[1024];

    (void)snprintf(
        response_h,
        sizeof(response_h),
        "%s/%s_response.h",
        output_directory,
        base_snake);

    (void)snprintf(
        response_c,
        sizeof(response_c),
        "%s/%s_response.c",
        output_directory,
        base_snake);

    if (!emit_response_method_header(
            document,
            api,
            method,
            response_h,
            base_name,
            error,
            error_capacity)) {

        return false;
    }

    return
        emit_response_method_source(
            document,
            api,
            method,
            response_c,
            base_name,
            error,
            error_capacity);
}



static bool response_value_has_storage(
    const adsl_document_t *document,
    const char *type) {

    char array_base[256];

    if (response_dynamic_array_base(
            type,
            array_base,
            sizeof(array_base))) {

        return
            !response_is_byte_array(type);
    }

    if (response_scalar_c_type(type) != NULL ||
        response_is_string(type)) {

        return false;
    }

    const adsl_type_t *declared =
        response_find_type(
            document,
            type);

    if (declared == NULL ||
        declared->enum_count > 0u) {

        return false;
    }

    for (size_t i = 0u;
         i < declared->field_count;
         ++i) {

        if (response_value_has_storage(
                document,
                declared->fields[i].type)) {

            return true;
        }
    }

    return false;
}


static bool response_method_has_storage(
    const adsl_document_t *document,
    const adsl_method_t *method) {

    if (method->returns_type != NULL) {
        return
            response_value_has_storage(
                document,
                method->returns_type);
    }

    for (size_t i = 0u;
         i < method->return_field_count;
         ++i) {

        if (response_value_has_storage(
                document,
                method->return_fields[i].type)) {

            return true;
        }
    }

    return false;
}


static bool response_emit_direct_result_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    fprintf(
        output,
        "typedef struct {\n");

    if (method->returns_type != NULL) {
        fprintf(
            output,
            "    ");

        if (!response_emit_field_c_type(
                output,
                document,
                method->returns_type,
                error,
                error_capacity)) {

            return false;
        }

        fprintf(
            output,
            " value;\n");
    } else {
        for (size_t i = 0u;
             i < method->return_field_count;
             ++i) {

            fprintf(
                output,
                "    ");

            if (!response_emit_field_c_type(
                    output,
                    document,
                    method->return_fields[i].type,
                    error,
                    error_capacity)) {

                return false;
            }

            fprintf(
                output,
                " %s;\n",
                method->return_fields[i].name);
        }
    }

    fprintf(
        output,
        "} %s_%s_result_t;\n\n",
        api_name,
        method_name);

    return true;
}


static bool response_emit_direct_storage_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    fprintf(
        output,
        "typedef struct {\n");

    if (method->returns_type != NULL) {
        if (!response_emit_storage_fields(
                output,
                document,
                method->returns_type,
                "value",
                error,
                error_capacity)) {

            return false;
        }
    } else {
        for (size_t i = 0u;
             i < method->return_field_count;
             ++i) {

            char path[512];

            if (!response_path_append(
                    NULL,
                    method->return_fields[i].name,
                    path,
                    sizeof(path)) ||
                !response_emit_storage_fields(
                    output,
                    document,
                    method->return_fields[i].type,
                    path,
                    error,
                    error_capacity)) {

                return false;
            }
        }
    }

    fprintf(
        output,
        "} %s_%s_response_storage_t;\n\n",
        api_name,
        method_name);

    return true;
}


static void response_emit_direct_frame_declaration(
    FILE *output,
    const char *api_name,
    const char *method_name,
    const char *api_upper,
    const char *method_upper,
    bool has_storage) {

    fprintf(
        output,
        "typedef enum {\n"
        "    %s_%s_RESPONSE_RESULT = 0,\n"
        "    %s_%s_RESPONSE_ERROR = 1\n"
        "} %s_%s_response_kind_t;\n\n"
        "typedef struct {\n"
        "    %s_%s_response_kind_t kind;\n"
        "    uint32_t request_id;\n"
        "    %s_%s_result_t result;\n"
        "} %s_%s_response_frame_t;\n\n"
        "aether_status_t %s_%s_response_decode(\n",
        api_upper,
        method_upper,
        api_upper,
        method_upper,
        api_name,
        method_name,
        api_name,
        method_name,
        api_name,
        method_name,
        api_name,
        method_name,
        api_name,
        method_name);

    if (has_storage) {
        fprintf(
            output,
            "    const %s_%s_response_storage_t *api,\n",
            api_name,
            method_name);
    }

    fprintf(
        output,
        "    const uint8_t *data,\n"
        "    size_t length,\n"
        "    %s_%s_response_frame_t *frame,\n"
        "    size_t *consumed);\n\n",
        api_name,
        method_name);
}


static void response_emit_direct_reader_integer_helpers(
    FILE *output,
    const char *base) {

    fprintf(
        output,
        "typedef aether_meta_reader_t %s_response_reader_t;\n"
        "#define %s_response_read_u8 aether_meta_read_u8\n"
        "#define %s_response_read_u16le aether_meta_read_u16le\n"
        "#define %s_response_read_u32le aether_meta_read_u32le\n"
        "#define %s_response_read_u64le aether_meta_read_u64le\n\n",
        base,
        base,
        base,
        base,
        base);
}


static void response_emit_direct_reader_signed_helpers(
    FILE *output,
    const char *base) {

    fprintf(
        output,
        "#define %s_response_read_i16le aether_meta_read_i16le\n"
        "#define %s_response_read_i32le aether_meta_read_i32le\n"
        "#define %s_response_read_i64le aether_meta_read_i64le\n\n",
        base,
        base,
        base);
}


static void response_emit_direct_reader_pack_helper(
    FILE *output,
    const char *base) {

    fprintf(
        output,
        "#define %s_response_read_pack aether_meta_read_pack\n\n",
        base);
}


static void response_emit_direct_reader_view_uuid_helpers(
    FILE *output,
    const char *base) {

    fprintf(
        output,
        "#define %s_response_read_view aether_meta_read_view\n"
        "#define %s_response_read_uuid aether_meta_read_uuid\n\n",
        base,
        base);
}