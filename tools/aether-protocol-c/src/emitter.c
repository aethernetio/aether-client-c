

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


/*
 * Generated C identifiers must not collide with C keywords. ADSL method
 * names are protocol names and must not be renamed, so the C target only
 * adjusts the emitted identifier (for example `register` becomes
 * `register_`). The wire name and command id are unaffected.
 *
 * This is applied to identifiers that are derived from protocol names and
 * used as C members, functions or typedefs. Type/API prefixes are not
 * affected because they are already prefixed with the base name.
 */
static bool c_identifier(
    const char *source,
    char *target,
    size_t capacity) {

    static const char *const keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for", "goto",
        "if", "inline", "int", "long", "register", "restrict", "return",
        "short", "signed", "sizeof", "static", "struct", "switch",
        "typedef", "union", "unsigned", "void", "volatile", "while",
        "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex",
        "_Generic", "_Imaginary", "_Noreturn", "_Static_assert",
        "_Thread_local",
        NULL
    };

    snake_name(source, target, capacity);

    for (size_t i = 0u; keywords[i] != NULL; ++i) {

        if (strcmp(target, keywords[i]) == 0) {

            size_t length = strlen(target);

            if (length + 1u >= capacity) {
                return false;
            }

            target[length] = '_';
            target[length + 1u] = '\0';
            return true;
        }
    }

    return true;
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


static bool emit_decode_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_decode_struct_fields(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_decode_hierarchy_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *static_type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_decode_array_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity);



/*
 * Generated local parameter storage/pass classification.
 *
 * Local callbacks and the remote ABI must stay in step, so this helper is
 * the single place that decides how a decoded root parameter becomes a
 * callback argument:
 *
 *   scalar / enum / UUID   -> by value, stack local
 *   dynamic array view     -> by value, stack local
 *   stream / ref wrapper   -> stack local, address passed
 *   concrete struct        -> caller scratch, pointer passed (MCU stack)
 *   static array alias     -> caller scratch, pointer passed (MCU stack)
 *
 * `valid` is set to false for unknown/abstract types so the caller can
 * raise a generation error with its own context.
 */
typedef enum {
    C_PARAM_KIND_SCALAR,
    C_PARAM_KIND_DYNAMIC_VIEW,
    C_PARAM_KIND_STREAM_PTR,
    C_PARAM_KIND_REF_PTR,
    C_PARAM_KIND_ROOT_STRUCT,
    C_PARAM_KIND_ROOT_STATIC_ARRAY
} c_param_kind_t;

static c_param_kind_t classify_param_kind(
    const adsl_document_t *document,
    const adsl_param_t *param,
    bool *valid) {

    *valid = false;

    if (document == NULL || param == NULL || param->type == NULL) {
        return C_PARAM_KIND_SCALAR;
    }

    if (param->stream != NULL) {
        *valid = true;
        return C_PARAM_KIND_STREAM_PTR;
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

        (void)array_static_size;

        if (array_dynamic) {
            *valid = true;
            return C_PARAM_KIND_DYNAMIC_VIEW;
        }

        *valid = true;
        return C_PARAM_KIND_ROOT_STATIC_ARRAY;
    }

    char base[128];

    if (!scalar_type_name(param->type, base, sizeof(base))) {
        return C_PARAM_KIND_SCALAR;
    }

    if (primitive_c_type(base) != NULL) {
        *valid = true;
        return C_PARAM_KIND_SCALAR;
    }

    const adsl_type_t *declared =
        find_declared_type(document, base);

    if (declared == NULL) {
        return C_PARAM_KIND_SCALAR;
    }


    if (declared->stream != NULL) {
        /*
         * Declared stream parameters use the same pointer ABI as anonymous
         * inline streams: the decoder produces a small view struct and the
         * callback takes its address.
         */
        *valid = true;
        return C_PARAM_KIND_STREAM_PTR;
    }

    if (declared->enum_count > 0u) {
        *valid = true;
        return C_PARAM_KIND_SCALAR;
    }


    if (type_is_polymorphic_reference(document, declared)) {
        *valid = true;
        return C_PARAM_KIND_REF_PTR;
    }

    if (declared->is_abstract) {
        return C_PARAM_KIND_SCALAR;
    }

    *valid = true;
    return C_PARAM_KIND_ROOT_STRUCT;
}


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
        "#include \"aether_meta_runtime.h\"\n"
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
                "    const %s_ref_t *value);\n\n"
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_ref_t *value);\n\n",
                serializer_base,
                type_name,
                type_name,
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
                "    const %s_t *value);\n\n"
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_t *value);\n\n",
                serializer_base,
                type_name,
                type_name,
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

        if (hierarchy) {
            fprintf(
                output,
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_ref_t *value) {\n\n",
                base_snake,
                type_name,
                type_name);
        } else {
            fprintf(
                output,
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_t *value) {\n\n",
                base_snake,
                type_name,
                type_name);
        }

        fputs(
            "    if (ctx == NULL || value == NULL) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    memset(value, 0, sizeof(*value));\n\n",
            output);

        bool decoded;

        if (hierarchy) {
            decoded =
                emit_decode_hierarchy_expression(
                    output,
                    document,
                    type,
                    "(*value)",
                    base_snake,
                    0u,
                    error,
                    error_capacity);
        } else {
            decoded =
                emit_decode_struct_fields(
                    output,
                    document,
                    type,
                    "value",
                    base_snake,
                    0u,
                    error,
                    error_capacity);
        }

        if (!decoded) {
            fclose(output);
            return false;
        }

        fputs(
            "\n"
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


/*
 * Emitter-side wrapper around adsl_collect_effective_methods.
 *
 * All API iteration sites use this so a child API flattens its parents'
 * methods parent-first, exactly like the Java reference generator. C has
 * no interface inheritance at runtime; inheritance is resolved here at
 * generation time.
 *
 * On failure the helper writes a human-readable error and the caller must
 * abort generation. On success *out_methods is heap-allocated and must be
 * released with free(); a zero count returns *out_methods == NULL.
 */
static bool emitter_collect_effective(
    const adsl_document_t *document,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,
    size_t error_capacity) {

    return adsl_collect_effective_methods(
        document,
        api,
        out_methods,
        out_count,
        error,
        error_capacity);
}


/*
 * Lookup a single effective method by name. Unlike find_api_definition,
 * this traverses inherited methods so `--api ChildApi --method parentMethod`
 * resolves for the Java-compatible call semantics.
 */
static const adsl_effective_method_t *emitter_find_effective_method(
    const adsl_effective_method_t *methods,
    size_t count,
    const char *name) {

    if (methods == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u; i < count; ++i) {

        if (strcmp(methods[i].method->name, name) == 0) {

            return &methods[i];
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


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective(
            document,
            api,
            &effective,
            &effective_count,
            error,
            error_capacity)) {

        return false;
    }

    for (size_t m = 0u;
         m < effective_count;
         ++m) {

        const adsl_method_t *method =
            effective[m].method;


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

    free(effective);



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


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective(
            document,
            api,
            &effective,
            &effective_count,
            error,
            error_capacity)) {

        return false;
    }

    for (size_t m = 0u;
         m < effective_count;
         ++m) {

        const adsl_method_t *method =
            effective[m].method;


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

    free(effective);

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




        adsl_effective_method_t *effective = NULL;
        size_t effective_count = 0u;

        if (!emitter_collect_effective(
                document,
                api,
                &effective,
                &effective_count,
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }

        for (size_t m = 0u;
             m < effective_count;
             ++m) {

            const adsl_method_t *method =
                effective[m].method;


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

        free(effective);

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


        adsl_effective_method_t *effective = NULL;
        size_t effective_count = 0u;

        if (!emitter_collect_effective(
                document,
                api,
                &effective,
                &effective_count,
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }

        for (size_t m = 0u;
             m < effective_count;
             ++m) {

            const adsl_method_t *method =
                effective[m].method;


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



                "%s"
                "%s"
                "    if (api->tx_capacity < %uu) {\n"
                "        return AETHER_ERR_OVERFLOW;\n"
                "    }\n\n"
                "    size_t pos = 0u;\n"
                "    api->tx[pos++] = %uu;\n",

                method->param_count != 0u
                    ? "    uint8_t *tx = api->tx;\n"
                    : "",
                method->param_count != 0u
                    ? "    size_t tx_capacity = api->tx_capacity;\n\n"
                    : "",
                has_response
                    ? 5u
                    : 1u,
                (unsigned int)effective[m].id);



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

        free(effective);

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
            adsl_effective_method_t *effective = NULL;
            size_t effective_count = 0u;

            if (!emitter_collect_effective(
                    document,
                    selected_api,
                    &effective,
                    &effective_count,
                    error,
                    error_capacity)) {

                return false;
            }


            /*
             * The lookup result points into the effective array, which is
             * freed before the found/not-found check. Reduce it to a boolean
             * before free() so no pointer into freed storage is retained.
             */
            bool method_found =
                emitter_find_effective_method(
                    effective,
                    effective_count,
                    selected_method_name) != NULL;

            free(effective);

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


/*
 * Canonical C type mapping for a single ADSL field or result value.
 *
 * This is the single source of truth for how an ADSL type becomes a C
 * declaration. It is shared by:
 *
 *   - normal generated struct field declarations
 *   - anonymous response result struct fields
 *
 * Nullable values use pointer shape (`const T *`); non-nullable values use
 * inline shape (`T`). Arrays collapse to their generated alias typedef.
 * Polymorphic references use the generated `<x>_ref_t`.
 *
 * On success the caller receives the base C type name in base_out and a
 * flag telling whether the field should be declared as `const T *` or `T`.
 */
static bool format_c_type(
    const adsl_document_t *document,
    const char *type,
    char *base_out,
    size_t base_capacity,
    bool *out_pointer,
    char *error,
    size_t error_capacity) {

    if (type == NULL ||
        base_out == NULL ||
        base_capacity == 0u ||
        out_pointer == NULL) {

        set_error(error, error_capacity, "invalid C type request");
        return false;
    }

    if (strchr(type, '~') != NULL) {
        set_error(error, error_capacity,
            "collapsible fields are not generated for MCU targets");
        return false;
    }

    bool nullable =
        strchr(type, '?') != NULL;

    char cleaned[256];

    if (!strip_nullable_suffix(type, cleaned, sizeof(cleaned))) {
        set_error(error, error_capacity, "field type is too long");
        return false;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            cleaned,
            array_element,
            sizeof(array_element),
            &array_dynamic,
            &array_static_size)) {

        (void)array_element;
        (void)array_dynamic;
        (void)array_static_size;

        if (!array_alias_name(
                document,
                cleaned,
                base_out,
                base_capacity)) {
            set_error(error, error_capacity, "field array alias is too long");
            return false;
        }

        *out_pointer = nullable;
        return true;
    }

    char base[128];

    if (!scalar_type_name(cleaned, base, sizeof(base))) {
        set_error(error, error_capacity, "unsupported field shape");
        return false;
    }

    const char *primitive = primitive_c_type(base);

    if (primitive != NULL) {
        int written = snprintf(base_out, base_capacity, "%s", primitive);
        if (written < 0 || (size_t)written >= base_capacity) {
            set_error(error, error_capacity, "field type is too long");
            return false;
        }
        *out_pointer = nullable;
        return true;
    }

    const adsl_type_t *declared = find_declared_type(document, base);

    if (declared == NULL) {
        set_error(error, error_capacity, "unknown field type");
        return false;
    }

    char name[256];
    declared_c_name(declared->name, name, sizeof(name));

    if (declared->stream != NULL ||
        declared->enum_count > 0u) {
        int written = snprintf(base_out, base_capacity, "%s_t", name);
        if (written < 0 || (size_t)written >= base_capacity) {
            set_error(error, error_capacity, "field type is too long");
            return false;
        }
        *out_pointer = nullable;
        return true;
    }

    if (type_is_polymorphic_reference(document, declared)) {
        int written = snprintf(base_out, base_capacity, "%s_ref_t", name);
        if (written < 0 || (size_t)written >= base_capacity) {
            set_error(error, error_capacity, "field type is too long");
            return false;
        }
        *out_pointer = nullable;
        return true;
    }

    if (declared->is_abstract) {
        set_error(error, error_capacity,
            "abstract field has no concrete hierarchy descendants");
        return false;
    }

    int written = snprintf(base_out, base_capacity, "%s_t", name);
    if (written < 0 || (size_t)written >= base_capacity) {
        set_error(error, error_capacity, "field type is too long");
        return false;
    }

    *out_pointer = nullable;
    return true;
}




static bool emit_field_declaration(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity);



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

        if (!emit_field_declaration(
                output,
                document,
                field->type,
                field->name,
                error,
                error_capacity)) {
            return false;
        }
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




static bool method_is_response_bearing(
    const adsl_method_t *method) {

    return method != NULL &&
           (method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL);
}


static bool local_api_supported(
    const adsl_api_t *api,
    char *error,
    size_t error_capacity) {

    if (api == NULL) {
        set_error(error, error_capacity, "selected local API not found");
        return false;
    }


    if (api->method_count == 0u &&
        api->parent_count == 0u) {
        set_error(error, error_capacity, "selected local API has no methods");
        return false;
    }


    for (size_t m = 0u; m < api->method_count; ++m) {
        const adsl_method_t *method = &api->methods[m];


        /*
         * Command ids are contextual and computed by
         * adsl_collect_effective_methods(), so has_id is no longer a
         * reliable indicator here. Effective id validity is enforced by
         * the collector.
         */



        /*
         * Nullable and collapsible method parameters are not generated for
         * MCU targets, matching the remote API ABI. Keep the support gap
         * symmetric so local and remote stay in step.
         */
        for (size_t p = 0u; p < method->param_count; ++p) {
            const adsl_param_t *param = &method->params[p];

            if (param->stream != NULL) {
                continue;
            }

            if (param->type != NULL &&
                (strchr(param->type, '?') != NULL ||
                 strchr(param->type, '~') != NULL)) {
                set_error(error, error_capacity,
                    "nullable/collapsible parameters are not generated for MCU targets");
                return false;
            }
        }
    }

    return true;
}


/*
 * Local dispatcher emitter.
 *
 * The dispatcher consumes one method command from an inbound RX frame,
 * decodes the parameters with the same generic type codec used by
 * standalone DTOs and the response envelope decoder, then invokes the
 * user callback. It deliberately does not implement a Java-like future
 * registry: response-bearing methods simply receive the request_id as
 * their first callback argument, and the actual result/error emission is
 * a separate tooling phase.
 *
 * ABI:
 *
 *   aether_status_t <api>_dispatch(
 *       <api>_local_t *api,
 *       aether_meta_read_context_t *decode);
 *
 * Consumed bytes are visible through decode->reader.position. The next
 * command in the same RX buffer can be processed by calling dispatch
 * again with the same context.
 */


static bool emit_local_callback_declaration(
    FILE *output,
    const adsl_document_t *document,
    const adsl_api_t *api,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    char api_name[256];
    char method_name[256];

    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));

    fprintf(
        output,
        "typedef aether_status_t (*%s_%s_fn)(\n"
        "    void *ctx",
        api_name,
        method_name);

    if (method_is_response_bearing(method)) {
        fputs(",\n    uint32_t request_id", output);
    }

    for (size_t p = 0u; p < method->param_count; ++p) {
        const adsl_param_t *param = &method->params[p];

        fputs(",\n    ", output);

        if (!emit_parameter_type(
                output,
                document,
                param,
                error,
                error_capacity)) {
            return false;
        }

        fprintf(output, " %s", param->name);
    }

    fputs(");\n\n", output);

    return true;
}


static bool emit_local_api_header(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated local API header");
        return false;
    }

    char guard[256];
    char base_snake[256];
    char api_name[256];

    upper_name(base_name, guard, sizeof(guard));
    snake_name(base_name, base_snake, sizeof(base_snake));
    snake_name(api->name, api_name, sizeof(api_name));

    fprintf(
        output,
        "#ifndef %s_LOCAL_H\n"
        "#define %s_LOCAL_H\n\n"
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n",
        guard,
        guard,
        base_snake);


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective(
            document,
            api,
            &effective,
            &effective_count,
            error,
            error_capacity)) {
        fclose(output);
        return false;
    }

    for (size_t m = 0u; m < effective_count; ++m) {
        if (!emit_local_callback_declaration(
                output,
                document,
                api,
                effective[m].method,
                error,
                error_capacity)) {
            free(effective);
            fclose(output);
            return false;
        }
    }


    fprintf(
        output,
        "typedef struct {\n"
        "    void *ctx;\n");


    for (size_t m = 0u; m < effective_count; ++m) {
        char method_name[256];


        c_identifier(
            effective[m].method->name,
            method_name,
            sizeof(method_name));


        fprintf(
            output,
            "    %s_%s_fn %s;\n",
            api_name,
            method_name,
            method_name);
    }

    free(effective);


    fprintf(
        output,
        "} %s_local_t;\n\n"
        "aether_status_t %s_dispatch(\n"
        "    %s_local_t *api,\n"
        "    aether_meta_read_context_t *ctx);\n\n"
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n",
        api_name,
        api_name,
        api_name);

    if (ferror(output)) {
        set_error(error, error_capacity, "cannot write generated local API header");
        fclose(output);
        return false;
    }

    fclose(output);
    return true;
}


/*
 * Emit the local value declaration and decode statements for one method
 * parameter. The C declaration shape matches the remote API ABI emitted
 * by emit_parameter_type(); the decode statement uses the generic type
 * decoder.
 */
static bool emit_local_param_decode(
    FILE *output,
    const adsl_document_t *document,
    const adsl_param_t *param,
    const char *base_snake,
    char *error,
    size_t error_capacity) {

    const char *type = param->type;


    if (param->stream != NULL) {
        /*
         * Inline stream parameter: the stream name is not a declared
         * document type, it is a per-parameter struct typedef emitted
         * alongside the types header. Decode it explicitly rather than
         * going through the generic type lookup, and treat the payload as
         * a borrowed, read-only view into the RX frame.
         */
        char stream_name[256];

        declared_c_name(
            type,
            stream_name,
            sizeof(stream_name));

        fprintf(
            output,
            "        %s_t %s = {0};\n"
            "        {\n"
            "            const uint8_t *_meta_stream_data = NULL;\n"
            "            size_t _meta_stream_length = 0u;\n"
            "            if (!aether_meta_read_view(&ctx->reader,\n"
            "                    &_meta_stream_data,\n"
            "                    &_meta_stream_length)) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n"
            "            (%s).data = (uint8_t *)_meta_stream_data;\n"
            "            (%s).capacity = _meta_stream_length;\n"
            "            (%s).length = _meta_stream_length;\n"
            "        }\n",
            stream_name,
            param->name,
            param->name,
            param->name,
            param->name);

        return true;
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
            set_error(error, error_capacity,
                "generated array parameter alias is too long");
            return false;
        }

        if (array_dynamic) {
            fprintf(output, "        %s %s = {0};\n", alias, param->name);

            if (!emit_decode_value_expression(
                    output,
                    document,
                    type,
                    param->name,
                    base_snake,
                    0u,
                    error,
                    error_capacity)) {
                return false;
            }

            return true;
        }

        /*
         * Root static array parameter: the callback ABI passes it by
         * pointer, so back it with caller scratch instead of a hidden
         * MCU stack object (byte[512] would otherwise become a 512-byte
         * stack local).
         */
        fprintf(
            output,
            "        %s *%s = (%s *)aether_meta_scratch_alloc(\n"
            "            ctx,\n"
            "            sizeof(%s),\n"
            "            _Alignof(%s));\n"
            "        if (%s == NULL) {\n"
            "            return AETHER_ERR_OVERFLOW;\n"
            "        }\n"
            "        memset(%s, 0, sizeof(*%s));\n",
            alias, param->name, alias,
            alias, alias,
            param->name,
            param->name, param->name);

        char array_target[320];

        int array_target_size = snprintf(
            array_target,
            sizeof(array_target),
            "(*%s)",
            param->name);

        if (array_target_size < 0 ||
            (size_t)array_target_size >= sizeof(array_target)) {
            set_error(error, error_capacity,
                "generated array decode target is too long");
            return false;
        }

        if (!emit_decode_value_expression(
                output,
                document,
                type,
                array_target,
                base_snake,
                0u,
                error,
                error_capacity)) {
            return false;
        }

        return true;
    }


    char base[128];

    if (!scalar_type_name(type, base, sizeof(base))) {
        set_error(error, error_capacity, "unsupported parameter shape");
        return false;
    }

    const char *primitive = primitive_c_type(base);

    if (primitive != NULL) {
        fprintf(
            output,
            "        %s %s = {0};\n",
            primitive,
            param->name);

        if (!emit_decode_value_expression(
                output,
                document,
                type,
                param->name,
                base_snake,
                0u,
                error,
                error_capacity)) {
            return false;
        }

        return true;
    }

    const adsl_type_t *declared =
        find_declared_type(document, base);

    if (declared == NULL) {
        set_error(error, error_capacity, "unknown parameter type");
        return false;
    }

    char declared_name[256];
    declared_c_name(declared->name, declared_name, sizeof(declared_name));


    if (declared->stream != NULL) {
        /*
         * Declared stream parameter: the value is a named type whose only
         * purpose is to carry the stream shape. Decode it as a borrowed
         * view, taking the temporary const pointer first so the stream
         * struct's mutable `data` field (kept for the builder ABI) does not
         * violate the reader's const contract.
         */
        fprintf(
            output,
            "        %s_t %s = {0};\n"
            "        {\n"
            "            const uint8_t *_meta_stream_data = NULL;\n"
            "            size_t _meta_stream_length = 0u;\n"
            "            if (!aether_meta_read_view(&ctx->reader,\n"
            "                    &_meta_stream_data,\n"
            "                    &_meta_stream_length)) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n"
            "            (%s).data = (uint8_t *)_meta_stream_data;\n"
            "            (%s).capacity = _meta_stream_length;\n"
            "            (%s).length = _meta_stream_length;\n"
            "        }\n",
            declared_name,
            param->name,
            param->name,
            param->name,
            param->name);

        return true;
    }

    if (declared->enum_count > 0u) {
        fprintf(
            output,
            "        %s_t %s = {0};\n",
            declared_name,
            param->name);
    } else if (type_is_polymorphic_reference(document, declared)) {

        fprintf(
            output,
            "        %s_ref_t %s = {0};\n",
            declared_name,
            param->name);
    } else if (declared->is_abstract) {
        set_error(error, error_capacity,
            "abstract parameter has no concrete hierarchy descendants");
        return false;

    } else {
        /*
         * Root concrete struct parameter: the callback ABI is pointer-shaped,
         * so back the decoded struct with caller scratch. Large nested
         * structures must not grow the dispatcher stack frame on MCU.
         */
        fprintf(
            output,
            "        %s_t *%s = (%s_t *)aether_meta_scratch_alloc(\n"
            "            ctx,\n"
            "            sizeof(%s_t),\n"
            "            _Alignof(%s_t));\n"
            "        if (%s == NULL) {\n"
            "            return AETHER_ERR_OVERFLOW;\n"
            "        }\n"
            "        memset(%s, 0, sizeof(*%s));\n",
            declared_name, param->name, declared_name,
            declared_name, declared_name,
            param->name,
            param->name, param->name);

        char struct_target[320];

        int struct_target_size = snprintf(
            struct_target,
            sizeof(struct_target),
            "(*%s)",
            param->name);

        if (struct_target_size < 0 ||
            (size_t)struct_target_size >= sizeof(struct_target)) {
            set_error(error, error_capacity,
                "generated struct decode target is too long");
            return false;
        }

        if (!emit_decode_value_expression(
                output,
                document,
                type,
                struct_target,
                base_snake,
                0u,
                error,
                error_capacity)) {
            return false;
        }

        return true;
    }


    if (!emit_decode_value_expression(
            output,
            document,
            type,
            param->name,
            base_snake,
            0u,
            error,
            error_capacity)) {
        return false;
    }

    return true;
}


/*
 * Emit the callback invocation for one method. The pass expression follows
 * the remote parameter ABI in emit_parameter_type():
 *
 *   primitive / enum / dynamic array / stream -> by value
 *   static array / concrete struct / polymorphic ref -> address-of
 */
static bool emit_local_callback_invocation(
    FILE *output,
    const adsl_document_t *document,
    const adsl_api_t *api,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    char api_name[256];
    char method_name[256];

    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));

    fprintf(
        output,
        "        aether_status_t status =\n"
        "            api->%s(\n"
        "                api->ctx",
        method_name);

    if (method_is_response_bearing(method)) {
        fputs(",\n                request_id", output);
    }


    for (size_t p = 0u; p < method->param_count; ++p) {
        const adsl_param_t *param = &method->params[p];

        bool valid = false;
        c_param_kind_t kind =
            classify_param_kind(document, param, &valid);

        if (!valid) {
            set_error(error, error_capacity,
                "unsupported local parameter shape");
            return false;
        }

        /*
         * Scalar, enum, dynamic array view and stream/ref wrappers are
         * stack locals in the dispatcher; the callback ABI takes them by
         * value. Root concrete structs and root static arrays live in
         * caller scratch and the callback ABI takes them by pointer, so
         * their generated local is already a pointer and must not be
         * address-taken again.
         */
        bool needs_address =
            kind == C_PARAM_KIND_STREAM_PTR ||
            kind == C_PARAM_KIND_REF_PTR;

        fprintf(
            output,
            ",\n                %s%s",
            needs_address ? "&" : "",
            param->name);
    }


    fputs(");\n\n", output);

    return true;
}


static bool emit_local_api_source(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated local API source");
        return false;
    }

    char base_snake[256];
    char api_name[256];

    snake_name(base_name, base_snake, sizeof(base_snake));
    snake_name(api->name, api_name, sizeof(api_name));

    fprintf(
        output,
        "#include \"%s_local.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n"

        "aether_status_t %s_dispatch(\n"
        "    %s_local_t *api,\n"

        "    aether_meta_read_context_t *ctx) {\n\n"
        "    if (api == NULL || ctx == NULL) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    /*\n"
        "     * Scratch is a per-command temporary arena. Parameters decoded\n"
        "     * into RX-backed views or scratch storage are valid only for the\n"
        "     * duration of the local callback. Applications that need to\n"
        "     * retain them asynchronously must copy the required data into\n"
        "     * application-owned storage before returning from the callback.\n"
        "     * request_id may be retained, decoded pointer/view params may not.\n"
        "     */\n"
        "    size_t scratch_mark = ctx->scratch_position;\n\n"
        "    uint8_t command = 0u;\n\n"
        "    if (!aether_meta_read_u8(&ctx->reader, &command)) {\n"
        "        return AETHER_ERR_PROTOCOL;\n"
        "    }\n\n"
        "    switch (command) {\n",


        base_snake,
        api_name,
        api_name);


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective(
            document,
            api,
            &effective,
            &effective_count,
            error,
            error_capacity)) {
        fclose(output);
        return false;
    }

    for (size_t m = 0u; m < effective_count; ++m) {
        const adsl_method_t *method = effective[m].method;


        char method_name[256];
        c_identifier(method->name, method_name, sizeof(method_name));

        fprintf(
            output,
            "    case %uu: {\n",
            (unsigned int)effective[m].id);

        fprintf(
            output,
            "        if (api->%s == NULL) {\n"
            "            return AETHER_ERR_ARGUMENT;\n"
            "        }\n\n",
            method_name);

        if (method_is_response_bearing(method)) {
            fputs(
                "        uint32_t request_id = 0u;\n"
                "        if (!aether_meta_read_u32le(&ctx->reader, &request_id)) {\n"
                "            return AETHER_ERR_PROTOCOL;\n"
                "        }\n\n",
                output);
        }

        for (size_t p = 0u; p < method->param_count; ++p) {
            if (!emit_local_param_decode(
                    output,
                    document,
                    &method->params[p],
                    base_snake,
                    error,
                    error_capacity)) {
                fclose(output);
                return false;
            }
        }

        if (!emit_local_callback_invocation(
                output,
                document,
                api,
                method,
                error,
                error_capacity)) {
            fclose(output);
            return false;
        }


        fputs(
            "        ctx->scratch_position = scratch_mark;\n"
            "        return status;\n"
            "    }\n\n",
            output);
    }

    free(effective);


    fputs(
        "    default:\n"
        "        return AETHER_ERR_PROTOCOL;\n"
        "    }\n"
        "}\n",
        output);

    if (ferror(output)) {
        set_error(error, error_capacity, "cannot write generated local API source");
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

        set_error(error, error_capacity, "invalid local emitter arguments");
        return false;
    }

    const adsl_api_t *api =
        find_api_definition(document, selected_api_name);

    if (!local_api_supported(api, error, error_capacity)) {
        return false;
    }

    char base_snake[256];
    snake_name(base_name, base_snake, sizeof(base_snake));

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

    return emit_local_api_source(
        document,
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
    const adsl_document_t *document,
    const adsl_api_t *api,
    const char *name,
    char *error,
    size_t error_capacity) {

    if (api == NULL ||
        name == NULL) {

        return NULL;
    }

    /*
     * Response generation must resolve the method through the same
     * effective-method collector used by remote/local emission, so
     * `--response-method ChildApi.foo` picks the parent declaration when
     * a same-name child declaration is shadowed, exactly like Java.
     */
    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective(
            document,
            api,
            &effective,
            &effective_count,
            error,
            error_capacity)) {

        return NULL;
    }

    const adsl_effective_method_t *found =
        emitter_find_effective_method(
            effective,
            effective_count,
            name);

    const adsl_method_t *result =
        found != NULL ? found->method : NULL;

    free(effective);

    return result;
}






/*
 * Response decoder emitter.
 *
 * The response envelope (command byte + request id) is the only place with
 * response-specific knowledge. The result payload is decoded by the same
 * generic type codec that standalone ADSL types use, so enum, static
 * arrays, nested structs, polymorphic and inherited result values all
 * share one wire implementation with no parallel type-support matrix.
 *
 * Result C representation uses the canonical generated types emitted into
 * <base>_types.h. Dynamic non-byte arrays and polymorphic concrete children
 * land in caller-owned scratch. byte[] and String borrow the RX buffer.
 *
 * Response decoder ABI:
 *
 *   aether_status_t <api>_<method>_response_decode(
 *       aether_meta_read_context_t *ctx,
 *       <api>_<method>_response_frame_t *frame);
 *
 * The caller owns ctx->reader, ctx->scratch and frame. On success
 * ctx->reader.position points one past the consumed frame.
 */



static bool emit_field_declaration(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity) {

    if (name == NULL) {
        set_error(error, error_capacity, "result field has no name");
        return false;
    }

    char base[256];
    bool pointer;

    if (!format_c_type(
            document,
            type,
            base,
            sizeof(base),
            &pointer,
            error,
            error_capacity)) {
        return false;
    }

    fprintf(
        output,
        pointer ? "    const %s *%s;\n" : "    %s %s;\n",
        base,
        name);

    return true;
}





static bool method_returns_void(
    const adsl_method_t *method);



static bool response_emit_result_struct(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    /*
     * returns: void carries no result payload at all. Do not emit a
     * placeholder struct, and do not try to resolve a declared type
     * "void" against the ADSL type table.
     */
    if (method_returns_void(method)) {
        return true;
    }

    fprintf(output, "typedef struct {\n");


    if (method->returns_type != NULL) {
        if (!emit_field_declaration(
                output,
                document,
                method->returns_type,
                "value",
                error,
                error_capacity)) {
            return false;
        }
    } else {
        for (size_t i = 0u; i < method->return_field_count; ++i) {
            if (!emit_field_declaration(
                    output,
                    document,
                    method->return_fields[i].type,
                    method->return_fields[i].name,
                    error,
                    error_capacity)) {
                return false;
            }
        }
    }


    fprintf(
        output,
        "} %s_%s_result_t;\n\n",
        api_name,
        method_name);

    return true;
}




static bool method_returns_void(
    const adsl_method_t *method) {

    return method != NULL &&
           method->returns_type != NULL &&
           strcmp(method->returns_type, "void") == 0;
}


static bool response_method_supported(
    const adsl_document_t *document,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    (void)document;

    if (method == NULL) {
        set_error(error, error_capacity, "selected response method not found");
        return false;
    }

    if (method->throws_type != NULL) {
        set_error(error, error_capacity,
            "typed throws are not yet supported by the C response decoder");
        return false;
    }

    /*
     * returns: void is a response-bearing method: the wire format still
     * carries META_RESULT + request_id, but no result payload follows.
     * Java ApiGenerator treats it the same way.
     */
    if (method_returns_void(method)) {
        return true;
    }

    if (method->returns_type == NULL &&
        method->return_field_count == 0u) {
        set_error(error, error_capacity, "selected response method has no result");
        return false;
    }

    if (method->returns_type != NULL &&
        method->return_field_count > 0u) {
        set_error(error, error_capacity,
            "response method cannot mix named and anonymous result");
        return false;
    }

    return true;
}




static void response_emit_frame_declaration(
    FILE *output,
    const char *api_name,
    const char *method_name,
    const char *api_upper,
    const char *method_upper,
    bool has_result) {

    fprintf(
        output,
        "typedef enum {\n"
        "    %s_%s_RESPONSE_RESULT = 0,\n"
        "    %s_%s_RESPONSE_ERROR = 1\n"
        "} %s_%s_response_kind_t;\n\n",
        api_upper, method_upper,
        api_upper, method_upper,
        api_name, method_name);

    if (has_result) {
        fprintf(
            output,
            "typedef struct {\n"
            "    %s_%s_response_kind_t kind;\n"
            "    uint32_t request_id;\n"
            "    %s_%s_result_t result;\n"
            "} %s_%s_response_frame_t;\n\n",
            api_name, method_name,
            api_name, method_name,
            api_name, method_name);
    } else {
        /*
         * returns: void has no result payload. The frame intentionally
         * omits a result member so the wire semantics stay explicit.
         */
        fprintf(
            output,
            "typedef struct {\n"
            "    %s_%s_response_kind_t kind;\n"
            "    uint32_t request_id;\n"
            "} %s_%s_response_frame_t;\n\n",
            api_name, method_name,
            api_name, method_name);
    }

    fprintf(
        output,
        "aether_status_t %s_%s_response_decode(\n"
        "    aether_meta_read_context_t *ctx,\n"
        "    %s_%s_response_frame_t *frame);\n\n",
        api_name, method_name,
        api_name, method_name);
}



static bool emit_response_method_header(
    const adsl_document_t *document,
    const adsl_api_t *api,
    const adsl_method_t *method,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated response header");
        return false;
    }

    char guard[256];
    char base_snake[256];
    char api_name[256];
    char method_name[256];
    char api_upper[256];
    char method_upper[256];

    upper_name(base_name, guard, sizeof(guard));
    snake_name(base_name, base_snake, sizeof(base_snake));
    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));
    upper_name(api_name, api_upper, sizeof(api_upper));
    upper_name(method_name, method_upper, sizeof(method_upper));

    fprintf(
        output,
        "#ifndef %s_RESPONSE_H\n"
        "#define %s_RESPONSE_H\n\n"
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n",
        guard,
        guard,
        base_snake);

    bool ok =
        response_emit_result_struct(
            output,
            document,
            method,
            api_name,
            method_name,
            error,
            error_capacity);

    if (ok) {

        response_emit_frame_declaration(
            output,
            api_name,
            method_name,
            api_upper,
            method_upper,
            !method_returns_void(method));

    }

    fprintf(
        output,
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n");

    if (ferror(output)) {
        ok = false;
        set_error(error, error_capacity, "cannot write generated response header");
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

    FILE *output = fopen(path, "wb");

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
    c_identifier(method->name, method_name, sizeof(method_name));
    upper_name(api_name, api_upper, sizeof(api_upper));
    upper_name(method_name, method_upper, sizeof(method_upper));

    fprintf(
        output,
        "#include \"%s_response.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n"
        "aether_status_t %s_%s_response_decode(\n"
        "    aether_meta_read_context_t *ctx,\n"
        "    %s_%s_response_frame_t *frame) {\n\n"
        "    if (ctx == NULL || frame == NULL) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    memset(frame, 0, sizeof(*frame));\n\n"
        "    uint8_t command = 0u;\n"
        "    if (!aether_meta_read_u8(&ctx->reader, &command)) return AETHER_ERR_PROTOCOL;\n\n"
        "    uint32_t request_id = 0u;\n"
        "    if (!aether_meta_read_u32le(&ctx->reader, &request_id)) return AETHER_ERR_PROTOCOL;\n"
        "    frame->request_id = request_id;\n\n"
        "    if (command == 1u) {\n"
        "        frame->kind = %s_%s_RESPONSE_ERROR;\n"
        "        return AETHER_OK;\n"
        "    }\n\n"
        "    if (command != 0u) {\n"
        "        return AETHER_ERR_PROTOCOL;\n"
        "    }\n\n"
        "    frame->kind = %s_%s_RESPONSE_RESULT;\n\n",
        base_snake,
        api_name, method_name,
        api_name, method_name,
        api_upper, method_upper,
        api_upper, method_upper);

    bool ok;


    if (method_returns_void(method)) {
        ok = true;
    } else if (method->returns_type != NULL) {
        ok = emit_decode_value_expression(
            output,
            document,
            method->returns_type,

            "frame->result.value",
            base_snake,
            0u,
            error,
            error_capacity);
    } else {
        adsl_type_t synthetic = {
            NULL,
            false,
            0u,
            false,
            NULL,
            method->return_fields,
            method->return_field_count,
            NULL,
            0u,
            NULL
        };

        ok = emit_decode_struct_fields(
            output,
            document,
            &synthetic,
            "(&frame->result)",
            base_snake,
            0u,
            error,
            error_capacity);
    }

    if (ok) {
        fputs(
            "\n    return AETHER_OK;\n"
            "}\n",
            output);
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

        set_error(error, error_capacity, "invalid response emitter arguments");
        return false;
    }

    const adsl_api_t *api =
        response_find_api(document, selected_api_name);

    const adsl_method_t *method =

        response_find_method(
            document,
            api,
            selected_method_name,
            error,
            error_capacity);


    if (!response_method_supported(document, method, error, error_capacity)) {
        return false;
    }

    char base_snake[256];

    snake_name(base_name, base_snake, sizeof(base_snake));

    char types_h[1024];
    char types_c[1024];
    char response_h[1024];
    char response_c[1024];

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

    return emit_response_method_source(
        document,
        api,
        method,
        response_c,
        base_name,
        error,
        error_capacity);
}




/*
 * Generic symmetric ADSL type decoder emitter.
 *
 * The Java reference generator exposes one symmetric serialize/deserialize
 * codec per ADSL type. This emitter mirrors that design for MCU targets
 * without heap, reflection, or a Java-like object model:
 *
 *   - the caller provides aether_meta_read_context_t as the first argument;
 *   - RX bytes are borrowed views;
 *   - scratch storage is caller-owned and only used for polymorphic child
 *     objects, dynamic non-byte arrays, and pointer-shaped nullable fields;
 *   - primitive, enum, byte[], fixed byte[] decode without scratch;
 *   - wire semantics (field order, nullable bit = NULL, hierarchy
 *     discriminator, packed array count, META vs BODY) exactly follow the
 *     Java generator and the existing serializer.
 *
 * Standalone generated deserialize functions are emitted next to the
 * existing serialize functions. Response envelope decoding and any future
 * local dispatcher refactor must reuse this layer rather than maintain a
 * parallel type-support matrix.
 */

static bool emit_decode_struct_fields_recursive(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    size_t *bit_index,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated struct decode depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type(document, type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");
            return false;
        }

        if (!emit_decode_struct_fields_recursive(
                output,
                document,
                parent,
                target,
                base_snake,
                bit_index,
                depth + 1u,
                error,
                error_capacity)) {
            return false;
        }
    }

    for (size_t f = 0u; f < type->field_count; ++f) {
        const adsl_field_t *field = &type->fields[f];

        if (strchr(field->type, '~') != NULL) {
            set_error(
                error,
                error_capacity,
                "collapsible struct fields are not generated for MCU targets");
            return false;
        }

        bool nullable =
            strchr(field->type, '?') != NULL;

        char field_target[1024];

        int field_size =
            snprintf(
                field_target,
                sizeof(field_target),
                "%s->%s",
                target,
                field->name);

        if (field_size < 0 ||
            (size_t)field_size >= sizeof(field_target)) {
            set_error(
                error,
                error_capacity,
                "generated decode field expression is too long");
            return false;
        }

        if (!nullable) {
            if (!emit_decode_value_expression(
                    output,
                    document,
                    field->type,
                    field_target,
                    base_snake,
                    depth + 1u,
                    error,
                    error_capacity)) {
                return false;
            }

            continue;
        }

        char underlying[256];

        if (!strip_nullable_suffix(
                field->type,
                underlying,
                sizeof(underlying))) {
            set_error(
                error,
                error_capacity,
                "nullable field type is too long");
            return false;
        }

        fprintf(
            output,
            "    if ((_meta_nullable_mask & (UINT64_C(1) << %zuu)) == 0u) {\n",
            *bit_index);

        char array_element[128];
        bool array_dynamic;
        size_t array_static_size;

        bool field_is_array =
            parse_array_type(
                underlying,
                array_element,
                sizeof(array_element),
                &array_dynamic,
                &array_static_size);

        (void)array_element;
        (void)array_dynamic;
        (void)array_static_size;

        const char *ctype = NULL;
        char ctype_buf[256];

        if (field_is_array) {
            if (!array_alias_name(
                    document,
                    underlying,
                    ctype_buf,
                    sizeof(ctype_buf))) {
                set_error(
                    error,
                    error_capacity,
                    "nullable array alias name is too long");
                return false;
            }

            ctype = ctype_buf;
        } else {
            char base[128];

            if (!scalar_type_name(
                    underlying,
                    base,
                    sizeof(base))) {
                set_error(
                    error,
                    error_capacity,
                    "nullable field has unsupported shape");
                return false;
            }

            ctype = primitive_c_type(base);

            if (ctype == NULL) {
                const adsl_type_t *declared =
                    find_declared_type(document, base);

                if (declared == NULL) {
                    set_error(
                        error,
                        error_capacity,
                        "nullable field has unknown type");
                    return false;
                }

                char declared_name[256];
                declared_c_name(
                    declared->name,
                    declared_name,
                    sizeof(declared_name));

                if (declared->enum_count > 0u) {
                    snprintf(
                        ctype_buf,
                        sizeof(ctype_buf),
                        "%s_t",
                        declared_name);
                    ctype = ctype_buf;
                } else if (type_is_polymorphic_reference(
                               document,
                               declared)) {
                    snprintf(
                        ctype_buf,
                        sizeof(ctype_buf),
                        "%s_ref_t",
                        declared_name);
                    ctype = ctype_buf;
                } else if (declared->is_abstract) {
                    set_error(
                        error,
                        error_capacity,
                        "nullable abstract field is not generated");
                    return false;
                } else {
                    snprintf(
                        ctype_buf,
                        sizeof(ctype_buf),
                        "%s_t",
                        declared_name);
                    ctype = ctype_buf;
                }
            }
        }

        fprintf(
            output,

            "        %s *_meta_tmp_%u = (%s *)aether_meta_scratch_alloc(ctx, sizeof(%s), _Alignof(%s));\n"
            "        if (_meta_tmp_%u == NULL) return AETHER_ERR_OVERFLOW;\n"
            "        memset(_meta_tmp_%u, 0, sizeof(*_meta_tmp_%u));\n",
            ctype,
            depth,
            ctype,
            ctype,
            ctype,
            depth,
            depth,
            depth);



        char nullable_target[768];

        int nullable_target_size =
            snprintf(
                nullable_target,
                sizeof(nullable_target),
                "(*_meta_tmp_%u)",
                depth);

        if (nullable_target_size < 0 ||
            (size_t)nullable_target_size >= sizeof(nullable_target)) {
            set_error(
                error,
                error_capacity,
                "generated nullable expression is too long");
            return false;
        }

        if (!emit_decode_value_expression(
                output,
                document,
                underlying,
                nullable_target,
                base_snake,
                depth + 1u,
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,

            "        %s = _meta_tmp_%u;\n"
            "    } else {\n"
            "        %s = NULL;\n"
            "    }\n",
            field_target,
            depth,
            field_target);


        (*bit_index)++;
    }

    return true;
}


static bool emit_decode_struct_fields(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated struct decode depth exceeds 64");
        return false;
    }

    size_t nullable_count = 0u;

    if (!count_all_nullable_fields(
            document,
            type,
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


    /*
     * Wrap the whole BODY decode in its own lexical block so that a
     * nested struct field decoded inside this body cannot shadow
     * _meta_nullable_mask. Parent and child fields of one inheritance
     * body still share a single aggregate mask because the recursion
     * for parent fields stays inside this block.
     */
    fprintf(output, "    {\n");

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
            "    uint64_t _meta_nullable_mask = UINT64_C(0);\n");

        for (size_t b = 0u; b < mask_width; ++b) {
            fprintf(
                output,
                "    {\n"
                "        uint8_t _meta_mask_byte = 0u;\n"
                "        if (!aether_meta_read_u8(&ctx->reader, &_meta_mask_byte)) return AETHER_ERR_PROTOCOL;\n"
                "        _meta_nullable_mask |= ((uint64_t)_meta_mask_byte) << %zuu;\n"
                "    }\n",
                b * 8u);
        }

    }

    size_t bit_index = 0u;

    bool fields_ok =
        emit_decode_struct_fields_recursive(
            output,
            document,
            type,
            target,
            base_snake,
            &bit_index,
            depth,
            error,
            error_capacity);

    if (!fields_ok) {
        return false;
    }

    fprintf(output, "    }\n");

    return true;
}



static bool emit_decode_array_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated array decode depth exceeds 64");
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
            "invalid generated array decode type");
        return false;
    }

    if (dynamic &&
        strcmp(element, "byte") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_view(&ctx->reader, &(%s).data, &(%s).length)) return AETHER_ERR_PROTOCOL;\n",
            target,
            target);
        return true;
    }

    if (!dynamic &&
        strcmp(element, "byte") == 0) {
        fprintf(
            output,
            "    if (ctx->reader.position > ctx->reader.length ||\n"
            "        %zuu > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;\n"
            "    memcpy((%s).data, ctx->reader.data + ctx->reader.position, %zuu);\n"
            "    ctx->reader.position += %zuu;\n",
            static_size,
            target,
            static_size,
            static_size);
        return true;
    }

    if (!dynamic) {
        fprintf(
            output,

            "    for (size_t _meta_i_%u = 0u; _meta_i_%u < %zuu; ++_meta_i_%u) {\n",
            depth,
            depth,
            static_size,
            depth);


        char elem_target[1024];


        int elem_size =
            snprintf(
                elem_target,
                sizeof(elem_target),
                "%s.data[_meta_i_%u]",
                target,
                depth);


        if (elem_size < 0 ||
            (size_t)elem_size >= sizeof(elem_target)) {
            set_error(
                error,
                error_capacity,
                "generated array element expression is too long");
            return false;
        }

        if (!emit_decode_value_expression(
                output,
                document,
                element,
                elem_target,
                base_snake,
                depth + 1u,
                error,
                error_capacity)) {
            return false;
        }

        fputs(
            "    }\n",
            output);

        return true;
    }

    /*
     * Dynamic non-byte array: decode packed length, reserve element array
     * from caller scratch, then decode each element.
     */
    const char *ctype = primitive_c_type(element);
    char ctype_buf[256];

    if (ctype == NULL) {
        const adsl_type_t *declared =
            find_declared_type(document, element);

        if (declared == NULL) {
            set_error(
                error,
                error_capacity,
                "unknown dynamic array element type");
            return false;
        }

        char declared_name[256];
        declared_c_name(
            declared->name,
            declared_name,
            sizeof(declared_name));

        if (declared->enum_count > 0u) {
            snprintf(
                ctype_buf,
                sizeof(ctype_buf),
                "%s_t",
                declared_name);
            ctype = ctype_buf;
        } else if (type_is_polymorphic_reference(
                       document,
                       declared)) {
            snprintf(
                ctype_buf,
                sizeof(ctype_buf),
                "%s_ref_t",
                declared_name);
            ctype = ctype_buf;
        } else if (declared->is_abstract) {
            set_error(
                error,
                error_capacity,
                "abstract dynamic array element type is not generated");
            return false;
        } else {
            snprintf(
                ctype_buf,
                sizeof(ctype_buf),
                "%s_t",
                declared_name);
            ctype = ctype_buf;
        }
    }

    fprintf(
        output,

        "    {\n"
        "        uint64_t _meta_len64_%u = 0u;\n"
        "        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_%u)) return AETHER_ERR_PROTOCOL;\n"
        "        if (_meta_len64_%u > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;\n"
        "        size_t _meta_len_%u = (size_t)_meta_len64_%u;\n"
        "        %s *_meta_buf_%u = NULL;\n"
        "        if (_meta_len_%u > 0u) {\n"
        "            if (_meta_len_%u > SIZE_MAX / sizeof(%s)) return AETHER_ERR_PROTOCOL;\n"
        "            _meta_buf_%u = (%s *)aether_meta_scratch_alloc(ctx, _meta_len_%u * sizeof(%s), _Alignof(%s));\n"
        "            if (_meta_buf_%u == NULL) return AETHER_ERR_OVERFLOW;\n"
        "            memset(_meta_buf_%u, 0, _meta_len_%u * sizeof(%s));\n"
        "        }\n"
        "        for (size_t _meta_i_%u = 0u; _meta_i_%u < _meta_len_%u; ++_meta_i_%u) {\n",
        depth, depth, depth, depth, depth,
        ctype, depth,
        depth,
        depth, ctype,
        depth, ctype, depth, ctype, ctype,
        depth,
        depth, depth, ctype,
        depth, depth, depth, depth);



    char array_target[768];

    int array_target_size =
        snprintf(
            array_target,
            sizeof(array_target),
            "_meta_buf_%u[_meta_i_%u]",
            depth,
            depth);

    if (array_target_size < 0 ||
        (size_t)array_target_size >= sizeof(array_target)) {
        set_error(
            error,
            error_capacity,
            "generated array element expression is too long");
        return false;
    }

    if (!emit_decode_value_expression(
            output,
            document,
            element,
            array_target,
            base_snake,
            depth + 1u,
            error,
            error_capacity)) {
        return false;
    }


    fprintf(
        output,


        "        }\n"
        "        (%s).data = _meta_buf_%u;\n"
        "        (%s).length = _meta_len_%u;\n"
        "    }\n",
        target,
        depth,
        target,
        depth);


    return true;
}


static bool emit_decode_hierarchy_expression(
    FILE *output,
    const adsl_document_t *document,
    const adsl_type_t *static_type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated hierarchy decode depth exceeds 64");
        return false;
    }

    fprintf(
        output,

        "    {\n"
        "        uint8_t _meta_type_id_%u = 0u;\n"
        "        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_%u)) return AETHER_ERR_PROTOCOL;\n"
        "        switch (_meta_type_id_%u) {\n",
        depth,
        depth,
        depth);


    size_t concrete_count = 0u;

    for (size_t i = 0u; i < document->type_count; ++i) {
        const adsl_type_t *candidate = &document->types[i];

        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self(
                document,
                candidate,
                static_type)) {
            continue;
        }

        if (!candidate->has_id ||
            candidate->id == 0u) {
            continue;
        }

        char candidate_name[256];
        declared_c_name(
            candidate->name,
            candidate_name,
            sizeof(candidate_name));

        fprintf(
            output,

            "        case %uu: {\n"
            "            %s_t *_meta_child_%u = (%s_t *)aether_meta_scratch_alloc(ctx, sizeof(%s_t), _Alignof(%s_t));\n"
            "            if (_meta_child_%u == NULL) return AETHER_ERR_OVERFLOW;\n"
            "            memset(_meta_child_%u, 0, sizeof(*_meta_child_%u));\n",
            (unsigned int)candidate->id,
            candidate_name, depth,
            candidate_name, candidate_name, candidate_name,
            depth,
            depth, depth);



        char child_target[256];

        int child_target_size =
            snprintf(
                child_target,
                sizeof(child_target),
                "_meta_child_%u",
                depth);

        if (child_target_size < 0 ||
            (size_t)child_target_size >= sizeof(child_target)) {
            set_error(
                error,
                error_capacity,
                "generated hierarchy child expression is too long");
            return false;
        }

        if (!emit_decode_struct_fields(
                output,
                document,
                candidate,
                child_target,
                base_snake,
                depth + 1u,
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,

            "            (%s).as.%s = _meta_child_%u;\n"
            "            break;\n"
            "        }\n",
            target,
            candidate_name,
            depth);


        concrete_count++;
    }

    (void)concrete_count;

    fprintf(
        output,

        "        default:\n"
        "            return AETHER_ERR_PROTOCOL;\n"
        "        }\n"
        "        (%s).type_id = _meta_type_id_%u;\n"
        "    }\n",
        target,
        depth);


    return true;
}


static bool emit_decode_value_expression(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated decode depth exceeds 64");
        return false;
    }

    if (strchr(type, '~') != NULL) {
        set_error(
            error,
            error_capacity,
            "collapsible values are not generated for MCU targets");
        return false;
    }

    if (strchr(type, '?') != NULL) {
        set_error(
            error,
            error_capacity,
            "nullable values must be decoded by their containing field mask");
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
        return emit_decode_array_expression(
            output,
            document,
            type,
            target,
            base_snake,
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
            "unsupported generated decode shape");
        return false;
    }

    if (strcmp(base, "bool") == 0 ||
        strcmp(base, "boolean") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint8_t _meta_bool = 0u;\n"
            "        if (!aether_meta_read_u8(&ctx->reader, &_meta_bool)) return AETHER_ERR_PROTOCOL;\n"
            "        (%s) = ((int8_t)_meta_bool > 0) ? true : false;\n"
            "    }\n",
            target);
        return true;
    }

    if (strcmp(base, "byte") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint8_t _meta_byte = 0u;\n"
            "        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;\n"
            "        (%s) = (int8_t)_meta_byte;\n"
            "    }\n",
            target);
        return true;
    }

    if (strcmp(base, "short") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_i16le(&ctx->reader, &(%s))) return AETHER_ERR_PROTOCOL;\n",
            target);
        return true;
    }

    if (strcmp(base, "int") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_i32le(&ctx->reader, &(%s))) return AETHER_ERR_PROTOCOL;\n",
            target);
        return true;
    }

    if (strcmp(base, "long") == 0 ||
        strcmp(base, "date") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_i64le(&ctx->reader, &(%s))) return AETHER_ERR_PROTOCOL;\n",
            target);
        return true;
    }

    if (strcmp(base, "float") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint32_t _meta_bits = 0u;\n"
            "        if (!aether_meta_read_u32le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;\n"
            "        memcpy(&(%s), &_meta_bits, sizeof(_meta_bits));\n"
            "    }\n",
            target);
        return true;
    }

    if (strcmp(base, "double") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint64_t _meta_bits = 0u;\n"
            "        if (!aether_meta_read_u64le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;\n"
            "        memcpy(&(%s), &_meta_bits, sizeof(_meta_bits));\n"
            "    }\n",
            target);
        return true;
    }

    if (strcmp(base, "uuid") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_uuid(&ctx->reader, &(%s))) return AETHER_ERR_PROTOCOL;\n",
            target);
        return true;
    }

    if (strcmp(base, "string") == 0 ||
        strcmp(base, "uri") == 0) {
        fprintf(
            output,
            "    if (!aether_meta_read_view(&ctx->reader, &(%s).data, &(%s).length)) return AETHER_ERR_PROTOCOL;\n",
            target,
            target);
        return true;
    }

    if (strcmp(base, "intpack") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint64_t _meta_pack = 0u;\n"
            "        if (!aether_meta_read_pack(&ctx->reader, &_meta_pack)) return AETHER_ERR_PROTOCOL;\n"
            "        (%s) = (int64_t)_meta_pack;\n"
            "    }\n",
            target);
        return true;
    }

    const adsl_type_t *declared_type =
        find_declared_type(document, base);

    if (declared_type == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated decode type");
        return false;
    }


    if (declared_type->stream != NULL) {
        /*
         * Incoming stream borrows the immutable RX frame. The historical
         * stream_t representation uses uint8_t* because it is also used as
         * an outgoing builder, but data passed by the decoder is read-only
         * and must not outlive the RX buffer.
         */
        fprintf(
            output,
            "    if (!aether_meta_read_view(&ctx->reader, &(%s).data, &(%s).length)) return AETHER_ERR_PROTOCOL;\n"
            "    (%s).capacity = (%s).length;\n",
            target,
            target,
            target,
            target);
        return true;
    }


    if (declared_type->enum_count > 0u) {
        char enum_name[256];
        declared_c_name(
            declared_type->name,
            enum_name,
            sizeof(enum_name));

        fprintf(
            output,
            "    {\n"
            "        uint8_t _meta_enum = 0u;\n"
            "        if (!aether_meta_read_u8(&ctx->reader, &_meta_enum)) return AETHER_ERR_PROTOCOL;\n"
            "        if (_meta_enum >= %zuu) return AETHER_ERR_PROTOCOL;\n"
            "        (%s) = (%s_t)_meta_enum;\n"
            "    }\n",
            declared_type->enum_count,
            target,
            enum_name);
        return true;
    }

    if (type_is_polymorphic_reference(
            document,
            declared_type)) {
        return emit_decode_hierarchy_expression(
            output,
            document,
            declared_type,
            target,
            base_snake,
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

    fprintf(
        output,
        "    /* concrete struct value */\n");


    {
        char address[1024];
        int address_size =
            snprintf(
                address,
                sizeof(address),
                "(&(%s))",
                target);

        if (address_size < 0 ||
            (size_t)address_size >= sizeof(address)) {
            set_error(
                error,
                error_capacity,
                "generated concrete struct decode target is too long");
            return false;
        }

        return emit_decode_struct_fields(
            output,
            document,
            declared_type,
            address,
            base_snake,
            depth,
            error,
            error_capacity);
    }

}