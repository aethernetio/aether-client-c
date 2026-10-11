

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
#include "adsl_project_internal.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>




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


/*
 * Build "<directory>/<stem><suffix>" into a fixed buffer.
 *
 * The emitter writes every output file through a bounded path buffer. A
 * silently truncated path is dangerous: fopen() may succeed on a prefix
 * name and the caller never learns that the intended file was not
 * written. This helper therefore fails loudly on truncation so callers
 * can report the error before any file is opened.
 */
static bool build_output_path(
    char *buffer,
    size_t capacity,
    const char *directory,
    const char *stem,
    const char *suffix) {

    int written =
        snprintf(
            buffer,
            capacity,
            "%s/%s%s",
            directory,
            stem,
            suffix);

    if (written < 0 ||
        (size_t)written >= capacity) {
        return false;
    }

    return true;
}




/*
 * Generated-origin comments are intentionally stable and grep-friendly.
 *
 * They are part of the generated-code usability contract: a developer or AI
 * should be able to jump from a C symbol to the physical ADSL declaration
 * without reading the generator implementation.
 */
static void emit_type_origin(
    FILE *output,
    const adsl_type_t *type) {

    fprintf(
        output,
        "/* ADSL: %s:%zu types.%s */\n",
        type->source_path != NULL
            ? type->source_path
            : "<unknown>",
        type->source_line,
        type->name);
}


static void emit_method_origin(
    FILE *output,
    const adsl_api_t *owner,
    const adsl_method_t *method) {

    const char *source_path =
        method->source_path != NULL
            ? method->source_path
            : owner->source_path != NULL
                ? owner->source_path
                : "<unknown>";

    size_t source_line =
        method->source_line != 0u
            ? method->source_line
            : owner->source_line;

    fprintf(
        output,
        "/* ADSL: %s:%zu api.%s.methods.%s */\n",
        source_path,
        source_line,
        owner->name,
        method->name);
}



/*
 * Abbreviation of a module (source .adsl.yaml file) used to prefix every
 * generated C identifier that belongs to that module. This guarantees
 * that two files declaring the same short type name ("Key") generate
 * distinct C symbols ("csa_Key_t", "pa_Key_t") when emitted into a
 * single aether_api.h/aether_api.c.
 *
 * Rule:
 *   - Split the basename (without ".adsl.yaml"/".adsl.yml") into
 *     segments on lower->upper transitions and on any non-letter byte.
 *   - If there is more than one segment: take the first letter of each
 *     and lowercase it.
 *   - If there is exactly one segment: take the whole segment,
 *     lowercased.
 *
 * Examples: ClientServerApi -> "csa", PeerApi -> "pa",
 *           clienttypes -> "clienttypes", aether_reflection -> "ar".
 */
static void abbreviate_from_path(
    const char *path,
    char *target,
    size_t capacity) {

    if (target == NULL || capacity == 0u) {
        return;
    }

    target[0] = '\0';

    if (path == NULL || path[0] == '\0') {
        return;
    }

    const char *base = path;
    for (const char *p = path; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            base = p + 1;
        }
    }

    char stem[256];
    size_t stem_length = 0u;
    for (const char *p = base;
         *p != '\0' && stem_length + 1u < sizeof(stem);
         ++p) {
        stem[stem_length++] = *p;
    }
    stem[stem_length] = '\0';

    const char *suffix_yaml = ".adsl.yaml";
    const char *suffix_yml = ".adsl.yml";
    size_t stem_sz = strlen(stem);
    size_t yaml_sz = strlen(suffix_yaml);
    size_t yml_sz = strlen(suffix_yml);

    if (stem_sz > yaml_sz &&
        strcmp(stem + stem_sz - yaml_sz, suffix_yaml) == 0) {
        stem[stem_sz - yaml_sz] = '\0';
    } else if (stem_sz > yml_sz &&
               strcmp(stem + stem_sz - yml_sz, suffix_yml) == 0) {
        stem[stem_sz - yml_sz] = '\0';
    }

    size_t segment_count = 0u;
    char first_letters[64];
    size_t first_letter_count = 0u;
    bool segment_open = false;

    for (size_t i = 0u; stem[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)stem[i];
        bool is_alpha = isalpha(c) != 0;
        bool starts_new =
            !segment_open ||
            (is_alpha && isupper(c) && segment_open &&
             i > 0u && islower((unsigned char)stem[i - 1u]));

        if (!is_alpha) {
            if (segment_open) {
                segment_open = false;
            }
            continue;
        }

        if (starts_new) {
            segment_count++;
            if (first_letter_count + 1u < sizeof(first_letters)) {
                first_letters[first_letter_count++] =
                    (char)tolower(c);
            }
        }

        segment_open = true;
    }

    first_letters[first_letter_count] = '\0';

    if (segment_count <= 1u) {
        size_t length = strlen(stem);
        size_t out = 0u;
        for (size_t i = 0u;
             i < length && out + 1u < capacity;
             ++i) {
            unsigned char c = (unsigned char)stem[i];
            if (isalnum(c)) {
                target[out++] = (char)tolower(c);
            }
        }
        target[out] = '\0';
        return;
    }

    size_t out = 0u;
    for (size_t i = 0u;
         i < first_letter_count && out + 1u < capacity;
         ++i) {
        target[out++] = first_letters[i];
    }
    target[out] = '\0';
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
 * The emitter context is defined in src/adsl_project_internal.h so
 * that adsl.c and tests can construct one without duplicating the
 * layout. See that header for the ownership contract of each field.
 */





static emitter_ctx_t emitter_ctx_standalone(
    const adsl_document_t *document) {

    emitter_ctx_t ctx;
    ctx.document = document;
    ctx.resolver = NULL;
    ctx.project = NULL;
    ctx.plan = NULL;
    ctx.base_snake[0] = '\0';
    ctx.module_abbr[0] = '\0';

    if (document != NULL && document->root_source_path != NULL) {
        abbreviate_from_path(
            document->root_source_path,
            ctx.module_abbr,
            sizeof(ctx.module_abbr));
    }

    return ctx;
}





/*
 * Enumeration helpers used by the top-level emitters.
 *
 * Standalone mode enumerates the document's include closure
 * (resolved_types / resolved_apis). Project mode enumerates only the
 * document's own declarations (types / apis); cross-document references
 * are resolved through the ctx resolver, never through enumeration.
 */
static const adsl_type_t *ctx_types(
    const emitter_ctx_t *ctx,
    size_t *count) {

    if (ctx->resolver != NULL) {
        *count = ctx->document->type_count;
        return ctx->document->types;
    }


    /*
     * Legacy parsed documents may not have gone through the include
     * resolution pass yet (adsl_parse_file without adsl_load_file), so
     * fall back to the root arrays. Merged/project documents alias the
     * two views and are unaffected.
     */
    if (ctx->document->resolved_type_count > 0u) {
        *count = ctx->document->resolved_type_count;
        return ctx->document->resolved_types;
    }

    *count = ctx->document->type_count;
    return ctx->document->types;

}


static const adsl_api_t *ctx_apis(
    const emitter_ctx_t *ctx,
    size_t *count) {

    if (ctx->resolver != NULL) {
        *count = ctx->document->api_count;
        return ctx->document->apis;
    }


    if (ctx->document->resolved_api_count > 0u) {
        *count = ctx->document->resolved_api_count;
        return ctx->document->resolved_apis;
    }

    *count = ctx->document->api_count;
    return ctx->document->apis;

}



/*
 * Hierarchy enumeration helpers.
 *
 * Polymorphism detection and hierarchy ref / switch-table generation
 * must see every descendant declared anywhere in the project, not only
 * in the current document. Standalone mode uses the include closure;
 * project mode uses the resolver's global type view (deduplicated by
 * source_path+name) so that a Base declared in one document sees a
 * Child declared in another.
 *
 * This is deliberately distinct from ctx_types(), which enumerates only
 * what the current module must generate.
 */
static size_t ctx_hierarchy_type_count(
    const emitter_ctx_t *ctx) {

    if (ctx->resolver != NULL) {
        return adsl_project_resolver_type_count(ctx->resolver);
    }

    return ctx->document->resolved_type_count;
}


static const adsl_type_t *ctx_hierarchy_type_at(
    const emitter_ctx_t *ctx,
    size_t index) {

    if (ctx->resolver != NULL) {
        return adsl_project_resolver_type_at(ctx->resolver, index);
    }

    return &ctx->document->resolved_types[index];
}



/*
 * Index of a type in the current module's emission array, or SIZE_MAX
 * if the type does not belong to the current module.
 *
 * The emission array is resolved_types[] in standalone mode (the
 * include closure merged into one document) and types[] in project mode
 * (only the current document's own declarations). Callers use this to
 * decide whether a referenced type must be emitted locally or handled
 * as a cross-module dependency by the project emitter.
 *
 * This deliberately does not compute indices across arrays: subtracting
 * pointers that belong to different arrays is undefined behaviour, and
 * a project can reference a type declared in another document.
 */

static size_t ctx_local_type_index(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type) {

    const adsl_type_t *base;
    size_t count;


    if (ctx->resolver != NULL) {
        base = ctx->document->types;
        count = ctx->document->type_count;
    } else if (ctx->document->resolved_type_count > 0u) {
        base = ctx->document->resolved_types;
        count = ctx->document->resolved_type_count;
    } else {
        base = ctx->document->types;
        count = ctx->document->type_count;
    }


    /*
     * Compare element addresses for equality rather than performing
     * pointer subtraction across potentially unrelated arrays. If the
     * referenced type is declared in a different project module, its
     * address will not match any local element and the caller treats it
     * as an external by-value dependency.
     */
    for (size_t i = 0u; i < count; ++i) {
        if (&base[i] == type) {
            return i;
        }
    }

    return SIZE_MAX;
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



/*
 * Convert an ADSL type name into its generated C identifier.
 *
 * Trailing nullable/collapsible modifiers ('?' and '~') are semantic
 * suffixes that never belong in the C symbol name. Stripping them here
 * gives every caller -- header declarations, serializers, and the local
 * dispatcher -- one canonical name for the same stream type. Without this
 * the nullable-stream declaration path and the decode path would disagree
 * (e.g. "test_stream__t" vs "test_stream_t") and generated code would not
 * compile.
 */

static bool strip_nullable_suffix(
    const char *type,
    char *result,
    size_t capacity);




static void declared_c_name(
    const char *abbr,
    const char *source,
    char *target,
    size_t capacity) {

    if (capacity == 0u) {
        return;
    }


    char stripped[256];

    if (!strip_nullable_suffix(source, stripped, sizeof(stripped))) {
        stripped[0] = '\0';
    }

    char snake[256];
    snake_name(stripped, snake, sizeof(snake));

    size_t pos = 0u;

    if (abbr != NULL && abbr[0] != '\0') {
        for (size_t i = 0u;
             abbr[i] != '\0' && pos + 1u < capacity;
             ++i) {
            target[pos++] = abbr[i];
        }
        if (pos + 1u < capacity) {
            target[pos++] = '_';
        }
    }

    for (size_t i = 0u;
         snake[i] != '\0' && pos + 1u < capacity;
         ++i) {
        target[pos++] = snake[i];
    }

    target[pos] = '\0';

}


/*
 * Pure provenance prefix: the C symbol prefix implied by the physical ADSL
 * file that declares the type. No fallback, no context. Returns an empty
 * string when the declaration has no source_path.
 */
static void type_symbol_prefix(
    const adsl_type_t *type,
    char *out,
    size_t capacity) {

    if (out == NULL || capacity == 0u) {
        return;
    }

    out[0] = '\0';

    if (type != NULL &&
        type->source_path != NULL &&
        type->source_path[0] != '\0') {

        abbreviate_from_path(
            type->source_path,
            out,
            capacity);
    }
}

/*
 * The single authority for a type declaration's effective C symbol prefix.
 *
 * Every generated symbol of a declaration -- typedef, struct tag,
 * serializer, deserializer, *_with_depth entry points, tiered-int tables,
 * hierarchy helpers and cross-file references -- must obtain its prefix
 * here. No other code path may invent or fall back to a prefix.
 *
 * Rules, in order:
 *   1. If the declaration has a source_path, the prefix is derived from
 *      that file. Parser-produced declarations always have one, so this is
 *      the normal case in every mode.
 *   2. Otherwise the declaration has no provenance. A fallback to the
 *      current module name is permitted ONLY for the historical standalone
 *      single-file wrappers: resolver == NULL, module_abbr non-empty, and
 *      the document was loaded from one physical file
 *      (document->root_source_path != NULL). In unified/merged mode the
 *      merged document has no root_source_path, so this fallback can never
 *      apply there.
 *   3. Anything else is a hard error: a declaration with no usable origin
 *      must not silently produce symbols.
 */
static bool type_effective_prefix_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (out == NULL || capacity == 0u) {
        set_error(error, error_capacity, "invalid type prefix buffer");
        return false;
    }

    out[0] = '\0';

    type_symbol_prefix(type, out, capacity);

    if (out[0] != '\0') {
        return true;
    }

    bool legacy_single_file =
        ctx != NULL &&
        ctx->resolver == NULL &&
        ctx->module_abbr[0] != '\0' &&
        ctx->document != NULL &&
        ctx->document->root_source_path != NULL;

    if (legacy_single_file) {

        (void)snprintf(
            out,
            capacity,
            "%s",
            ctx->module_abbr);

        return out[0] != '\0';
    }

    set_error(
        error,
        error_capacity,
        "type declaration has no source_path and no legacy module prefix");

    return false;
}



/*
 * Effective C symbol prefix for an API declaration. Mirrors the rules of
 * type_effective_prefix_ctx(): the prefix is taken from the physical ADSL
 * file that declares the API. In merged/unified mode the document has no
 * root_source_path, so this is the only correct source for the prefix.
 */
static bool api_effective_prefix_ctx(
    const emitter_ctx_t *ctx,
    const adsl_api_t *api,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (out == NULL || capacity == 0u) {
        set_error(error, error_capacity, "invalid api prefix buffer");
        return false;
    }

    out[0] = '\0';

    if (api != NULL &&
        api->source_path != NULL &&
        api->source_path[0] != '\0') {

        abbreviate_from_path(
            api->source_path,
            out,
            capacity);
    }

    if (out[0] != '\0') {
        return true;
    }

    bool legacy_single_file =
        ctx != NULL &&
        ctx->resolver == NULL &&
        ctx->module_abbr[0] != '\0' &&
        ctx->document != NULL &&
        ctx->document->root_source_path != NULL;

    if (legacy_single_file) {
        (void)snprintf(out, capacity, "%s", ctx->module_abbr);
        return out[0] != '\0';
    }

    set_error(
        error,
        error_capacity,
        "api declaration has no source_path and no legacy module prefix");

    return false;
}





/*
 * Full prefixed C name of a resolved type declaration:
 * "<effective prefix>_<snake(name)>". This is the only way callers should
 * obtain a type's C name; the prefix is decided solely by
 * type_effective_prefix_ctx(), so typedefs, serializers and references of
 * the same declaration can never disagree.
 */
static bool declared_type_c_name_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (type == NULL || out == NULL || capacity == 0u) {
        set_error(error, error_capacity, "invalid type name request");
        return false;
    }

    char prefix[32];

    if (!type_effective_prefix_ctx(
            ctx, type, prefix, sizeof(prefix), error, error_capacity)) {
        return false;
    }

    declared_c_name(prefix, type->name, out, capacity);

    return out[0] != '\0';
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


static const adsl_type_t *find_declared_type_ctx(
    const emitter_ctx_t *ctx,
    const char *name);





static bool type_is_polymorphic_reference(
    const adsl_document_t *document,
    const adsl_type_t *type);





static bool type_has_children_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type);


static bool type_is_polymorphic_reference_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type);

static bool type_is_descendant_or_self_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *candidate,
    const adsl_type_t *ancestor);







typedef struct {
    char aliases[256][256];
    size_t count;
} array_alias_state_t;



static bool emit_hierarchy_ref_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *error,
    size_t error_capacity);





static bool emit_hierarchy_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *static_type,
    const char *expression,
    const char *prefix,
    bool reference_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);











static bool emit_all_struct_dependencies_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    uint8_t *states,
    unsigned int depth,
    array_alias_state_t *alias_state,
    char *error,
    size_t error_capacity);

static bool emit_all_struct_field_declarations_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    unsigned int depth,
    char *error,
    size_t error_capacity);

static bool emit_struct_definition_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    size_t type_index,
    uint8_t *states,
    array_alias_state_t *alias_state,
    char *error,
    size_t error_capacity);


static bool count_all_nullable_fields_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    unsigned int depth,
    size_t *count,
    char *error,
    size_t error_capacity);

static bool emit_all_nullable_mask_bits_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    size_t *bit_index,
    char *error,
    size_t error_capacity);

static bool emit_all_struct_payload_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
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

    /*
     * Java TypeInfo parses '~' before '?'. Both are semantic suffix
     * modifiers and neither changes the underlying wire representation.
     * Strip the complete modifier suffix before normal type/codec lookup.
     */
    size_t length =
        strlen(type);

    while (length > 0u &&
           (type[length - 1u] == '?' ||
            type[length - 1u] == '~')) {

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


static bool array_alias_name_ctx(
    const emitter_ctx_t *ctx,
    const char *type,
    char *name,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
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
        find_declared_type_ctx(ctx, element);

    if (declared != NULL) {

        if (!declared_type_c_name_ctx(
                ctx,
                declared,
                stem,
                sizeof(stem),
                error,
                error_capacity)) {
            return false;
        }


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


static bool array_alias_name(
    const adsl_document_t *document,
    const char *type,
    char *name,
    size_t capacity) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return array_alias_name_ctx(&ctx, type, name, capacity, NULL, 0u);
}



static bool emit_array_element_c_type_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *element,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid array element context");
        return false;
    }

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
        find_declared_type_ctx(ctx, element);

    if (declared == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated array element type");

        return false;
    }


    char generated_name[256];

    if (!declared_type_c_name_ctx(
            ctx,
            declared,
            generated_name,
            sizeof(generated_name),
            error,
            error_capacity)) {
        return false;
    }



    if (declared->enum_count > 0u) {
        fprintf(
            output,
            "%s_t",
            generated_name);

        return true;
    }


    if (type_is_polymorphic_reference_ctx(
            ctx,
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









static bool has_array_alias(
        const array_alias_state_t *state,
        const char *alias) {
    for (size_t i = 0u; i < state->count; ++i) {
        if (strcmp(state->aliases[i], alias) == 0) {
            return true;
        }
    }
    return false;
}

static bool remember_array_alias(
        array_alias_state_t *state,
        const char *alias,
        char *error,
        size_t error_capacity) {
    if (state->count >= 256u) {
        set_error(error, error_capacity, "too many generated array aliases");
        return false;
    }

    strcpy(state->aliases[state->count++], alias);
    return true;
}


static bool emit_array_alias_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    array_alias_state_t *alias_state,
    const char *type,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid array alias context");
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

        return true;
    }

    if (dynamic &&
        strcmp(element, "byte") == 0) {

        return true;
    }

    char alias[256];

    if (!array_alias_name_ctx(
            ctx,
            type,
            alias,
            sizeof(alias),
            error,
            error_capacity)) {

        if (error != NULL && error[0] == '\0') {
            set_error(
                error,
                error_capacity,
                "generated array alias name is too long");
        }

        return false;
    }

    if (has_array_alias(alias_state, alias)) {
        return true;
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

        if (!emit_array_element_c_type_ctx(
                output,
                ctx,
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

    } else {

        fputs(
            "typedef struct {\n"
            "    ",
            output);

        if (!emit_array_element_c_type_ctx(
                output,
                ctx,
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
    }

    return remember_array_alias(
        alias_state,
        alias,
        error,
        error_capacity);
}







/*
 * A fixed-size array embeds its elements inline. If the element is a normal
 * generated struct, C therefore requires the struct body to be complete
 * before the array typedef is emitted. Dynamic arrays contain only a pointer,
 * while enums, hierarchy refs, streams and primitives are already complete at
 * the early alias-emission stage.
 */

static bool array_alias_requires_complete_element_ctx(
    const emitter_ctx_t *ctx,
    const char *type) {

    if (ctx == NULL || ctx->document == NULL || type == NULL) {
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
            &static_size) ||
        dynamic) {

        return false;
    }

    (void)static_size;

    const adsl_type_t *declared =
        find_declared_type_ctx(ctx, element);

    return declared != NULL &&
           !declared->has_tiered_int &&
           declared->enum_count == 0u &&
           !declared->is_abstract &&
           declared->stream == NULL &&
           !type_is_polymorphic_reference_ctx(ctx, declared);
}









static bool emit_array_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const char *type,
    const char *expression,
    const char *prefix,
    bool array_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);







static bool emit_struct_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *declared_type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,

    size_t error_capacity);

/*
 * Owner lookup for a field/parameter type reference.
 *
 * Strips nullable and array modifiers, ignores primitives and inline
 * streams, and returns the module prefix that owns the resolved user
 * type. external_out is set true only when the owner prefix differs
 * from the current module.
 */
static bool resolve_type_owner(
    const emitter_ctx_t *ctx,
    const char *type_ref,
    char *owner_out,
    size_t owner_capacity,
    bool *external_out,
    char *error,
    size_t error_capacity);








static bool emit_decode_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);




static bool emit_decode_struct_fields_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);




static bool emit_decode_hierarchy_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *static_type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);




static bool emit_decode_array_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);




static bool emit_decode_struct_fields_recursive_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    size_t *bit_index,
    unsigned int depth,
    const char *runtime_depth,
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

static c_param_kind_t classify_param_kind_ctx(
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    bool *valid) {

    *valid = false;

    if (ctx == NULL || ctx->document == NULL ||
        param == NULL || param->type == NULL) {
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
        find_declared_type_ctx(ctx, base);

    if (declared == NULL) {
        return C_PARAM_KIND_SCALAR;
    }

    if (declared->has_tiered_int) {
        *valid = true;
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

    if (type_is_polymorphic_reference_ctx(ctx, declared)) {
        *valid = true;
        return C_PARAM_KIND_REF_PTR;
    }

    if (declared->is_abstract) {
        return C_PARAM_KIND_SCALAR;
    }

    *valid = true;
    return C_PARAM_KIND_ROOT_STRUCT;
}

static c_param_kind_t classify_param_kind(
    const adsl_document_t *document,
    const adsl_param_t *param,
    bool *valid) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return classify_param_kind_ctx(&ctx, param, valid);
}




static size_t method_nullable_param_count(
    const adsl_method_t *method) {

    if (method == NULL) {
        return 0u;
    }

    size_t count = 0u;

    for (size_t p = 0u;
         p < method->param_count;
         ++p) {

        const adsl_param_t *param =
            &method->params[p];

        if (param->type != NULL &&
            strchr(param->type, '?') != NULL) {

            count++;
        }
    }

    return count;
}


static size_t nullable_mask_width(
    size_t nullable_count) {

    if (nullable_count == 0u) {
        return 0u;
    }

    if (nullable_count <= 8u) {
        return 1u;
    }

    if (nullable_count <= 16u) {
        return 2u;
    }

    if (nullable_count <= 32u) {
        return 4u;
    }

    if (nullable_count <= 64u) {
        return 8u;
    }

    return 0u;
}





static bool stream_effective_prefix_ctx(
    const emitter_ctx_t *ctx,
    const adsl_stream_t *stream,
    const char *fallback_source_path,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity);


static bool emit_parameter_type_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    if (param == NULL || param->type == NULL) {
        set_error(error, error_capacity, "invalid C parameter metadata");
        return false;
    }

    const char *type = param->type;

    if (param->stream != NULL) {
        if (*type == '\0') {
            set_error(error, error_capacity,
                "stream parameter is missing a generated name");
            return false;
        }


        char stream_name[256];

        const adsl_type_t *stream_type =
            find_declared_type_ctx(ctx, type);


        char stream_abbr[32];
        stream_abbr[0] = '\0';



        if (stream_type != NULL &&
            !type_effective_prefix_ctx(
                ctx, stream_type, stream_abbr, sizeof(stream_abbr),
                error, error_capacity)) {
            return false;
        }

        if (stream_abbr[0] == '\0' &&
            !stream_effective_prefix_ctx(
                ctx, param->stream, NULL,
                stream_abbr, sizeof(stream_abbr),
                error, error_capacity)) {
            return false;
        }


        declared_c_name(
            stream_abbr, type, stream_name, sizeof(stream_name));


        fprintf(output, "const %s_t *", stream_name);
        return true;
    }

    if (strchr(type, '?') != NULL) {
        char cleaned[256];

        if (!strip_nullable_suffix(type, cleaned, sizeof(cleaned))) {
            set_error(error, error_capacity,
                "nullable API parameter type is too long");
            return false;
        }

        if (strchr(cleaned, '?') != NULL ||
            strchr(cleaned, '~') != NULL) {
            set_error(error, error_capacity,
                "malformed nullable API parameter type");
            return false;
        }

        adsl_param_t underlying = *param;
        underlying.type = cleaned;

        bool valid = false;
        c_param_kind_t kind =
            classify_param_kind_ctx(ctx, &underlying, &valid);

        if (!valid) {
            set_error(error, error_capacity,
                "unsupported nullable API parameter type");
            return false;
        }

        bool add_pointer =
            kind == C_PARAM_KIND_SCALAR ||
            kind == C_PARAM_KIND_DYNAMIC_VIEW;

        if (add_pointer) {
            fputs("const ", output);
        }

        if (!emit_parameter_type_ctx(
                output, ctx, &underlying,
                error, error_capacity)) {
            return false;
        }

        if (add_pointer) {
            fputs(" *", output);
        }

        return true;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            type, array_element, sizeof(array_element),
            &array_dynamic, &array_static_size)) {

        (void)array_element;
        (void)array_static_size;

        char alias[256];

        if (!array_alias_name_ctx(
                ctx, type, alias, sizeof(alias),
                error, error_capacity)) {
            if (error != NULL && error[0] == '\0') {
                set_error(error, error_capacity,
                    "generated array parameter type name is too long");
            }
            return false;
        }

        if (!array_dynamic) {
            fputs("const ", output);
        }

        fputs(alias, output);

        if (!array_dynamic) {
            fputs(" *", output);
        }
        return true;
    }

    char base[128];

    if (!scalar_type_name(type, base, sizeof(base))) {
        set_error(error, error_capacity,
            "milestone-2 emitter supports scalar or one-dimensional array parameters only");
        return false;
    }

    const char *c_type = primitive_c_type(base);

    if (c_type != NULL) {
        fputs(c_type, output);
        return true;
    }

    const adsl_type_t *declared_type =
        find_declared_type_ctx(ctx, base);

    if (declared_type == NULL) {
        set_error(error, error_capacity, "unknown C parameter type");
        return false;
    }

    char generated_name[256];

    if (!declared_type_c_name_ctx(
            ctx,
            declared_type,
            generated_name,
            sizeof(generated_name),
            error,
            error_capacity)) {
        return false;
    }


    if (declared_type->has_tiered_int) {
        fprintf(output, "%s_t", generated_name);
        return true;
    }

    if (declared_type->stream != NULL) {
        fprintf(output, "const %s_t *", generated_name);
        return true;
    }

    if (declared_type->enum_count > 0u) {
        fprintf(output, "%s_t", generated_name);
        return true;
    }

    if (type_is_polymorphic_reference_ctx(ctx, declared_type)) {
        fprintf(output, "const %s_ref_t *", generated_name);
        return true;
    }

    if (declared_type->is_abstract) {
        set_error(error, error_capacity,
            "abstract parameter type has no concrete hierarchy descendants");
        return false;
    }

    fprintf(output, "const %s_t *", generated_name);
    return true;
}

static bool emit_parameter_type(
    FILE *output,
    const adsl_document_t *document,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return emit_parameter_type_ctx(
        output, &ctx, param, error, error_capacity);
}



static const adsl_type_t *find_declared_type_doc(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    size_t name_length =
        strlen(name);


    const adsl_type_t *type_source =
        document->resolved_type_count > 0u
            ? document->resolved_types
            : document->types;

    size_t type_source_count =
        document->resolved_type_count > 0u
            ? document->resolved_type_count
            : document->type_count;

    for (size_t i = 0u;
         i < type_source_count;
         ++i) {


        const adsl_type_t *candidate =
            &type_source[i];

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



/*
 * Context-aware variant of find_declared_type_doc().
 *
 * Project mode (resolver != NULL) resolves the name through the shared
 * project resolver so that cross-document references match what
 * adsl_project_validate() accepted. Standalone mode preserves the
 * historical case-insensitive scan of the document's include closure.
 */
static const adsl_type_t *find_declared_type_ctx(
    const emitter_ctx_t *ctx,
    const char *name) {

    if (ctx == NULL || ctx->document == NULL || name == NULL) {
        return NULL;
    }

    if (ctx->resolver != NULL) {
        return adsl_project_resolver_type(ctx->resolver, name);
    }

    return find_declared_type_doc(ctx->document, name);
}


/*
 * Public-in-file wrapper preserved for standalone callers.
 *
 * Behaves exactly like the historical find_declared_type(): a
 * case-insensitive lookup in the document's include closure. Project
 * callers use find_declared_type_ctx() with a resolver instead.
 */
static const adsl_type_t *find_declared_type(
    const adsl_document_t *document,
    const char *name) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return find_declared_type_ctx(&ctx, name);
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
            "        uint32_t bits = aether_meta_java_float_bits(%s);\n"
            "        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, bits);\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            expression);

        return true;
    } else if (strcmp(base, "double") == 0) {
        fprintf(
            output,
            "    {\n"
            "        uint64_t bits = aether_meta_java_double_bits(%s);\n"
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




static bool emit_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const char *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity);







static bool emit_struct_definition_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    size_t type_index,
    uint8_t *states,
    array_alias_state_t *alias_state,
    char *error,
    size_t error_capacity);





static bool emit_serialize_param_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    if (param->stream != NULL) {
        if (param->type == NULL || *param->type == '\0') {
            set_error(error, error_capacity,
                "stream parameter is missing a generated name");
            return false;
        }

        fprintf(
            output,
            "    if (%s == NULL) return AETHER_ERR_ARGUMENT;\n"
            "    if (%s->length > %s->capacity) return AETHER_ERR_ARGUMENT;\n",
            param->name, param->name, param->name);

        char stream_expression[512];

        int stream_expression_size = snprintf(
            stream_expression, sizeof(stream_expression),
            "(*%s)", param->name);

        if (stream_expression_size < 0 ||
            (size_t)stream_expression_size >= sizeof(stream_expression)) {
            set_error(error, error_capacity,
                "generated stream parameter expression is too long");
            return false;
        }

        emit_bytes_view_expression(
            output, stream_expression, param->name);
        return true;
    }

    if (strchr(param->type, '?') != NULL) {
        char cleaned[256];

        if (!strip_nullable_suffix(
                param->type, cleaned, sizeof(cleaned))) {
            set_error(error, error_capacity,
                "nullable API parameter type is too long");
            return false;
        }

        adsl_param_t underlying = *param;

        if (strchr(cleaned, '?') != NULL ||
            strchr(cleaned, '~') != NULL) {
            set_error(error, error_capacity,
                "malformed nullable API parameter type");
            return false;
        }

        underlying.type = cleaned;

        bool valid = false;
        c_param_kind_t kind =
            classify_param_kind_ctx(ctx, &underlying, &valid);

        if (!valid) {
            set_error(error, error_capacity,
                "unsupported nullable API parameter type");
            return false;
        }

        char dereferenced_name[320];

        if (kind == C_PARAM_KIND_SCALAR ||
            kind == C_PARAM_KIND_DYNAMIC_VIEW) {

            int written = snprintf(
                dereferenced_name, sizeof(dereferenced_name),
                "(*%s)", param->name);

            if (written < 0 ||
                (size_t)written >= sizeof(dereferenced_name)) {
                set_error(error, error_capacity,
                    "generated nullable parameter expression is too long");
                return false;
            }

            underlying.name = dereferenced_name;
        }

        fprintf(output, "    if (%s != NULL) {\n", param->name);

        if (!emit_serialize_param_ctx(
                output, ctx, base_snake, &underlying,
                error, error_capacity)) {
            return false;
        }

        fputs("    }\n", output);
        return true;
    }

    char array_element[128];
    bool array_dynamic;
    size_t array_static_size;

    if (parse_array_type(
            param->type, array_element, sizeof(array_element),
            &array_dynamic, &array_static_size)) {
        (void)array_element;
        (void)array_static_size;

        return emit_array_expression_ctx(
            output, ctx, base_snake,
            param->type, param->name, param->name,
            !array_dynamic, 0u, NULL,
            error, error_capacity);
    }

    char base[128];

    if (!scalar_type_name(
            param->type, base, sizeof(base))) {
        set_error(error, error_capacity,
            "milestone-2 emitter supports scalar or one-dimensional array parameters only");
        return false;
    }

    const adsl_type_t *declared_type =
        find_declared_type_ctx(ctx, base);

    if (declared_type != NULL &&
        type_is_polymorphic_reference_ctx(ctx, declared_type)) {




        char type_name[256];
        char type_abbr[32];

        if (!type_effective_prefix_ctx(
                ctx, declared_type, type_abbr, sizeof(type_abbr),
                error, error_capacity)) {
            return false;
        }


        snake_name(
            declared_type->name, type_name, sizeof(type_name));



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

            type_abbr, type_name, param->name);

        return true;
    }

    bool struct_pointer =
        declared_type != NULL &&
        declared_type->enum_count == 0u &&
        !declared_type->has_tiered_int;

    return emit_value_expression_ctx(
        output, ctx, base_snake,
        param->type, param->name, param->name,
        struct_pointer, 0u, NULL,
        error, error_capacity);
}







static bool emit_remote_nullable_param_mask(
    FILE *output,
    const adsl_method_t *method,
    const char *param_prefix,
    char *error,
    size_t error_capacity) {

    size_t nullable_count =
        method_nullable_param_count(
            method);

    if (nullable_count == 0u) {
        return true;
    }

    size_t mask_width =
        nullable_mask_width(
            nullable_count);

    if (mask_width == 0u) {
        set_error(
            error,
            error_capacity,
            "API method has more than 64 nullable parameters");

        return false;
    }

    if (param_prefix == NULL) {
        param_prefix = "";
    }

    fputs(
        "    uint64_t _meta_nullable_mask = UINT64_C(0);\n",
        output);

    size_t bit_index = 0u;

    for (size_t p = 0u;
         p < method->param_count;
         ++p) {

        const adsl_param_t *param =
            &method->params[p];

        if (param->type == NULL ||
            strchr(param->type, '?') == NULL) {

            continue;
        }

        fprintf(
            output,
            "    if (%s%s == NULL) _meta_nullable_mask |= "
            "(UINT64_C(1) << %zuu);\n",
            param_prefix,
            param->name,
            bit_index);

        bit_index++;
    }

    fprintf(
        output,
        "    if (pos + %zuu > tx_capacity) return AETHER_ERR_OVERFLOW;\n",
        mask_width);

    for (size_t byte_index = 0u;
         byte_index < mask_width;
         ++byte_index) {

        fprintf(
            output,
            "    tx[pos++] = (uint8_t)"
            "((_meta_nullable_mask >> %zuu) & UINT64_C(0xff));\n",
            byte_index * 8u);
    }

    fputc(
        '\n',
        output);

    return true;
}



static bool emit_local_nullable_param_mask(
    FILE *output,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    size_t nullable_count =
        method_nullable_param_count(
            method);

    if (nullable_count == 0u) {
        return true;
    }

    size_t mask_width =
        nullable_mask_width(
            nullable_count);

    if (mask_width == 0u) {
        set_error(
            error,
            error_capacity,
            "API method has more than 64 nullable parameters");

        return false;
    }

    fputs(
        "        uint64_t _meta_nullable_mask = UINT64_C(0);\n",
        output);

    for (size_t byte_index = 0u;
         byte_index < mask_width;
         ++byte_index) {

        fprintf(
            output,
            "        {\n"
            "            uint8_t _meta_mask_byte = 0u;\n"
            "            if (!aether_meta_read_u8("
            "&ctx->reader, &_meta_mask_byte)) {\n"
            "                return AETHER_ERR_PROTOCOL;\n"
            "            }\n"
            "            _meta_nullable_mask |= "
            "((uint64_t)_meta_mask_byte) << %zuu;\n"
            "        }\n",
            byte_index * 8u);
    }

    fputc(
        '\n',
        output);

    return true;
}







static bool emit_stream_typedef(
    FILE *output,
    const char *abbr,
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
        abbr,
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


/*
 * Growing list of distinct module base_names whose generated headers
 * the current module must include. The generator itself is allowed to
 * use dynamic memory: it runs on the developer's host, not on the MCU,
 * and the number of project modules is not bounded by the language.
 *
 * Entries are module base_names without the "_types.h" suffix. The
 * caller is responsible for releasing the array with
 * external_modules_free().
 */
typedef struct {
    char **names;
    size_t count;
    size_t capacity;
} external_modules_t;

static void external_modules_free(
    external_modules_t *state);



static bool collect_external_modules_types(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,
    size_t error_capacity);

/*
 * API-only variant. The API header needs owners of parameter, return,
 * throws and return-field types; the types header only needs owners of
 * struct fields and parents. Keeping the two views apart avoids
 * pulling a foreign *_types.h into a module that never references the
 * foreign type in any of its own type definitions.
 */
static bool collect_external_modules_api(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,

    size_t error_capacity);

/*
 * Source-level collector: header deps plus hierarchy descendants whose
 * fields the impl of a local polymorphic type serializes. Kept separate
 * because the header only needs a forward typedef for hierarchy union
 * members, while the source needs the full struct definition.
 */

static bool collect_external_modules_types_source(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,
    size_t error_capacity);



static bool emit_external_includes(
    FILE *output,
    const external_modules_t *state);


static bool emit_types_header_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid types header context");
        return false;
    }

    if (output == NULL) {
        set_error(error, error_capacity, "no output stream for types header");
        return false;
    }



    array_alias_state_t alias_state = {0};

    (void)base_name;




    fprintf(
        output,
        "#ifndef AETHER_API_H\n"
        "#define AETHER_API_H\n\n"
        "#include \"aether_meta_runtime.h\"\n"
        "#include <stdbool.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n");



    /*
     * Project mode: pull in the *_types.h of every module whose
     * declarations are used by this module's own types or API
     * signatures. Standalone output remains a single self-contained
     * translation unit and does not reach this branch.
     */

    if (ctx->resolver != NULL) {

        external_modules_t external = {0};

        if (!collect_external_modules_types(
                ctx, &external, error, error_capacity)) {
            external_modules_free(&external);
            return false;
        }

        if (!emit_external_includes(output, &external)) {
            external_modules_free(&external);
            return false;
        }

        external_modules_free(&external);

        fputc('\n', output);
    }


    size_t type_count = 0u;

    const adsl_type_t *types = ctx_types(ctx, &type_count);

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (!type->has_tiered_int) {
            continue;
        }

        char type_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                type,
                type_name,
                sizeof(type_name),
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,
            "typedef uint64_t %s_t;\n"
            "extern const uint32_t %s_tiered_int_limits[%zu];\n\n",
            type_name,
            type_name,
            type->tiered_int_limit_count);
    }

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->stream == NULL) {
            continue;
        }

        char stream_prefix[32];



        if (!stream_effective_prefix_ctx(
                ctx,
                type->stream,
                type->source_path,
                stream_prefix,
                sizeof(stream_prefix),
                error,
                error_capacity)) {
            return false;
        }



        if (!emit_stream_typedef(
                output,
                stream_prefix,
                type->name,
                error,
                error_capacity)) {


            return false;
        }
    }

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                const adsl_param_t *param = &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }


                char stream_prefix[32];


                if (!stream_effective_prefix_ctx(
                        ctx,
                        param->stream,
                        method->source_path,
                        stream_prefix,
                        sizeof(stream_prefix),
                        error,
                        error_capacity)) {
                    return false;
                }


                if (!emit_stream_typedef(
                        output,
                        stream_prefix,
                        param->type,
                        error,
                        error_capacity)) {

                    return false;
                }

            }
        }
    }

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->enum_count == 0u) {
            continue;
        }

        char type_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                type,
                type_name,
                sizeof(type_name),
                error,
                error_capacity)) {
            return false;
        }


        fprintf(output, "typedef enum {\n");

        for (size_t e = 0u; e < type->enum_count; ++e) {

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

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->enum_count > 0u ||
            type->is_abstract ||
            type->stream != NULL ||
            type->has_tiered_int) {

            continue;
        }

        char type_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                type,
                type_name,
                sizeof(type_name),
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,
            "typedef struct %s_t %s_t;\n",
            type_name,
            type_name);
    }

    if (type_count > 0u) {
        fputc('\n', output);
    }

    for (size_t i = 0u; i < type_count; ++i) {

        if (!emit_hierarchy_ref_ctx(
                output,
                ctx,
                &types[i],
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        for (size_t f = 0u; f < type->field_count; ++f) {

            if (!array_alias_requires_complete_element_ctx(
                    ctx,
                    type->fields[f].type) &&
                !emit_array_alias_ctx(
                    output,
                    ctx,
                    &alias_state,
                    type->fields[f].type,
                    error,
                    error_capacity)) {

                return false;
            }
        }
    }

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                if (!array_alias_requires_complete_element_ctx(
                        ctx,
                        method->params[p].type) &&
                    !emit_array_alias_ctx(
                        output,
                        ctx,
                        &alias_state,
                        method->params[p].type,
                        error,
                        error_capacity)) {

                    return false;
                }
            }

            if ((!array_alias_requires_complete_element_ctx(
                        ctx,
                        method->returns_type) &&
                  !emit_array_alias_ctx(
                      output,
                      ctx,
                      &alias_state,
                      method->returns_type,
                      error,
                      error_capacity)) ||
                (!array_alias_requires_complete_element_ctx(
                        ctx,
                        method->throws_type) &&
                  !emit_array_alias_ctx(
                      output,
                      ctx,
                      &alias_state,
                      method->throws_type,
                      error,
                      error_capacity))) {

                return false;
            }
        }
    }

    size_t state_count = type_count == 0u ? 1u : type_count;

    uint8_t states[state_count];

    memset(states, 0, state_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->enum_count > 0u ||
            type->is_abstract ||
            type->stream != NULL) {

            continue;
        }

        if (!emit_struct_definition_ctx(
                output,
                ctx,
                i,
                states,
                &alias_state,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                if (array_alias_requires_complete_element_ctx(
                        ctx,
                        method->params[p].type) &&
                    !emit_array_alias_ctx(
                        output,
                        ctx,
                        &alias_state,
                        method->params[p].type,
                        error,
                        error_capacity)) {

                    return false;
                }
            }

            if ((array_alias_requires_complete_element_ctx(
                        ctx,
                        method->returns_type) &&
                  !emit_array_alias_ctx(
                      output,
                      ctx,
                      &alias_state,
                      method->returns_type,
                      error,
                      error_capacity)) ||
                (array_alias_requires_complete_element_ctx(
                        ctx,
                        method->throws_type) &&
                  !emit_array_alias_ctx(
                      output,
                      ctx,
                      &alias_state,
                      method->throws_type,
                      error,
                      error_capacity))) {

                return false;
            }
        }
    }


    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        bool hierarchy =
            type_is_polymorphic_reference_ctx(ctx, type);

        bool concrete_struct =
            type->enum_count == 0u &&
            !type->is_abstract &&
            type->stream == NULL &&
            !type->has_tiered_int &&
            !hierarchy;

        if (!hierarchy && !concrete_struct) {
            continue;
        }

        char type_suffix[256];
        char type_name[256];
        char serializer_base[32];

        if (!type_effective_prefix_ctx(
                ctx,
                type,
                serializer_base,
                sizeof(serializer_base),
                error,
                error_capacity)) {
            return false;
        }

        declared_c_name(
            serializer_base,
            type->name,
            type_name,
            sizeof(type_name));

        declared_c_name(
            "",
            type->name,
            type_suffix,
            sizeof(type_suffix));



        emit_type_origin(
            output,
            type);


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
                type_suffix,
                type_name,
                serializer_base,
                type_suffix,
                type_name);


            /*
             * Project mode adds a cross-module entry point that lets a
             * caller in another generated module thread its current
             * recursion depth into this module. Standalone does not
             * need it: all recursive calls stay inside one translation
             * unit and use the static *_impl directly.
             */
            if (ctx->resolver != NULL) {
                fprintf(
                    output,
                    "aether_status_t %s_serialize_%s_with_depth(\n"
                    "    uint8_t *tx,\n"
                    "    size_t tx_capacity,\n"
                    "    size_t *position,\n"
                    "    const %s_ref_t *value,\n"
                    "    unsigned int recursion_depth);\n\n"
                    "aether_status_t %s_deserialize_%s_with_depth(\n"
                    "    aether_meta_read_context_t *ctx,\n"
                    "    %s_ref_t *value,\n"
                    "    unsigned int recursion_depth);\n\n",
                    serializer_base,
                    type_suffix,
                    type_name,
                    serializer_base,
                    type_suffix,
                    type_name);

            }
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
                type_suffix,
                type_name,
                serializer_base,
                type_suffix,
                type_name);


            if (ctx->resolver != NULL) {
                fprintf(
                    output,
                    "aether_status_t %s_serialize_%s_with_depth(\n"
                    "    uint8_t *tx,\n"
                    "    size_t tx_capacity,\n"
                    "    size_t *position,\n"
                    "    const %s_t *value,\n"
                    "    unsigned int recursion_depth);\n\n"
                    "aether_status_t %s_deserialize_%s_with_depth(\n"
                    "    aether_meta_read_context_t *ctx,\n"
                    "    %s_t *value,\n"
                    "    unsigned int recursion_depth);\n\n",
                    serializer_base,
                    type_suffix,
                    type_name,
                    serializer_base,
                    type_suffix,
                    type_name);

            }
        }

    }


    return true;
}





static bool emit_types_header(
    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated types header");
        return false;
    }

    emitter_ctx_t ctx = emitter_ctx_standalone(document);

    if (!emit_types_header_ctx(
            output, &ctx, base_name, error, error_capacity)) {
        fclose(output);
        return false;
    }

    fprintf(
        output,
        "\n#ifdef __cplusplus\n"
        "}\n"
        "#endif\n"
        "#endif\n");

    if (ferror(output)) {
        set_error(error, error_capacity, "cannot write generated types header");
        fclose(output);
        return false;
    }

    fclose(output);
    return true;
}








static bool emit_types_source_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid types source context");
        return false;
    }

    if (output == NULL) {
        set_error(error, error_capacity, "no output stream for types source");
        return false;
    }

    (void)base_name;






    /*
     * Project mode: the .c accesses fields, sizeof and serializers of
     * every external type touched by its impl bodies. The header only
     * pulls in by-value and array-element dependencies; hierarchy
     * descendants are forward-declared there and must be given their
     * complete definition here.
     */
    if (ctx->resolver != NULL) {

        external_modules_t source_external = {0};


        if (!collect_external_modules_types_source(
                ctx, &source_external, error, error_capacity)) {



            external_modules_free(&source_external);
            return false;
        }

        if (!emit_external_includes(output, &source_external)) {
            external_modules_free(&source_external);
            return false;
        }

        if (source_external.count > 0u) {
            fputc('\n', output);
        }

        external_modules_free(&source_external);
    }

    size_t type_count = 0u;
    const adsl_type_t *types = ctx_types(ctx, &type_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (!type->has_tiered_int) {
            continue;
        }

        char type_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                type,
                type_name,
                sizeof(type_name),
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,
            "const uint32_t %s_tiered_int_limits[%zu] = {\n",
            type_name,
            type->tiered_int_limit_count);

        for (size_t l = 0u; l < type->tiered_int_limit_count; ++l) {

            fprintf(
                output,
                "    UINT32_C(%u)%s\n",
                (unsigned int)type->tiered_int_limits[l],
                l + 1u == type->tiered_int_limit_count
                    ? ""
                    : ",");
        }

        fputs("};\n\n", output);
    }

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        bool hierarchy =
            type_is_polymorphic_reference_ctx(ctx, type);

        bool concrete_struct =
            type->enum_count == 0u &&
            !type->is_abstract &&
            type->stream == NULL &&
            !type->has_tiered_int &&
            !hierarchy;

        if (!hierarchy && !concrete_struct) {
            continue;
        }


        /*
         * Per-type serialize/deserialize symbol prefix. Shadows the
         * function-scope base_snake so each type uses the file that
         * declares it, not the output module name.
         */
        char base_snake[32];
        char type_suffix[256];
        char type_name[256];

        if (!type_effective_prefix_ctx(
                ctx,
                type,
                base_snake,
                sizeof(base_snake),
                error,
                error_capacity)) {
            return false;
        }

        declared_c_name(
            base_snake,
            type->name,
            type_name,
            sizeof(type_name));

        declared_c_name(
            "",
            type->name,
            type_suffix,
            sizeof(type_suffix));



        if (hierarchy) {
            fprintf(
                output,
                "static aether_status_t %s_serialize_%s_impl(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_ref_t *value,\n"
                "    unsigned int recursion_depth);\n\n"
                "static aether_status_t %s_deserialize_%s_impl(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_ref_t *value,\n"
                "    unsigned int recursion_depth);\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                type_name);

        } else {
            fprintf(
                output,
                "static aether_status_t %s_serialize_%s_impl(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_t *value,\n"
                "    unsigned int recursion_depth);\n\n"
                "static aether_status_t %s_deserialize_%s_impl(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_t *value,\n"
                "    unsigned int recursion_depth);\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                type_name);
        }
    }

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        bool hierarchy =
            type_is_polymorphic_reference_ctx(ctx, type);

        bool concrete_struct =
            type->enum_count == 0u &&
            !type->is_abstract &&
            type->stream == NULL &&
            !type->has_tiered_int &&
            !hierarchy;

        if (!hierarchy && !concrete_struct) {
            continue;
        }

        char type_name[256];
        char type_suffix[256];

        char base_snake[32];

        if (!type_effective_prefix_ctx(
                ctx,
                type,
                base_snake,
                sizeof(base_snake),
                error,
                error_capacity)) {
            return false;
        }

        declared_c_name(
            base_snake,
            type->name,
            type_name,
            sizeof(type_name));

        declared_c_name(
            "",
            type->name,
            type_suffix,
            sizeof(type_suffix));



        if (hierarchy) {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_ref_t *value) {\n\n"
                "    return %s_serialize_%s_impl(\n"
                "        tx,\n"
                "        tx_capacity,\n"
                "        position,\n"
                "        value,\n"
                "        0u);\n"
                "}\n\n"
                "static aether_status_t %s_serialize_%s_impl(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_ref_t *value,\n"
                "    unsigned int recursion_depth) {\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                base_snake,
                type_suffix,
                type_name);


        } else {
            fprintf(
                output,
                "aether_status_t %s_serialize_%s(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_t *value) {\n\n"
                "    return %s_serialize_%s_impl(\n"
                "        tx,\n"
                "        tx_capacity,\n"
                "        position,\n"
                "        value,\n"
                "        0u);\n"
                "}\n\n"
                "static aether_status_t %s_serialize_%s_impl(\n"
                "    uint8_t *tx,\n"
                "    size_t tx_capacity,\n"
                "    size_t *position,\n"
                "    const %s_t *value,\n"
                "    unsigned int recursion_depth) {\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                base_snake,
                type_suffix,
                type_name);


        }

        fputs(
            "    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {\n"
            "        return AETHER_ERR_OVERFLOW;\n"
            "    }\n\n"
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
                emit_hierarchy_expression_ctx(
                    output,
                    ctx,
                    base_snake,
                    type,
                    "value",
                    "value",
                    true,
                    0u,
                    "recursion_depth",
                    error,
                    error_capacity);

        } else {
            emitted =
                emit_struct_value_expression_ctx(
                    output,
                    ctx,
                    base_snake,
                    type,
                    "value",
                    "value",
                    true,
                    0u,
                    "recursion_depth",
                    error,
                    error_capacity);
        }


        if (!emitted) {
            return false;
        }

        fputs(
            "\n"
            "    *position = pos;\n"
            "    return AETHER_OK;\n"
            "}\n\n",

            output);

        /*
         * Project mode: cross-module entry point that lets a caller in
         * another generated module thread its current recursion depth
         * into this module. Standalone does not need it: all recursive
         * calls stay inside one translation unit and use the static
         * *_impl directly.
         */
        if (ctx->resolver != NULL) {
            if (hierarchy) {
                fprintf(
                    output,
                    "aether_status_t %s_serialize_%s_with_depth(\n"
                    "    uint8_t *tx,\n"
                    "    size_t tx_capacity,\n"
                    "    size_t *position,\n"
                    "    const %s_ref_t *value,\n"
                    "    unsigned int recursion_depth) {\n\n"
                    "    return %s_serialize_%s_impl(\n"
                    "        tx, tx_capacity, position, value,\n"
                    "        recursion_depth);\n"
                    "}\n\n",
                    base_snake, type_name, type_name,
                    base_snake, type_name);
            } else {
                fprintf(
                    output,
                    "aether_status_t %s_serialize_%s_with_depth(\n"
                    "    uint8_t *tx,\n"
                    "    size_t tx_capacity,\n"
                    "    size_t *position,\n"
                    "    const %s_t *value,\n"
                    "    unsigned int recursion_depth) {\n\n"
                    "    return %s_serialize_%s_impl(\n"
                    "        tx, tx_capacity, position, value,\n"
                    "        recursion_depth);\n"
                    "}\n\n",
                    base_snake, type_suffix, type_name,
                    base_snake, type_suffix);

            }
        }


        if (hierarchy) {
            fprintf(
                output,
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_ref_t *value) {\n\n"
                "    return %s_deserialize_%s_impl(\n"
                "        ctx,\n"
                "        value,\n"
                "        0u);\n"
                "}\n\n"
                "static aether_status_t %s_deserialize_%s_impl(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_ref_t *value,\n"
                "    unsigned int recursion_depth) {\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                base_snake,
                type_suffix,
                type_name);


        } else {
            fprintf(
                output,
                "aether_status_t %s_deserialize_%s(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_t *value) {\n\n"
                "    return %s_deserialize_%s_impl(\n"
                "        ctx,\n"
                "        value,\n"
                "        0u);\n"
                "}\n\n"
                "static aether_status_t %s_deserialize_%s_impl(\n"
                "    aether_meta_read_context_t *ctx,\n"
                "    %s_t *value,\n"
                "    unsigned int recursion_depth) {\n\n",
                base_snake,
                type_suffix,
                type_name,
                base_snake,
                type_suffix,
                base_snake,
                type_suffix,
                type_name);




        }

        fputs(
            "    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {\n"
            "        return AETHER_ERR_OVERFLOW;\n"
            "    }\n\n"
            "    if (ctx == NULL || value == NULL) {\n"
            "        return AETHER_ERR_ARGUMENT;\n"
            "    }\n\n"
            "    memset(value, 0, sizeof(*value));\n\n",
            output);

        bool decoded;


        if (hierarchy) {
            decoded =
                emit_decode_hierarchy_expression_ctx(
                    output,
                    ctx,
                    type,
                    "(*value)",
                    base_snake,
                    0u,
                    "recursion_depth",
                    error,
                    error_capacity);

        } else {

            decoded =
                emit_decode_struct_fields_ctx(
                    output,
                    ctx,
                    type,
                    "value",
                    base_snake,
                    0u,
                    "recursion_depth",
                    error,
                    error_capacity);

        }

        if (!decoded) {
            return false;
        }

        fputs(
            "\n"
            "    return AETHER_OK;\n"
            "}\n\n",

            output);

        /*
         * Project mode: cross-module entry point for deserialization.
         * Mirrors the serializer's *_with_depth. Standalone keeps its
         * single public wrapper plus static *_impl and does not
         * generate this symbol.
         */
        if (ctx->resolver != NULL) {
            if (hierarchy) {
                fprintf(
                    output,
                    "aether_status_t %s_deserialize_%s_with_depth(\n"
                    "    aether_meta_read_context_t *ctx,\n"
                    "    %s_ref_t *value,\n"
                    "    unsigned int recursion_depth) {\n\n"
                    "    return %s_deserialize_%s_impl(\n"
                    "        ctx, value, recursion_depth);\n"
                    "}\n\n",
                    base_snake, type_suffix, type_name,
                    base_snake, type_suffix);

            } else {
                fprintf(
                    output,
                    "aether_status_t %s_deserialize_%s_with_depth(\n"
                    "    aether_meta_read_context_t *ctx,\n"
                    "    %s_t *value,\n"
                    "    unsigned int recursion_depth) {\n\n"
                    "    return %s_deserialize_%s_impl(\n"
                    "        ctx, value, recursion_depth);\n"
                    "}\n\n",
                    base_snake, type_suffix, type_name,
                    base_snake, type_suffix);

            }
        }

    }

    if (ferror(output)) {
        set_error(
            error,
            error_capacity,
            "cannot write generated types source");

        return false;
    }

    return true;
}



static bool emit_types_source(
    const adsl_document_t *document,
    const char *path,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        set_error(error, error_capacity, "cannot create generated types source");
        return false;
    }

    char base_snake[256];

    snake_name(base_name, base_snake, sizeof(base_snake));

    fprintf(
        output,
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n",
        base_snake);

    emitter_ctx_t ctx = emitter_ctx_standalone(document);

    if (!emit_types_source_ctx(
            output, &ctx, base_name, error, error_capacity)) {
        fclose(output);
        return false;
    }

    fclose(output);
    return true;
}






static const adsl_api_t *find_api_definition_doc(
    const adsl_document_t *document,
    const char *name) {

    if (document == NULL ||
        name == NULL) {

        return NULL;
    }

    for (size_t i = 0u;
         i < document->resolved_api_count;
         ++i) {

        if (strcmp(
                document->resolved_apis[i].name,
                name) == 0) {

            return &document->resolved_apis[i];
        }
    }

    return NULL;
}


/*
 * Context-aware variant of find_api_definition_doc().
 *
 * Project mode (resolver != NULL) resolves the API through the shared
 * project resolver so that cross-document API inheritance matches what
 * adsl_project_validate() accepted. Standalone mode preserves the
 * historical include-closure lookup.
 */
static const adsl_api_t *find_api_definition_ctx(
    const emitter_ctx_t *ctx,
    const char *name) {

    if (ctx == NULL || ctx->document == NULL || name == NULL) {
        return NULL;
    }

    if (ctx->resolver != NULL) {
        return adsl_project_resolver_api(ctx->resolver, name);
    }

    return find_api_definition_doc(ctx->document, name);
}


/*
 * Public-in-file wrapper preserved for standalone callers.
 */
static const adsl_api_t *find_api_definition(
    const adsl_document_t *document,
    const char *name) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return find_api_definition_ctx(&ctx, name);
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

static bool emitter_collect_effective_doc(
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
 * Context-aware variant of emitter_collect_effective_doc().
 *
 * Project mode uses the project-scoped effective method collector so
 * that parent API lookup is global and the resulting method order and
 * wire ids match adsl_project_validate(). Standalone mode preserves the
 * per-document collector.
 */
static bool emitter_collect_effective_ctx(
    const emitter_ctx_t *ctx,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "selected API not found");
        return false;
    }

    if (ctx->resolver != NULL) {
        return adsl_project_resolver_collect_effective(
            ctx->resolver,
            api,
            out_methods,
            out_count,
            error,
            error_capacity);
    }

    return emitter_collect_effective_doc(
        ctx->document,
        api,
        out_methods,
        out_count,
        error,
        error_capacity);
}


/*
 * Public-in-file wrapper preserved for standalone callers.
 */
static bool emitter_collect_effective(
    const adsl_document_t *document,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,
    size_t error_capacity) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return emitter_collect_effective_ctx(
        &ctx,
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




static bool method_has_collapsible_params(
    const adsl_method_t *method);





static bool emit_stream_builder_declarations_for_api_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *stream_abbr,
    const char *stream_name,
    const char *api_reference,
    char *error,
    size_t error_capacity) {

    const adsl_api_t *api =
        find_api_definition_ctx(ctx, api_reference);

    if (api == NULL) {
        set_error(error, error_capacity,
            "stream api does not resolve to a declared API");
        return false;
    }


    char stream_c_name[256];
    char api_name[256];
    char api_abbr[32];

    declared_c_name(stream_abbr, stream_name, stream_c_name, sizeof(stream_c_name));
    snake_name(api->name, api_name, sizeof(api_name));

    if (!api_effective_prefix_ctx(
            ctx, api, api_abbr, sizeof(api_abbr),
            error, error_capacity)) {
        return false;
    }


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective_ctx(
            ctx, api, &effective, &effective_count,
            error, error_capacity)) {
        return false;
    }

    for (size_t m = 0u; m < effective_count; ++m) {

        const adsl_method_t *method = effective[m].method;

        bool has_response =
            method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL;

        char method_name[256];
        snake_name(method->name, method_name, sizeof(method_name));

        bool collapsible =
            method_has_collapsible_params(method);

        emit_method_origin(output, effective[m].owner, method);

        /*
         * The builder entry point is namespaced by the stream *type* name
         * (e.g. csa_login_stream) so that the module prefix (csa_) still
         * leads the symbol, while the builder cannot collide with the plain
         * API function that shares {api_prefix}_{api_name}_{method_name}.
         */
        fprintf(
            output,
            "aether_status_t %s_%s_%s(\n"
            "    %s_t *builder_stream",
            stream_c_name, api_name, method_name, stream_c_name);

        if (collapsible) {


            fprintf(output, ",\n    %s_%s_%s_context_t *collapse",
                api_abbr, api_name, method_name);


        }

        if (has_response) {
            fputs(",\n    uint32_t request_id", output);
        }

        for (size_t p = 0u; p < method->param_count; ++p) {

            fputs(",\n    ", output);

            if (!emit_parameter_type_ctx(
                    output, ctx, &method->params[p],
                    error, error_capacity)) {
                free(effective);
                return false;
            }

            fprintf(output, " %s", method->params[p].name);
        }

        fputs(");\n\n", output);

        if (collapsible) {
            emit_method_origin(output, effective[m].owner, method);

            fprintf(
                output,
                "aether_status_t %s_%s_%s_flush(\n"
                "    %s_t *builder_stream,\n"
                "    %s_%s_%s_context_t *collapse);\n\n",

                stream_c_name, api_name, method_name,
                stream_c_name, api_abbr, api_name, method_name);


        }
    }

    free(effective);
    return !ferror(output);
}





/*
 * Effective C symbol prefix for a stream builder. Streams carry their
 * physical origin only indirectly: through the API that owns them. When
 * the stream type itself has no source_path (merged/unified document) we
 * fall back to the owning API's prefix, and finally to the legacy module
 * prefix for standalone output.
 */
static bool stream_effective_prefix_ctx(
    const emitter_ctx_t *ctx,
    const adsl_stream_t *stream,
    const char *fallback_source_path,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (out == NULL || capacity == 0u) {
        set_error(error, error_capacity, "invalid stream prefix buffer");
        return false;
    }

    out[0] = '\0';

    /*
     * Prefer the prefix of the API that owns this stream. Streams defined
     * inline inside a parameter carry no source_path of their own, and the
     * enclosing method's source_path can belong to a different document
     * when the API is inherited across files. The owning API is always the
     * physical declaration site, so its prefix is authoritative.
     */
    if (stream != NULL) {

        if (stream->api != NULL && *stream->api != '\0') {

            const adsl_api_t *api =
                find_api_definition_ctx(ctx, stream->api);

            if (api != NULL &&
                api_effective_prefix_ctx(
                    ctx, api, out, capacity,
                    error, error_capacity)) {
                if (out[0] != '\0') {
                    return true;
                }
            }
        }

        for (size_t i = 0u; i < stream->api_count; ++i) {

            if (stream->apis[i].api == NULL ||
                *stream->apis[i].api == '\0') {
                continue;
            }

            const adsl_api_t *api =
                find_api_definition_ctx(ctx, stream->apis[i].api);

            if (api != NULL &&
                api_effective_prefix_ctx(
                    ctx, api, out, capacity,
                    error, error_capacity)) {
                if (out[0] != '\0') {
                    return true;
                }
            }
        }
    }

    if (fallback_source_path != NULL && fallback_source_path[0] != '\0') {
        abbreviate_from_path(fallback_source_path, out, capacity);
    }

    if (out[0] != '\0') {
        return true;
    }

    if (ctx != NULL && ctx->module_abbr[0] != '\0') {
        (void)snprintf(out, capacity, "%s", ctx->module_abbr);
        return out[0] != '\0';
    }

    set_error(
        error, error_capacity,
        "stream builder has no module prefix");

    return false;
}



static bool emit_stream_builder_declarations_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *stream_abbr,
    const char *stream_name,
    const adsl_stream_t *stream,
    char *error,
    size_t error_capacity) {


    if (stream == NULL) {
        return true;
    }

    if (stream->api != NULL &&
        *stream->api != '\0' &&

!emit_stream_builder_declarations_for_api_ctx(
            output, ctx, stream_abbr, stream_name, stream->api,
            error, error_capacity)) {

        return false;
    }

    for (size_t i = 0u; i < stream->api_count; ++i) {

        if (stream->apis[i].api == NULL ||
            *stream->apis[i].api == '\0') {
            continue;
        }


if (!emit_stream_builder_declarations_for_api_ctx(
                output, ctx, stream_abbr, stream_name, stream->apis[i].api,
                error, error_capacity)) {

            return false;
        }
    }

    return true;
}



static bool emit_all_stream_builder_declarations_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    char *error,
    size_t error_capacity) {

    size_t type_count = 0u;
    const adsl_type_t *types = ctx_types(ctx, &type_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->stream == NULL) {
            continue;
        }

        char stream_abbr[32];



        if (!stream_effective_prefix_ctx(
                ctx, type->stream, type->source_path,
                stream_abbr, sizeof(stream_abbr),
                error, error_capacity)) {
            return false;
        }



        if (!emit_stream_builder_declarations_ctx(
                output, ctx, stream_abbr, type->name, type->stream,
                error, error_capacity)) {

            return false;
        }
    }

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                const adsl_param_t *param = &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }


                char stream_abbr[32];
                stream_abbr[0] = '\0';
                bool have_abbr = false;


                const adsl_type_t *stream_type =
                    find_declared_type_ctx(ctx, param->type);



                if (stream_type != NULL) {
                    have_abbr = type_effective_prefix_ctx(
                        ctx, stream_type, stream_abbr, sizeof(stream_abbr),
                        error, error_capacity);
                    if (!have_abbr) {
                        return false;
                    }
                }

                if (stream_abbr[0] == '\0') {

                    if (!stream_effective_prefix_ctx(
                            ctx, param->stream, method->source_path,
                            stream_abbr,
                            sizeof(stream_abbr),
                            error, error_capacity)) {
                        return false;
                    }

                }

                if (!emit_stream_builder_declarations_ctx(
                        output, ctx, stream_abbr, param->type, param->stream,
                        error, error_capacity)) {

                    return false;
                }
            }
        }
    }

    return true;
}




static bool emit_stream_builder_definitions_for_api_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *stream_abbr,
    const char *stream_name,
    const char *api_reference,
    char *error,
    size_t error_capacity) {

    const adsl_api_t *api =
        find_api_definition_ctx(ctx, api_reference);

    if (api == NULL) {
        set_error(error, error_capacity,
            "stream api does not resolve to a declared API");
        return false;
    }


    char stream_c_name[256];
    char api_name[256];
    char api_abbr[32];

    declared_c_name(stream_abbr, stream_name, stream_c_name, sizeof(stream_c_name));
    snake_name(api->name, api_name, sizeof(api_name));

    if (!api_effective_prefix_ctx(
            ctx, api, api_abbr, sizeof(api_abbr),
            error, error_capacity)) {
        return false;
    }


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!emitter_collect_effective_ctx(
            ctx, api, &effective, &effective_count,
            error, error_capacity)) {
        return false;
    }

    for (size_t m = 0u; m < effective_count; ++m) {

        const adsl_method_t *method = effective[m].method;

        bool has_response =
            method->returns_type != NULL ||
            method->return_field_count > 0u ||
            method->throws_type != NULL;

        char method_name[256];
        snake_name(method->name, method_name, sizeof(method_name));

        bool collapsible =
            method_has_collapsible_params(method);

        emit_method_origin(output, effective[m].owner, method);

        if (collapsible) {
            /*
             * The builder entry point is namespaced by the stream type name
             * (csa_login_stream) so that the module prefix still leads the
             * symbol and the builder cannot collide with the plain API
             * function that shares {api_prefix}_{api_name}_{method_name}.
             */
            fprintf(
                output,
                "aether_status_t %s_%s_%s(\n"
                "    %s_t *builder_stream,\n"
                "    %s_%s_%s_context_t *collapse",

                stream_c_name, api_name, method_name,
                stream_c_name, api_abbr, api_name, method_name);


            for (size_t p = 0u; p < method->param_count; ++p) {

                fputs(",\n    ", output);

                if (!emit_parameter_type_ctx(
                        output, ctx, &method->params[p],
                        error, error_capacity)) {
                    free(effective);
                    return false;
                }

                fprintf(output, " %s", method->params[p].name);
            }

            fprintf(
                output,
                ") {\n\n"
                "    if (builder_stream == NULL ||\n"
                "        collapse == NULL ||\n"
                "        builder_stream->length > builder_stream->capacity ||\n"
                "        (builder_stream->capacity != 0u && "
                "builder_stream->data == NULL)) {\n"
                "        return AETHER_ERR_ARGUMENT;\n"
                "    }\n\n"

                "    %s_%s_remote_t remote = {0};\n\n"
                "    return %s_%s_%s(\n"
                "        &remote,\n"
                "        collapse",
                stream_abbr, api_name, stream_abbr, api_name, method_name);

            for (size_t p = 0u; p < method->param_count; ++p) {
                fprintf(output, ",\n        %s",
                    method->params[p].name);
            }

            fputs(");\n}\n\n", output);

            emit_method_origin(output, effective[m].owner, method);

            fprintf(
                output,
                "aether_status_t %s_%s_%s_flush(\n"
                "    %s_t *builder_stream,\n"

                "    %s_%s_%s_context_t *collapse) {\n\n"
                "    if (builder_stream == NULL ||\n"
                "        collapse == NULL ||\n"
                "        builder_stream->length > builder_stream->capacity ||\n"
                "        (builder_stream->capacity != 0u && "
                "builder_stream->data == NULL)) {\n"
                "        return AETHER_ERR_ARGUMENT;\n"
                "    }\n\n"
                "    if (collapse->count == 0u &&\n"
                "        (collapse->queue == NULL || collapse->queue->count == 0u)) {\n"
                "        return AETHER_OK;\n"
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
                "    %s_%s_remote_t remote = {\n"
                "        &commit,\n"
                "        %s_stream_commit,\n"
                "        builder_stream->data + builder_stream->length,\n"
                "        remaining\n"
                "    };\n\n"
                "    return %s_%s_%s_flush(\n"
                "        &remote,\n"
                "        collapse);\n"
                "}\n\n",


                stream_c_name, api_name, method_name,
                stream_c_name,
                api_abbr, api_name, method_name,
                stream_abbr,
                stream_abbr, api_name,
                stream_abbr,
                api_abbr, api_name, method_name);


            continue;
        }

        fprintf(
            output,
            "aether_status_t %s_%s_%s(\n"
            "    %s_t *builder_stream",
            stream_c_name, api_name, method_name, stream_c_name);

        if (has_response) {
            fputs(",\n    uint32_t request_id", output);
        }

        for (size_t p = 0u; p < method->param_count; ++p) {

            fputs(",\n    ", output);

            if (!emit_parameter_type_ctx(
                    output, ctx, &method->params[p],
                    error, error_capacity)) {
                free(effective);
                return false;
            }

            fprintf(output, " %s", method->params[p].name);
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
            "    %s_%s_remote_t remote = {\n"
            "        &commit,\n"
            "        %s_stream_commit,\n"
            "        builder_stream->data + builder_stream->length,\n"
            "        remaining\n"
            "    };\n\n"
            "    return %s_%s_%s(\n"
            "        &remote",
            stream_abbr, stream_abbr, api_name, stream_abbr, stream_abbr, api_name, method_name);

        if (has_response) {
            fputs(",\n        request_id", output);
        }

        for (size_t p = 0u; p < method->param_count; ++p) {
            fprintf(output, ",\n        %s", method->params[p].name);
        }

        fputs(");\n}\n\n", output);
    }

    free(effective);
    return !ferror(output);
}




static bool emit_stream_builder_definitions_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *stream_abbr,
    const char *stream_name,
    const adsl_stream_t *stream,
    char *error,
    size_t error_capacity) {


    if (stream == NULL) {
        return true;
    }

    if (stream->api != NULL &&
        *stream->api != '\0' &&

!emit_stream_builder_definitions_for_api_ctx(
            output, ctx, stream_abbr, stream_name, stream->api,
            error, error_capacity)) {

        return false;
    }

    for (size_t i = 0u; i < stream->api_count; ++i) {

        if (stream->apis[i].api == NULL ||
            *stream->apis[i].api == '\0') {
            continue;
        }


if (!emit_stream_builder_definitions_for_api_ctx(
                output, ctx, stream_abbr, stream_name,
                stream->apis[i].api,
                error, error_capacity)) {

            return false;
        }
    }

    return true;
}




/*
 * Emit the per-module stream commit helper once for every distinct prefix.
 * The helper struct and its static function are shared by all stream builders
 * that belong to the same ADSL document, and must therefore live under that
 * document's prefix (csa_, pa_, ...). A missing prefix would collide with
 * unrelated outputs and break compilation.
 */
static bool emit_stream_commit_once_ctx(
    FILE *output,
    const char *abbr,
    char emitted[32][32],
    size_t *emitted_count,
    char *error,
    size_t error_capacity) {

    if (abbr == NULL || abbr[0] == '\0') {
        set_error(
            error, error_capacity,
            "stream builder has no module prefix");
        return false;
    }

    if (emitted == NULL || emitted_count == NULL) {
        set_error(error, error_capacity, "invalid stream commit state");
        return false;
    }

    for (size_t i = 0u; i < *emitted_count; ++i) {
        if (strcmp(emitted[i], abbr) == 0) {
            return true;
        }
    }

    if (*emitted_count >= 32u) {
        set_error(error, error_capacity, "too many stream prefixes");
        return false;
    }

    (void)snprintf(
        emitted[*emitted_count], sizeof(emitted[0]), "%s", abbr);

    *emitted_count += 1u;

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
        abbr, abbr, abbr);

    return true;
}



static bool emit_all_stream_builder_definitions_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    char *error,
    size_t error_capacity) {

    /*
     * The stream commit helper is per-module: the struct type and the
     * static function are emitted once for each distinct stream prefix
     * that actually owns stream builders. Collect prefixes on the fly so
     * that csa_/pa_/etc. each get their own commit helper.
     */
    char emitted_abbrs[32][32];
    size_t emitted_count = 0u;

    size_t type_count = 0u;
    const adsl_type_t *types = ctx_types(ctx, &type_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->stream == NULL) {
            continue;
        }

        char stream_abbr[32];



        if (!stream_effective_prefix_ctx(
                ctx, type->stream, type->source_path,
                stream_abbr, sizeof(stream_abbr),
                error, error_capacity)) {
            return false;
        }



        if (!emit_stream_commit_once_ctx(
                output, stream_abbr,
                emitted_abbrs, &emitted_count,
                error, error_capacity)) {
            return false;
        }

        if (!emit_stream_builder_definitions_ctx(
                output, ctx, stream_abbr,
                type->name, type->stream,
                error, error_capacity)) {

            return false;
        }
    }

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                const adsl_param_t *param = &method->params[p];

                if (param->stream == NULL) {
                    continue;
                }

                char stream_abbr[32];

                const adsl_type_t *stream_type =
                    find_declared_type_ctx(ctx, param->type);

                if (stream_type != NULL) {
                    if (!type_effective_prefix_ctx(
                            ctx, stream_type, stream_abbr,
                            sizeof(stream_abbr),
                            error, error_capacity)) {
                        return false;
                    }
                } else {
                    stream_abbr[0] = '\0';
                }

                if (stream_abbr[0] == '\0') {

                    if (!stream_effective_prefix_ctx(
                            ctx, param->stream, method->source_path,
                            stream_abbr,
                            sizeof(stream_abbr),
                            error, error_capacity)) {
                        return false;
                    }

                }

                if (!emit_stream_commit_once_ctx(
                        output, stream_abbr,
                        emitted_abbrs, &emitted_count,
                        error, error_capacity)) {
                    return false;
                }

                if (!emit_stream_builder_definitions_ctx(
                        output, ctx, stream_abbr,
                        param->type, param->stream,
                        error, error_capacity)) {

                    return false;
                }
            }
        }
    }

    return true;
}



static bool param_is_collapsible(
    const adsl_param_t *param) {

    if (param == NULL ||
        param->type == NULL) {

        return false;
    }

    size_t length =
        strlen(param->type);

    return length > 0u &&
           param->type[length - 1u] == '~';
}


static bool method_has_collapsible_params(
    const adsl_method_t *method) {

    if (method == NULL) {
        return false;
    }

    for (size_t p = 0u;
         p < method->param_count;
         ++p) {

        if (param_is_collapsible(
                &method->params[p])) {

            return true;
        }
    }

    return false;
}


/*
 * Emit comparison of one METHOD collapse key component.
 *
 * The generated table deliberately compares logical key values rather than
 * pointer addresses. String/URI keys therefore compare byte content, UUIDs
 * compare their canonical two-word value, and integer-like scalar keys compare
 * directly. More complex key shapes are rejected until their Java equality
 * semantics can be reproduced exactly.
 */

static bool emit_collapsible_key_match_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    if (param == NULL ||
        param->type == NULL ||
        param_is_collapsible(param)) {
        return true;
    }

    if (param->stream != NULL) {
        set_error(error, error_capacity,
            "collapsible METHOD stream key parameters are not supported");
        return false;
    }

    bool nullable = strchr(param->type, '?') != NULL;

    char cleaned[256];

    if (!strip_nullable_suffix(
            param->type, cleaned, sizeof(cleaned))) {
        set_error(error, error_capacity,
            "collapsible METHOD key type is too long");
        return false;
    }

    char array_element[128];
    bool array_dynamic = false;
    size_t array_static_size = 0u;

    if (parse_array_type(
            cleaned, array_element, sizeof(array_element),
            &array_dynamic, &array_static_size)) {
        (void)array_dynamic;
        (void)array_static_size;
        set_error(error, error_capacity,
            "collapsible METHOD array key parameters are not supported");
        return false;
    }

    char base[128];

    if (!scalar_type_name(cleaned, base, sizeof(base))) {
        set_error(error, error_capacity,
            "unsupported collapsible METHOD key type");
        return false;
    }

    const adsl_type_t *declared =
        find_declared_type_ctx(ctx, base);

    bool is_bytes = strcmp(base, "string") == 0;
    bool is_uuid = strcmp(base, "uuid") == 0;
    bool is_float = strcmp(base, "float") == 0;
    bool is_double = strcmp(base, "double") == 0;
    bool is_scalar =
        primitive_c_type(base) != NULL &&
        !is_bytes && !is_uuid && !is_float && !is_double;

    if (declared != NULL &&
        (declared->enum_count > 0u || declared->has_tiered_int)) {
        is_scalar = true;
    }

    if (is_float || is_double) {
        const char *bits_fn = is_float
            ? "aether_meta_java_float_bits"
            : "aether_meta_java_double_bits";

        if (nullable) {
            fprintf(
                output,
                "        if (_match && "
                "((_entry->%s == NULL) != (%s == NULL))) {\n"
                "            _match = false;\n"
                "        } else if (_match && %s != NULL &&\n"
                "                   %s(*_entry->%s) != %s(*%s)) {\n"
                "            _match = false;\n"
                "        }\n",
                param->name, param->name, param->name,
                bits_fn, param->name, bits_fn, param->name);
        } else {
            fprintf(
                output,
                "        if (_match && %s(_entry->%s) != %s(%s)) {\n"
                "            _match = false;\n"
                "        }\n",
                bits_fn, param->name, bits_fn, param->name);
        }
        return !ferror(output);
    }

    if (!is_scalar && !is_bytes && !is_uuid) {
        set_error(error, error_capacity,
            "collapsible METHOD key type has no exact Java Objects.equals mapping");
        return false;
    }

    if (is_scalar) {
        if (nullable) {
            fprintf(
                output,
                "        if (_match && "
                "((_entry->%s == NULL) != (%s == NULL))) {\n"
                "            _match = false;\n"
                "        } else if (_match && %s != NULL && "
                "*_entry->%s != *%s) {\n"
                "            _match = false;\n"
                "        }\n",
                param->name, param->name, param->name,
                param->name, param->name);
        } else {
            fprintf(
                output,
                "        if (_match && _entry->%s != %s) {\n"
                "            _match = false;\n"
                "        }\n",
                param->name, param->name);
        }
        return true;
    }

    if (is_uuid) {
        if (nullable) {
            fprintf(
                output,
                "        if (_match && "
                "((_entry->%s == NULL) != (%s == NULL))) {\n"
                "            _match = false;\n"
                "        } else if (_match && %s != NULL &&\n"
                "                   (_entry->%s->msb != %s->msb ||\n"
                "                    _entry->%s->lsb != %s->lsb)) {\n"
                "            _match = false;\n"
                "        }\n",
                param->name, param->name, param->name,
                param->name, param->name,
                param->name, param->name);
        } else {
            fprintf(
                output,
                "        if (_match &&\n"
                "            (_entry->%s.msb != %s.msb ||\n"
                "             _entry->%s.lsb != %s.lsb)) {\n"
                "            _match = false;\n"
                "        }\n",
                param->name, param->name,
                param->name, param->name);
        }
        return true;
    }

    if (nullable) {
        fprintf(
            output,
            "        if (_match && "
            "((_entry->%s == NULL) != (%s == NULL))) {\n"
            "            _match = false;\n"
            "        } else if (_match && %s != NULL &&\n"
            "                   (_entry->%s->length != %s->length ||\n"
            "                    (%s->length != 0u &&\n"
            "                     (_entry->%s->data == NULL ||\n"
            "                      %s->data == NULL ||\n"
            "                      memcmp(_entry->%s->data,\n"
            "                             %s->data,\n"
            "                             %s->length) != 0)))) {\n"
            "            _match = false;\n"
            "        }\n",
            param->name, param->name, param->name,
            param->name, param->name, param->name,
            param->name, param->name, param->name,
            param->name, param->name);
    } else {
        fprintf(
            output,
            "        if (_match &&\n"
            "            (_entry->%s.length != %s.length ||\n"
            "             (%s.length != 0u &&\n"
            "              (_entry->%s.data == NULL ||\n"
            "               %s.data == NULL ||\n"
            "               memcmp(_entry->%s.data,\n"
            "                      %s.data,\n"
            "                      %s.length) != 0)))) {\n"
            "            _match = false;\n"
            "        }\n",
            param->name, param->name, param->name,
            param->name, param->name, param->name,
            param->name, param->name);
    }

    return true;
}





static bool collapsible_param_snapshots_nullable_value_ctx(
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    bool *snapshot,
    char *error,
    size_t error_capacity) {

    if (snapshot == NULL) {
        set_error(error, error_capacity,
            "invalid collapsible METHOD snapshot metadata");
        return false;
    }

    *snapshot = false;

    if (param == NULL ||
        param->type == NULL ||
        strchr(param->type, '?') == NULL) {
        return true;
    }

    char cleaned[256];

    if (!strip_nullable_suffix(
            param->type, cleaned, sizeof(cleaned))) {
        set_error(error, error_capacity,
            "collapsible METHOD nullable parameter type is too long");
        return false;
    }

    adsl_param_t underlying = *param;
    underlying.type = cleaned;

    bool valid = false;
    c_param_kind_t kind =
        classify_param_kind_ctx(ctx, &underlying, &valid);

    if (!valid) {
        set_error(error, error_capacity,
            "unsupported collapsible METHOD nullable parameter type");
        return false;
    }

    /*
     * Nullable scalar and dynamic-view arguments add one pointer solely to
     * represent NULL in the public C ABI. Retaining that caller pointer would
     * violate Java's value snapshot semantics, so generated entries keep the
     * underlying value/descriptor and point back to their own storage.
     *
     * Struct/static-array/stream/ref arguments are already pointer-shaped
     * without nullable and remain borrowed by the established METHOD lifetime
     * contract.
     */
    *snapshot =
        kind == C_PARAM_KIND_SCALAR ||
        kind == C_PARAM_KIND_DYNAMIC_VIEW;

    return true;
}




static bool emit_collapsible_entry_field_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    bool snapshot = false;

    if (!collapsible_param_snapshots_nullable_value_ctx(
            ctx, param, &snapshot, error, error_capacity)) {
        return false;
    }

    if (snapshot) {
        char cleaned[256];

        if (!strip_nullable_suffix(
                param->type, cleaned, sizeof(cleaned))) {
            set_error(error, error_capacity,
                "collapsible METHOD nullable parameter type is too long");
            return false;
        }

        adsl_param_t underlying = *param;
        underlying.type = cleaned;

        fputs("    ", output);

        if (!emit_parameter_type_ctx(
                output, ctx, &underlying, error, error_capacity)) {
            return false;
        }

        fprintf(output, " %s_value;\n", param->name);
    }

    fputs("    ", output);

    if (!emit_parameter_type_ctx(
            output, ctx, param, error, error_capacity)) {
        return false;
    }

    fprintf(output, " %s;\n", param->name);

    return !ferror(output);
}




static bool emit_collapsible_entry_store_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_param_t *param,
    char *error,
    size_t error_capacity) {

    bool snapshot = false;

    if (!collapsible_param_snapshots_nullable_value_ctx(
            ctx, param, &snapshot, error, error_capacity)) {
        return false;
    }

    if (!snapshot) {
        fprintf(output,
            "        _entry->%s = %s;\n",
            param->name, param->name);
        return !ferror(output);
    }

    fprintf(
        output,
        "        if (%s != NULL) {\n"
        "            _entry->%s_value = *%s;\n"
        "            _entry->%s = &_entry->%s_value;\n"
        "        } else {\n"
        "            _entry->%s = NULL;\n"
        "        }\n",
        param->name,
        param->name, param->name,
        param->name, param->name,
        param->name);

    return !ferror(output);
}





static bool emit_collapsible_method_declarations_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *api_abbr,
    const char *api_name,
    const char *method_name,
    const adsl_method_t *method,
    char *error,
    size_t error_capacity) {

    fprintf(output, "typedef struct {\n");

    for (size_t p = 0u; p < method->param_count; ++p) {
        if (!emit_collapsible_entry_field_ctx(
                output, ctx, &method->params[p],
                error, error_capacity)) {
            return false;
        }
    }

    fprintf(
        output,
        "} %s_%s_%s_entry_t;\n\n"
        "typedef struct {\n"
        "    %s_%s_%s_entry_t *entries;\n"
        "    size_t capacity;\n"
        "    size_t count;\n"
        "    aether_meta_collapse_queue_t *queue;\n"
        "} %s_%s_%s_context_t;\n\n"
        "aether_status_t %s_%s_%s(\n"
        "    %s_%s_remote_t *api,\n"
        "    %s_%s_%s_context_t *collapse",
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name,
        api_abbr, api_name, method_name);

    for (size_t p = 0u; p < method->param_count; ++p) {

        fputs(",\n    ", output);

        if (!emit_parameter_type_ctx(
                output, ctx, &method->params[p],
                error, error_capacity)) {
            return false;
        }

        fprintf(output, " %s", method->params[p].name);
    }

    fprintf(
        output,
        ");\n\n"
        "aether_status_t %s_%s_%s_flush(\n"
        "    %s_%s_remote_t *api,\n"
        "    %s_%s_%s_context_t *collapse);\n\n"
        "void %s_%s_%s_reset(\n"
        "    %s_%s_%s_context_t *collapse);\n\n",
        api_abbr, api_name, method_name,
        api_abbr, api_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name);

    return !ferror(output);
}




static bool emit_collapsible_method_source_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *api_abbr,
    const char *api_name,
    const char *method_name,
    const adsl_method_t *method,
    uint8_t command_id,
    char *error,
    size_t error_capacity) {

    fprintf(
        output,

        "static aether_status_t %s_%s_%s_collapse_serialize(\n"
        "    void *ctx,\n"
        "    uint8_t *tx,\n"
        "    size_t tx_capacity,\n"
        "    size_t *position);\n\n"
        "static void %s_%s_%s_collapse_commit(void *ctx);\n\n"
        "aether_status_t %s_%s_%s(\n"
        "    %s_%s_remote_t *api,\n"
        "    %s_%s_%s_context_t *collapse",

        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name,
        api_abbr, api_name, method_name);

    for (size_t p = 0u; p < method->param_count; ++p) {

        fputs(",\n    ", output);

        if (!emit_parameter_type_ctx(
                output, ctx, &method->params[p],
                error, error_capacity)) {
            return false;
        }

        fprintf(output, " %s", method->params[p].name);
    }

    fprintf(
        output,
        ") {\n\n"
        "    if (api == NULL ||\n"
        "        collapse == NULL ||\n"
        "        collapse->count > collapse->capacity ||\n"
        "        (collapse->capacity != 0u && collapse->entries == NULL)) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    bool _was_empty = collapse->count == 0u;\n\n"
        "    size_t _index = collapse->count;\n\n"
        "    for (size_t i = 0u; i < collapse->count; ++i) {\n"
        "        %s_%s_%s_entry_t *_entry = &collapse->entries[i];\n"
        "        bool _match = true;\n",
        api_abbr, api_name, method_name);

    for (size_t p = 0u; p < method->param_count; ++p) {

        const adsl_param_t *param = &method->params[p];

        if (param_is_collapsible(param)) {
            continue;
        }

        if (!emit_collapsible_key_match_ctx(
                output, ctx, param, error, error_capacity)) {
            return false;
        }
    }

    fputs(
        "\n"
        "        if (_match) {\n"
        "            _index = i;\n"
        "            break;\n"
        "        }\n"
        "    }\n\n"
        "    ",
        output);

    fprintf(
        output,
        "%s_%s_%s_entry_t *_entry = NULL;\n\n"
        "    if (_index == collapse->count) {\n"
        "        if (collapse->count >= collapse->capacity) {\n"
        "            return AETHER_ERR_OVERFLOW;\n"
        "        }\n\n"
        "        _entry = &collapse->entries[collapse->count];\n",
        api_abbr, api_name, method_name);

    for (size_t p = 0u; p < method->param_count; ++p) {

        const adsl_param_t *param = &method->params[p];

        if (param_is_collapsible(param)) {
            continue;
        }

        if (!emit_collapsible_entry_store_ctx(
                output, ctx, param, error, error_capacity)) {
            return false;
        }
    }

    fputs(
        "        collapse->count++;\n"
        "    } else {\n"
        "        _entry = &collapse->entries[_index];\n"
        "    }\n\n",
        output);

    for (size_t p = 0u; p < method->param_count; ++p) {

        const adsl_param_t *param = &method->params[p];

        if (!param_is_collapsible(param)) {
            continue;
        }

        if (!emit_collapsible_entry_store_ctx(
                output, ctx, param, error, error_capacity)) {
            return false;
        }
    }

    fprintf(
        output,
        "\n"
        "    if (_was_empty && collapse->queue != NULL) {\n"
        "        aether_status_t queue_status =\n"
        "            aether_meta_collapse_queue_register(\n"
        "                collapse->queue,\n"
        "                collapse,\n"
        "                %s_%s_%s_collapse_serialize,\n"
        "                %s_%s_%s_collapse_commit);\n"
        "        if (queue_status != AETHER_OK) {\n"
        "            collapse->count = 0u;\n"
        "            return queue_status;\n"
        "        }\n"
        "    }\n\n"
        "    return AETHER_OK;\n"
        "}\n\n"
        "static aether_status_t %s_%s_%s_collapse_serialize(\n"
        "    void *ctx,\n"
        "    uint8_t *tx,\n"
        "    size_t tx_capacity,\n"
        "    size_t *position) {\n\n"
        "    %s_%s_%s_context_t *collapse = ctx;\n\n"
        "    if (collapse == NULL ||\n"
        "        position == NULL ||\n"
        "        collapse->count > collapse->capacity ||\n"
        "        (collapse->capacity != 0u && collapse->entries == NULL)) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    if (collapse->count == 0u) {\n"
        "        return AETHER_OK;\n"
        "    }\n\n"
        "    if (tx == NULL) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    size_t pos = *position;\n\n"
        "    for (size_t i = 0u; i < collapse->count; ++i) {\n"
        "        %s_%s_%s_entry_t *_entry = &collapse->entries[i];\n\n"
        "        if (pos >= tx_capacity) {\n"
        "            return AETHER_ERR_OVERFLOW;\n"
        "        }\n\n"
        "        tx[pos++] = %uu;\n",
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        (unsigned int)command_id);

    if (!emit_remote_nullable_param_mask(
            output, method, "_entry->", error, error_capacity)) {
        return false;
    }

    for (size_t p = 0u; p < method->param_count; ++p) {

        char expression[384];

        int written = snprintf(
            expression, sizeof(expression), "_entry->%s",
            method->params[p].name);

        if (written < 0 || (size_t)written >= sizeof(expression)) {
            set_error(error, error_capacity,
                "collapsible METHOD parameter expression is too long");
            return false;
        }

        adsl_param_t stored = method->params[p];
        stored.name = expression;

        if (!emit_serialize_param_ctx(
                output, ctx, api_abbr, &stored,
                error, error_capacity)) {
            return false;
        }
    }

    fprintf(
        output,
        "    }\n\n"
        "    *position = pos;\n"
        "    return AETHER_OK;\n"
        "}\n\n"
        "static void %s_%s_%s_collapse_commit(void *ctx) {\n\n"
        "    %s_%s_%s_context_t *collapse = ctx;\n"
        "    if (collapse != NULL) {\n"
        "        collapse->count = 0u;\n"
        "    }\n"
        "}\n\n"
        "aether_status_t %s_%s_%s_flush(\n"
        "    %s_%s_remote_t *api,\n"
        "    %s_%s_%s_context_t *collapse) {\n\n"
        "    if (api == NULL ||\n"
        "        collapse == NULL ||\n"
        "        collapse->count > collapse->capacity ||\n"
        "        (collapse->capacity != 0u && collapse->entries == NULL)) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    if (collapse->queue != NULL) {\n"
        "        return aether_meta_collapse_queue_flush(\n"
        "            collapse->queue,\n"
        "            api->send_ctx,\n"
        "            api->send,\n"
        "            api->tx,\n"
        "            api->tx_capacity);\n"
        "    }\n\n"
        "    if (collapse->count == 0u) {\n"
        "        return AETHER_OK;\n"
        "    }\n\n"
        "    if (api->send == NULL || api->tx == NULL) {\n"
        "        return AETHER_ERR_ARGUMENT;\n"
        "    }\n\n"
        "    size_t pos = 0u;\n"
        "    aether_status_t status =\n"
        "        %s_%s_%s_collapse_serialize(\n"
        "            collapse,\n"
        "            api->tx,\n"
        "            api->tx_capacity,\n"
        "            &pos);\n\n"
        "    if (status != AETHER_OK) {\n"
        "        return status;\n"
        "    }\n\n"
        "    status = api->send(\n"
        "        api->send_ctx,\n"
        "        api->tx,\n"
        "        pos);\n\n"
        "    if (status == AETHER_OK) {\n"
        "        %s_%s_%s_collapse_commit(collapse);\n"
        "    }\n\n"
        "    return status;\n"
        "}\n\n"
        "void %s_%s_%s_reset(\n"
        "    %s_%s_%s_context_t *collapse) {\n\n"
        "    if (collapse != NULL) {\n"
        "        if (collapse->queue != NULL) {\n"
        "            aether_meta_collapse_queue_cancel(\n"
        "                collapse->queue,\n"
        "                collapse);\n"
        "        }\n"
        "        collapse->count = 0u;\n"
        "    }\n"
        "}\n\n",
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name,
        api_abbr, api_name, method_name);

    return !ferror(output);
}







static bool emit_api_header_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid API header context");
        return false;
    }

    if (output == NULL) {
        set_error(error, error_capacity, "no output stream for API header");
        return false;
    }

    (void)base_name;

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);


    /*
     * The transport send-fn typedef is emitted once per distinct module
     * prefix. Every API's remote_t refers to its own module's send fn, so a
     * single global typedef keyed off the first API would break any output
     * where more than one document contributes APIs (project/unified mode).
     */

    char send_abbrs[32][32];
    size_t send_abbr_count = 0u;

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        if (selected_api_name != NULL &&
            strcmp(api->name, selected_api_name) != 0) {
            continue;
        }

        char send_abbr[32];

        if (!api_effective_prefix_ctx(
                ctx, api, send_abbr, sizeof(send_abbr),
                error, error_capacity)) {
            return false;
        }

        bool already = false;

        for (size_t s = 0u; s < send_abbr_count; ++s) {
            if (strcmp(send_abbrs[s], send_abbr) == 0) {
                already = true;
                break;
            }
        }

        if (already) {
            continue;
        }

        if (send_abbr_count >= 32u) {
            set_error(error, error_capacity, "too many API prefixes");
            return false;
        }

        (void)snprintf(
            send_abbrs[send_abbr_count],
            sizeof(send_abbrs[0]),
            "%s",
            send_abbr);

        send_abbr_count += 1u;

        fprintf(
            output,
            "typedef aether_status_t (*%s_send_fn)(\n"
            "    void *ctx,\n"
            "    const uint8_t *data,\n"
            "    size_t length);\n\n",
            send_abbr);
    }


    /*
     * Project mode: API signatures may reference types owned by other
     * modules. Bring in only the owners of parameter, return, throws
     * and return-field types — not the full set of the current
     * module's own type dependencies. Standalone output remains one
     * self-contained translation unit and skips this branch.
     */

    if (ctx->resolver != NULL) {

        external_modules_t external_api = {0};

        if (!collect_external_modules_api(
                ctx, &external_api, error, error_capacity)) {
            external_modules_free(&external_api);
            return false;
        }

        if (!emit_external_includes(output, &external_api)) {
            external_modules_free(&external_api);
            return false;
        }

        if (external_api.count > 0u) {
            fputc('\n', output);
        }

        external_modules_free(&external_api);
    }

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        if (selected_api_name != NULL &&
            strcmp(api->name, selected_api_name) != 0) {
            continue;
        }

        char api_abbr[32];

        if (!api_effective_prefix_ctx(
                ctx, api, api_abbr, sizeof(api_abbr),
                error, error_capacity)) {
            return false;
        }

        char api_name[256];
        snake_name(api->name, api_name, sizeof(api_name));

        fprintf(
            output,
            "typedef struct {\n"
            "    void *send_ctx;\n"
            "    %s_send_fn send;\n"
            "    uint8_t *tx;\n"
            "    size_t tx_capacity;\n"

            "} %s_%s_remote_t;\n\n",
            api_abbr, api_abbr, api_name);

        adsl_effective_method_t *effective = NULL;
        size_t effective_count = 0u;

        if (!emitter_collect_effective_ctx(
                ctx, api, &effective, &effective_count,
                error, error_capacity)) {
            return false;
        }

        for (size_t m = 0u; m < effective_count; ++m) {

            const adsl_method_t *method = effective[m].method;

            if (selected_method_name != NULL &&
                strcmp(method->name, selected_method_name) != 0) {
                continue;
            }

            bool has_response =
                method->returns_type != NULL ||
                method->return_field_count > 0u ||
                method->throws_type != NULL;

            char method_name[256];
            snake_name(method->name, method_name, sizeof(method_name));

            emit_method_origin(output, effective[m].owner, method);

            if (method_has_collapsible_params(method)) {
                if (!emit_collapsible_method_declarations_ctx(
                        output, ctx, api_abbr, api_name, method_name, method,
                        error, error_capacity)) {
                    free(effective);
                    return false;
                }
                continue;
            }

            fprintf(
                output,

                "aether_status_t %s_%s_%s(\n"
                "    %s_%s_remote_t *api",
                api_abbr, api_name, method_name,
                api_abbr, api_name);

            if (has_response) {
                fputs(",\n    uint32_t request_id", output);
            }

            for (size_t p = 0u; p < method->param_count; ++p) {

                fputs(",\n    ", output);

                if (!emit_parameter_type_ctx(
                        output, ctx, &method->params[p],
                        error, error_capacity)) {
                    free(effective);
                    return false;
                }

                fprintf(output, " %s", method->params[p].name);
            }

            fputs(");\n\n", output);
        }

        free(effective);
    }

    if (selected_api_name == NULL &&
        selected_method_name == NULL &&
        !emit_all_stream_builder_declarations_ctx(
            output, ctx, error, error_capacity)) {
        return false;
    }

    if (ferror(output)) {
        set_error(error, error_capacity, "cannot write generated API header");
        return false;
    }

    return true;
}










static bool emit_api_source_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_name,
    const char *selected_api_name,
    const char *selected_method_name,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid API source context");
        return false;
    }

    if (output == NULL) {
        set_error(error, error_capacity, "no output stream for API source");
        return false;
    }

    (void)base_name;

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);


    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        if (selected_api_name != NULL &&
            strcmp(api->name, selected_api_name) != 0) {
            continue;
        }

        char api_abbr[32];

        if (!api_effective_prefix_ctx(
                ctx, api, api_abbr, sizeof(api_abbr),
                error, error_capacity)) {
            return false;
        }

        char api_name[256];
        snake_name(api->name, api_name, sizeof(api_name));

        adsl_effective_method_t *effective = NULL;
        size_t effective_count = 0u;

        if (!emitter_collect_effective_ctx(
                ctx, api, &effective, &effective_count,
                error, error_capacity)) {
            return false;
        }

        for (size_t m = 0u; m < effective_count; ++m) {

            const adsl_method_t *method = effective[m].method;

            if (selected_method_name != NULL &&
                strcmp(method->name, selected_method_name) != 0) {
                continue;
            }

            bool has_response =
                method->returns_type != NULL ||
                method->return_field_count > 0u ||
                method->throws_type != NULL;

            char method_name[256];
            snake_name(method->name, method_name, sizeof(method_name));

            emit_method_origin(output, effective[m].owner, method);

            if (method_has_collapsible_params(method)) {
                if (!emit_collapsible_method_source_ctx(
                        output, ctx, api_abbr, api_name, method_name,
                        method, effective[m].id, error, error_capacity)) {
                    free(effective);
                    return false;
                }
                continue;
            }

            fprintf(
                output,

                "aether_status_t %s_%s_%s(\n"
                "    %s_%s_remote_t *api",
                api_abbr, api_name, method_name,
                api_abbr, api_name);

            if (has_response) {
                fputs(",\n    uint32_t request_id", output);
            }

            for (size_t p = 0u; p < method->param_count; ++p) {

                fputs(",\n    ", output);

                if (!emit_parameter_type_ctx(
                        output, ctx, &method->params[p],
                        error, error_capacity)) {
                    free(effective);
                    return false;
                }

                fprintf(output, " %s", method->params[p].name);
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
                has_response ? 5u : 1u,
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

            if (!emit_remote_nullable_param_mask(
                    output, method, "", error, error_capacity)) {
                free(effective);
                return false;
            }

            for (size_t p = 0u; p < method->param_count; ++p) {
                if (!emit_serialize_param_ctx(
                        output, ctx, api_abbr, &method->params[p],
                        error, error_capacity)) {
                    free(effective);
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
        !emit_all_stream_builder_definitions_ctx(
            output, ctx, error, error_capacity)) {
        return false;
    }

    if (ferror(output)) {
        set_error(error, error_capacity, "cannot write generated API source");
        return false;
    }

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


    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    snake_name(base_name, ctx.base_snake, sizeof(ctx.base_snake));


    if (selected_api_name != NULL) {
        const adsl_api_t *selected_api =
            find_api_definition_ctx(&ctx, selected_api_name);

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

            if (!emitter_collect_effective_ctx(
                    &ctx,
                    selected_api,
                    &effective,
                    &effective_count,
                    error,
                    error_capacity)) {

                return false;
            }

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




    (void)base_name;


    char header_path[1024];
    char source_path[1024];

    if (!build_output_path(header_path, sizeof(header_path), output_directory, "aether_api", ".h") ||
        !build_output_path(source_path, sizeof(source_path), output_directory, "aether_api", ".c")) {

        set_error(
            error,
            error_capacity,
            "generated output path is too long");
        return false;
    }

    /*
     * The output directory is created if missing so that callers do not
     * need to create it themselves (the CLI passes a user-supplied path
     * and library callers may point anywhere).
     */
    (void)mkdir(output_directory, 0777);



    FILE *header = fopen(header_path, "wb");

    if (header == NULL) {
        set_error(error, error_capacity, "cannot create generated header");
        return false;
    }

    if (!emit_types_header_ctx(
            header,
            &ctx,
            base_name,
            error,
            error_capacity)) {

        fclose(header);
        return false;
    }

    if (!emit_api_header_ctx(
            header,
            &ctx,
            base_name,
            selected_api_name,
            selected_method_name,
            error,
            error_capacity)) {

        fclose(header);
        return false;
    }

    fprintf(
        header,
        "\n#ifdef __cplusplus\n"
        "}\n"
        "#endif\n"
        "#endif\n");
    fclose(header);

    FILE *source = fopen(source_path, "wb");

    if (source == NULL) {
        set_error(error, error_capacity, "cannot create generated source");
        return false;
    }


    fprintf(
        source,
        "#include \"aether_api.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n");


    if (!emit_types_source_ctx(
            source,
            &ctx,
            base_name,
            error,
            error_capacity)) {

        fclose(source);
        return false;
    }

    if (!emit_api_source_ctx(
            source,
            &ctx,
            base_name,
            selected_api_name,
            selected_method_name,
            error,
            error_capacity)) {

        fclose(source);
        return false;
    }

    fclose(source);
    return true;
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




/*
 * Project-mode safe variant of snake_name().
 *
 * snake_name() is a void function that silently truncates on overflow,
 * so a truncated name is indistinguishable from a legitimate short one.
 * For the project emitter we require that the source name can be
 * normalized without truncation and that the result is non-empty;
 * otherwise two distinct base_names could collide or a generated
 * filename could be silently wrong. The maximum expansion is two bytes
 * per source byte (an uppercase letter emits '_' plus its lowercase),
 * plus the terminating NUL.
 */
static bool safe_snake_name(
    const char *source,
    char *target,
    size_t capacity) {

    if (source == NULL || target == NULL || capacity == 0u) {
        return false;
    }

    if (source[0] == '\0') {
        target[0] = '\0';
        return false;
    }

    size_t source_length = strlen(source);

    if (source_length == 0u ||
        source_length > (capacity - 1u) / 2u) {
        return false;
    }

    snake_name(source, target, capacity);

    if (target[0] == '\0') {
        return false;
    }

    return true;

}

/*
 * Resolve the module prefix (base_snake) that owns the generated C
 * symbols of the given ADSL type declaration.
 *
 * Standalone: every declaration in the include closure is emitted by
 * the single generated module, so the owner prefix is the current
 * base_snake.
 *
 * Project mode: the resolver records the root document whose closure
 * contains the declaration; the emission plan maps that root document
 * to a module base_name. Failure to determine the owner is a hard
 * error: two modules that reference the same ADSL type must agree on
 * who owns "<owner>_serialize_<type>" and friends.
 *
 * The C type name itself (for example "item_t") is not module-prefixed
 * and is deliberately not affected by this helper.
 */
bool type_owner_base_snake(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || type == NULL || out == NULL || capacity == 0u) {
        set_error(
            error,
            error_capacity,
            "invalid type owner lookup arguments");
        return false;
    }

    if (ctx->resolver == NULL) {


        /*
         * Standalone / unified: one translation unit. The declaration's
         * owner prefix is, by definition, its own effective symbol prefix.
         * This delegates to the single authority so the owner name and the
         * generated symbols can never diverge. In legacy single-file mode
         * every type resolves to the same file, so this is equivalent to
         * the historical module_abbr behavior.
         */
        char decl_prefix[32];

        if (!type_effective_prefix_ctx(
                ctx, type, decl_prefix, sizeof(decl_prefix),
                error, error_capacity)) {
            return false;
        }

        size_t length = strlen(decl_prefix);

        if (length + 1u > capacity) {
            set_error(
                error,
                error_capacity,
                "type owner prefix does not fit");
            return false;
        }

        memcpy(out, decl_prefix, length + 1u);

        return true;

    }



    if (ctx->plan == NULL) {
        set_error(
            error,
            error_capacity,
            "project emitter context has no emission plan");
        return false;
    }

    const char *owner_source =
        adsl_project_resolver_type_source(ctx->resolver, type);

    if (owner_source == NULL) {
        char message[512];
        (void)snprintf(
            message,
            sizeof(message),
            "type '%s' has no owner in the project resolver",
            type->name != NULL ? type->name : "<unnamed>");
        set_error(error, error_capacity, message);
        return false;
    }

    const char *owner_base_name = NULL;

    for (size_t m = 0u; m < ctx->plan->module_count; ++m) {
        const char *sp = ctx->plan->modules[m].source_path;
        if (sp != NULL && strcmp(sp, owner_source) == 0) {
            owner_base_name = ctx->plan->modules[m].base_name;
            break;
        }
    }

    if (owner_base_name == NULL) {
        char message[512];
        (void)snprintf(
            message,
            sizeof(message),
            "type '%s' is owned by '%s' which is not in the emission plan",
            type->name != NULL ? type->name : "<unnamed>",
            owner_source);
        set_error(error, error_capacity, message);
        return false;
    }

    if (!safe_snake_name(owner_base_name, out, capacity)) {
        char message[512];
        (void)snprintf(
            message,
            sizeof(message),
            "owner module base_name '%s' cannot be normalized",
            owner_base_name);
        set_error(error, error_capacity, message);
        return false;
    }

    return true;

}


/*
 * Owner lookup for a field/parameter type reference.
 *
 * Strips nullable and array modifiers, ignores primitives and inline
 * streams, and returns the module prefix that owns the resolved user
 * type. external_out is set true only when the owner prefix differs
 * from the current module.
 *
 * Note on by-value cycles: a cycle between two modules where each
 * struct stores the other by value (A { B b; } and B { A a; }) cannot
 * be represented in C. Include guards do not help: they only prevent
 * duplicate inclusion, not recursive type completion. Such cycles
 * must be detected and rejected explicitly; they are out of scope for
 * this helper and for step 2B.
 */
bool resolve_type_owner(

    const emitter_ctx_t *ctx,
    const char *type_ref,
    char *owner_out,
    size_t owner_capacity,
    bool *external_out,
    char *error,
    size_t error_capacity) {

    if (owner_out != NULL && owner_capacity > 0u) {
        owner_out[0] = '\0';
    }
    if (external_out != NULL) {
        *external_out = false;
    }

    if (ctx == NULL || type_ref == NULL || type_ref[0] == '\0') {
        return true;
    }


    /*
     * Strip nullable/collapsible modifiers and, if present, the array
     * suffix so the underlying element type can be resolved. Array
     * aliases and array-typed parameters carry their element type in
     * the base name; the owner is the owner of the element.
     */
    char stripped[256];
    size_t stripped_length = 0u;

    for (const char *p = type_ref;
         *p != '\0' && *p != '[' &&
         stripped_length + 1u < sizeof(stripped);
         ++p) {
        stripped[stripped_length++] = *p;
    }
    stripped[stripped_length] = '\0';

    char base[256];

    if (!scalar_type_name(stripped, base, sizeof(base))) {
        return true;
    }


    const adsl_type_t *declared = find_declared_type_ctx(ctx, base);

    if (declared == NULL) {
        return true;
    }

    char owner[256];

    if (!type_owner_base_snake(
            ctx, declared, owner, sizeof(owner),
            error, error_capacity)) {
        return false;
    }

    if (owner_out != NULL && owner_capacity > 0u) {
        if (strlen(owner) + 1u > owner_capacity) {
            set_error(
                error,
                error_capacity,
                "resolved owner name does not fit");
            return false;
        }
        memcpy(owner_out, owner, strlen(owner) + 1u);
    }

    if (external_out != NULL) {
        *external_out =
            (ctx->base_snake[0] != '\0' &&
             strcmp(owner, ctx->base_snake) != 0);
    }

    return true;
}




static bool external_modules_add(
    external_modules_t *state,
    const char *name) {

    for (size_t i = 0u; i < state->count; ++i) {
        if (strcmp(state->names[i], name) == 0) {
            return true;
        }
    }

    size_t length = strlen(name);

    if (length == 0u || length + 1u > 256u) {
        return false;
    }

    if (state->count == state->capacity) {
        size_t new_capacity =
            state->capacity == 0u ? 4u : state->capacity * 2u;

        char **grown =
            realloc(state->names, new_capacity * sizeof(*grown));

        if (grown == NULL) {
            return false;
        }

        state->names = grown;
        state->capacity = new_capacity;
    }

    char *copy = malloc(length + 1u);

    if (copy == NULL) {
        return false;
    }

    memcpy(copy, name, length + 1u);
    state->names[state->count] = copy;
    state->count++;

    return true;
}

static void external_modules_free(
    external_modules_t *state) {

    if (state == NULL) {
        return;
    }

    for (size_t i = 0u; i < state->count; ++i) {
        free(state->names[i]);
    }

    free(state->names);

    state->names = NULL;
    state->count = 0u;
    state->capacity = 0u;
}


static bool external_modules_record(
    const emitter_ctx_t *ctx,
    const char *type_ref,
    external_modules_t *state,
    char *error,
    size_t error_capacity) {

    if (type_ref == NULL || type_ref[0] == '\0') {
        return true;
    }

    char owner[256];
    bool external = false;

    if (!resolve_type_owner(
            ctx, type_ref, owner, sizeof(owner), &external,
            error, error_capacity)) {
        return false;
    }

    if (!external) {
        return true;
    }

    if (!external_modules_add(state, owner)) {
        char message[512];
        (void)snprintf(
            message,
            sizeof(message),
            "too many external module dependencies for type '%s'",
            type_ref);
        set_error(error, error_capacity, message);
        return false;
    }

    return true;
}



static bool collect_external_modules_types(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->resolver == NULL) {
        return true;
    }

    size_t type_count = 0u;
    const adsl_type_t *types = ctx_types(ctx, &type_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (type->parent != NULL) {
            if (!external_modules_record(
                    ctx, type->parent, state,
                    error, error_capacity)) {
                return false;
            }
        }

        for (size_t f = 0u; f < type->field_count; ++f) {
            if (!external_modules_record(
                    ctx, type->fields[f].type, state,
                    error, error_capacity)) {
                return false;
            }
        }


        /*
         * Hierarchy descendants are deliberately NOT recorded here.
         * An external concrete descendant of a hierarchy union is only
         * referenced through a pointer in the union member, so a
         * forward typedef suffices. Including the owner's complete
         * types header for that purpose would create a header cycle
         * between two modules that are hierarchically related; the
         * forward typedefs are emitted by
         * emit_external_hierarchy_forward_typedefs.
         */
    }


    /*
     * Array aliases for API param/return/throws types are emitted into
     * this module's types header. If the element type is owned by
     * another module, that module's types header must be included here
     * as well, otherwise the alias body references an undefined C type.
     * Non-array API types do not generate an alias and are handled by
     * the API collector instead.
     */
    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {

                const char *ref = method->params[p].type;

                if (ref == NULL ||
                    (strchr(ref, '[') == NULL &&
                     strchr(ref, ']') == NULL)) {
                    continue;
                }

                if (!external_modules_record(
                        ctx, ref, state,
                        error, error_capacity)) {
                    return false;
                }
            }

            const char *return_refs[2] = {
                method->returns_type,
                method->throws_type
            };

            for (size_t r = 0u; r < 2u; ++r) {

                const char *ref = return_refs[r];

                if (ref == NULL ||
                    (strchr(ref, '[') == NULL &&
                     strchr(ref, ']') == NULL)) {
                    continue;
                }

                if (!external_modules_record(
                        ctx, ref, state,
                        error, error_capacity)) {
                    return false;
                }
            }

            for (size_t f = 0u; f < method->return_field_count; ++f) {

                const char *ref = method->return_fields[f].type;

                if (ref == NULL ||
                    (strchr(ref, '[') == NULL &&
                     strchr(ref, ']') == NULL)) {
                    continue;
                }

                if (!external_modules_record(
                        ctx, ref, state,
                        error, error_capacity)) {
                    return false;
                }
            }
        }
    }

    return true;
}



/*
 * Same as collect_external_modules_types, but walks API signatures.
 * Keeping the two views apart avoids pulling a foreign *_types.h into
 * a module whose own type definitions never reference the foreign
 * type, and vice versa.
 */
static bool collect_external_modules_api(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->resolver == NULL) {
        return true;
    }

    size_t api_count = 0u;
    const adsl_api_t *apis = ctx_apis(ctx, &api_count);

    for (size_t a = 0u; a < api_count; ++a) {

        const adsl_api_t *api = &apis[a];

        for (size_t m = 0u; m < api->method_count; ++m) {

            const adsl_method_t *method = &api->methods[m];

            for (size_t p = 0u; p < method->param_count; ++p) {
                if (method->params[p].stream != NULL) {
                    continue;
                }
                if (!external_modules_record(
                        ctx, method->params[p].type, state,
                        error, error_capacity)) {
                    return false;
                }
            }

            if (!external_modules_record(
                    ctx, method->returns_type, state,
                    error, error_capacity)) {
                return false;
            }

            if (!external_modules_record(
                    ctx, method->throws_type, state,
                    error, error_capacity)) {
                return false;
            }

            for (size_t f = 0u; f < method->return_field_count; ++f) {
                if (!external_modules_record(
                        ctx, method->return_fields[f].type, state,
                        error, error_capacity)) {
                    return false;
                }
            }
        }
    }


    return true;
}



/*
 * Source-level collector for the types module.
 *
 * Includes everything the types header already needs (by-value fields,
 * parents, array element types from API signatures) plus the external
 * hierarchy descendants whose fields the impl of a local polymorphic
 * type serializes via the type_id switch.
 *
 * The types header deliberately does not pull in those external
 * headers (a forward typedef suffices for the union pointer member),
 * so the .c file adds them on its own. Without this the impl would
 * fail to compile: it accesses `value.as.child->field` and evaluates
 * `sizeof(child_t)`.
 *
 * API-only dependencies are collected separately for the api module.
 * They are not relevant to the types source.
 */
static bool collect_external_modules_types_source(
    const emitter_ctx_t *ctx,
    external_modules_t *state,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->resolver == NULL) {
        return true;
    }

    if (!collect_external_modules_types(
            ctx, state, error, error_capacity)) {
        return false;
    }


    size_t type_count = 0u;
    const adsl_type_t *types = ctx_types(ctx, &type_count);

    for (size_t i = 0u; i < type_count; ++i) {

        const adsl_type_t *type = &types[i];

        if (!type_is_polymorphic_reference_ctx(ctx, type)) {
            continue;
        }

        size_t hierarchy_count = ctx_hierarchy_type_count(ctx);

        for (size_t h = 0u; h < hierarchy_count; ++h) {

            const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, h);

            if (candidate->enum_count > 0u ||
                candidate->is_abstract ||
                candidate == type ||
                !type_is_descendant_or_self_ctx(ctx, candidate, type)) {
                continue;
            }

            if (!external_modules_record(
                    ctx, candidate->name, state,
                    error, error_capacity)) {
                return false;
            }
        }
    }

    return true;
}



static bool emit_external_includes(
    FILE *output,
    const external_modules_t *state) {

    for (size_t i = 0u; i < state->count; ++i) {
        fprintf(
            output,
            "#include \"%s_types.h\"\n",
            state->names[i]);
    }

    return !ferror(output);
}






/*
 * Project emitter (ТЗ №4-Б2).
 *
 * Emits one selected document from a validated project using the shared
 * ctx-pipeline:
 *
 *   - emission plan is validated first so invalid plans are rejected
 *     before any output file is opened;
 *   - a project resolver is built once and threaded through every
 *     type/API resolution site via emitter_ctx_t;
 *   - only the selected document's own declarations are emitted
 *     (ctx_types()/ctx_apis() return the local arrays in project mode);
 *   - cross-document references are resolved through the resolver;
 *   - the four top-level emitters (types header/source, API header/source)
 *     are the only entry points; no parallel project pipeline exists.
 *
 * The caller owns output_directory and drives the order of module
 * generation. The result of a single call does not depend on the order
 * in which other modules were generated.
 */
bool adsl_emit_c_project(
    const adsl_project_t *project,
    const adsl_emission_plan_t *plan,
    size_t document_index,
    const char *output_directory,
    const char *base_name,
    char *error,
    size_t error_capacity) {

    if (project == NULL ||
        plan == NULL ||
        output_directory == NULL ||
        base_name == NULL ||
        output_directory[0] == '\0' ||
        base_name[0] == '\0') {

        set_error(error, error_capacity, "invalid project emitter arguments");
        return false;
    }

    if (document_index >= project->document_count) {
        char message[256];
        (void)snprintf(
            message,
            sizeof(message),
            "document index %zu is out of range (project has %zu documents)",
            document_index,
            project->document_count);
        set_error(error, error_capacity, message);
        return false;
    }

    if (plan->modules == NULL || plan->module_count == 0u) {
        set_error(error, error_capacity, "emission plan is empty");
        return false;
    }

    if (plan->module_count != project->document_count) {
        char message[256];
        (void)snprintf(
            message,
            sizeof(message),
            "emission plan has %zu entries but project has %zu documents",
            plan->module_count,
            project->document_count);
        set_error(error, error_capacity, message);
        return false;
    }

    for (size_t m = 0u; m < plan->module_count; ++m) {
        if (plan->modules[m].source_path == NULL ||
            plan->modules[m].source_path[0] == '\0' ||
            plan->modules[m].base_name == NULL ||
            plan->modules[m].base_name[0] == '\0') {
            char message[256];
            (void)snprintf(
                message,
                sizeof(message),
                "emission plan entry %zu has empty source_path or base_name",
                m);
            set_error(error, error_capacity, message);
            return false;
        }

        size_t matches = 0u;
        for (size_t d = 0u; d < project->document_count; ++d) {
            const char *sp = project->documents[d].root_source_path;
            if (sp != NULL &&
                strcmp(plan->modules[m].source_path, sp) == 0) {
                matches++;
            }
        }

        if (matches == 0u) {
            char message[256];
            (void)snprintf(
                message,
                sizeof(message),
                "emission plan entry '%s' does not match any project document",
                plan->modules[m].source_path);
            set_error(error, error_capacity, message);
            return false;
        }

        if (matches > 1u) {
            char message[256];
            (void)snprintf(
                message,
                sizeof(message),
                "emission plan entry '%s' matches multiple project documents",
                plan->modules[m].source_path);
            set_error(error, error_capacity, message);
            return false;
        }
    }

    for (size_t d = 0u; d < project->document_count; ++d) {
        const char *sp = project->documents[d].root_source_path;

        if (sp == NULL || sp[0] == '\0') {
            char message[256];
            (void)snprintf(
                message,
                sizeof(message),
                "project document %zu has no root_source_path",
                d);
            set_error(error, error_capacity, message);
            return false;
        }

        for (size_t other = d + 1u; other < project->document_count; ++other) {
            const char *other_sp = project->documents[other].root_source_path;
            if (other_sp != NULL && strcmp(sp, other_sp) == 0) {
                char message[256];
                (void)snprintf(
                    message,
                    sizeof(message),
                    "project documents %zu and %zu share root_source_path '%s'",
                    d, other, sp);
                set_error(error, error_capacity, message);
                return false;
            }
        }

        size_t found = 0u;
        for (size_t m = 0u; m < plan->module_count; ++m) {
            if (strcmp(plan->modules[m].source_path, sp) == 0) {
                found++;
            }
        }

        if (found != 1u) {
            char message[256];
            (void)snprintf(
                message,
                sizeof(message),
                "emission plan has %zu entries for document '%s' (expected 1)",
                found, sp);
            set_error(error, error_capacity, message);
            return false;
        }
    }

    const char *expected_base_name = NULL;
    const char *current_source_path =
        project->documents[document_index].root_source_path;

    for (size_t m = 0u; m < plan->module_count; ++m) {
        if (strcmp(plan->modules[m].source_path, current_source_path) == 0) {
            expected_base_name = plan->modules[m].base_name;
            break;
        }
    }

    if (expected_base_name == NULL ||
        strcmp(expected_base_name, base_name) != 0) {
        char message[256];
        (void)snprintf(
            message,
            sizeof(message),
            "base_name '%s' does not match emission plan entry '%s'",
            base_name,
            expected_base_name != NULL ? expected_base_name : "<none>");
        set_error(error, error_capacity, message);
        return false;
    }

    for (size_t i = 0u; i < plan->module_count; ++i) {
        char snake_i[256];

        if (!safe_snake_name(
                plan->modules[i].base_name,
                snake_i,
                sizeof(snake_i))) {

            char message[512];
            (void)snprintf(
                message,
                sizeof(message),
                "emission plan base_name '%s' cannot be normalized to a unique generated filename",
                plan->modules[i].base_name);
            set_error(error, error_capacity, message);
            return false;
        }

        for (size_t j = i + 1u; j < plan->module_count; ++j) {
            char snake_j[256];

            if (!safe_snake_name(
                    plan->modules[j].base_name,
                    snake_j,
                    sizeof(snake_j))) {

                char message[512];
                (void)snprintf(
                    message,
                    sizeof(message),
                    "emission plan base_name '%s' cannot be normalized to a unique generated filename",
                    plan->modules[j].base_name);
                set_error(error, error_capacity, message);
                return false;
            }

            if (strcmp(snake_i, snake_j) == 0) {
                char message[512];
                (void)snprintf(
                    message,
                    sizeof(message),
                    "emission plan has colliding module names after normalization: '%s'",
                    snake_i);
                set_error(error, error_capacity, message);
                return false;
            }
        }
    }

    adsl_project_resolver_t *resolver = NULL;

    if (!adsl_project_resolver_build(
            project, &resolver, error, error_capacity)) {
        return false;
    }

    const adsl_document_t *document =
        &project->documents[document_index];


emitter_ctx_t ctx;
    ctx.document = document;

    ctx.resolver = resolver;
    ctx.project = project;
    ctx.plan = plan;
    snake_name(base_name, ctx.base_snake, sizeof(ctx.base_snake));

    /*
     * The module abbreviation is derived from the physical ADSL file that
     * owns this document, not from the caller's base_name. All symbols of
     * this module -- types, serializers, API functions and stream builders
     * -- must carry the same prefix, so every one of them is looked up
     * from this single field.
     */
    ctx.module_abbr[0] = '\0';

    if (document->root_source_path != NULL) {
        abbreviate_from_path(
            document->root_source_path,
            ctx.module_abbr,
            sizeof(ctx.module_abbr));
    }



    char base_snake[256];

    if (!safe_snake_name(base_name, base_snake, sizeof(base_snake))) {
        adsl_project_resolver_release(resolver);
        set_error(
            error,
            error_capacity,
            "project emitter base_name cannot be normalized to a generated filename");
        return false;
    }


    char types_h[1024];
    char types_c[1024];
    char api_h[1024];
    char api_c[1024];

    if (!build_output_path(types_h, sizeof(types_h), output_directory, base_snake, "_types.h") ||
        !build_output_path(types_c, sizeof(types_c), output_directory, base_snake, "_types.c") ||
        !build_output_path(api_h, sizeof(api_h), output_directory, base_snake, "_api.h") ||
        !build_output_path(api_c, sizeof(api_c), output_directory, base_snake, "_api.c")) {

        adsl_project_resolver_release(resolver);
        set_error(
            error,
            error_capacity,
            "project emitter output path is too long");
        return false;
    }

    FILE *types_h_file = fopen(types_h, "wb");
    if (types_h_file == NULL) {
        adsl_project_resolver_release(resolver);
        set_error(error, error_capacity, "cannot create generated types header");
        return false;
    }

    if (!emit_types_header_ctx(
            types_h_file, &ctx, base_name, error, error_capacity)) {
        fclose(types_h_file);
        adsl_project_resolver_release(resolver);
        return false;
    }

    fprintf(
        types_h_file,
        "\n#ifdef __cplusplus\n}\n#endif\n#endif\n");
    fclose(types_h_file);

    FILE *types_c_file = fopen(types_c, "wb");
    if (types_c_file == NULL) {
        adsl_project_resolver_release(resolver);
        set_error(error, error_capacity, "cannot create generated types source");
        return false;
    }

    fprintf(
        types_c_file,
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n",
        base_snake);

    if (!emit_types_source_ctx(
            types_c_file, &ctx, base_name, error, error_capacity)) {
        fclose(types_c_file);
        adsl_project_resolver_release(resolver);
        return false;
    }

    fclose(types_c_file);

    FILE *api_h_file = fopen(api_h, "wb");
    if (api_h_file == NULL) {
        adsl_project_resolver_release(resolver);
        set_error(error, error_capacity, "cannot create generated API header");
        return false;
    }

    fprintf(
        api_h_file,
        "#ifndef %s_API_H\n"
        "#define %s_API_H\n\n"
        "#include \"%s_types.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n\n",
        base_snake, base_snake, base_snake);

    if (!emit_api_header_ctx(
            api_h_file, &ctx, base_name, NULL, NULL, error, error_capacity)) {
        fclose(api_h_file);
        adsl_project_resolver_release(resolver);
        return false;
    }

    fprintf(
        api_h_file,
        "#ifdef __cplusplus\n}\n#endif\n#endif\n");
    fclose(api_h_file);

    FILE *api_c_file = fopen(api_c, "wb");
    if (api_c_file == NULL) {
        adsl_project_resolver_release(resolver);
        set_error(error, error_capacity, "cannot create generated API source");
        return false;
    }

    fprintf(
        api_c_file,
        "#include \"%s_api.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n",
        base_snake);

    if (!emit_api_source_ctx(
            api_c_file, &ctx, base_name, NULL, NULL, error, error_capacity)) {
        fclose(api_c_file);
        adsl_project_resolver_release(resolver);
        return false;
    }

    fclose(api_c_file);


    adsl_project_resolver_release(resolver);
    return true;
}







static bool emit_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const char *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid value expression context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated value serialization depth exceeds 64");
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

        return emit_array_expression_ctx(
            output,
            ctx,
            base_snake,
            type,
            expression,
            prefix,
            struct_pointer,
            depth,
            runtime_depth,
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
        find_declared_type_ctx(ctx, base);



    if (declared_type == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated value type");
        return false;
    }



    if (declared_type->has_tiered_int) {
        char tiered_name[256];
        char tiered_expression[768];

        if (!declared_type_c_name_ctx(
                ctx,
                declared_type,
                tiered_name,
                sizeof(tiered_name),
                error,
                error_capacity)) {
            return false;
        }


        int tiered_expression_size =
            struct_pointer
                ? snprintf(
                      tiered_expression,
                      sizeof(tiered_expression),
                      "(*(%s))",
                      expression)
                : snprintf(
                      tiered_expression,
                      sizeof(tiered_expression),
                      "%s",
                      expression);

        if (tiered_expression_size < 0 ||
            (size_t)tiered_expression_size >=
                sizeof(tiered_expression)) {

            set_error(
                error,
                error_capacity,
                "generated tieredInt expression is too long");
            return false;
        }

        fprintf(
            output,
            "    {\n"
            "        aether_status_t meta_status = aether_meta_write_tiered_int(\n"
            "            tx,\n"
            "            tx_capacity,\n"
            "            &pos,\n"
            "            (uint64_t)(%s),\n"
            "            %uu,\n"
            "            %s_tiered_int_limits,\n"
            "            %zuu);\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            tiered_expression,
            (unsigned int)declared_type->tiered_int_min_bytes,
            tiered_name,
            declared_type->tiered_int_limit_count);

        return true;
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

    if (type_is_polymorphic_reference_ctx(ctx, declared_type)) {

        return emit_hierarchy_expression_ctx(
            output,
            ctx,
            base_snake,
            declared_type,
            expression,
            prefix,
            struct_pointer,
            depth,
            runtime_depth,
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

    char nested_name[256];

    declared_c_name(
        "",
        declared_type->name,
        nested_name,
        sizeof(nested_name));

    /*
     * The nested type may be declared in a different input file than the
     * one currently being emitted. Its serialize/deserialize symbols are
     * always prefixed by its own declaring file, so derive that prefix
     * here instead of reusing the caller's base_snake.
     */
    char nested_prefix[32];

    if (!type_effective_prefix_ctx(
            ctx,
            declared_type,
            nested_prefix,
            sizeof(nested_prefix),
            error,
            error_capacity)) {
        return false;
    }



    char nested_value[1024];
    int nested_value_size =
        snprintf(
            nested_value,
            sizeof(nested_value),
            struct_pointer ? "%s" : "(&(%s))",
            expression);

    if (nested_value_size < 0 ||
        (size_t)nested_value_size >= sizeof(nested_value)) {

        set_error(
            error,
            error_capacity,
            "generated nested struct serializer expression is too long");
        return false;
    }


    /*
     * Standalone wrappers sometimes construct a context without
     * module_abbr. Snapshot the context and fill it from the caller's
     * authoritative prefix before calling the strict owner helper.
     * In project mode module_abbr is already set, so this is a no-op.
     */
    emitter_ctx_t effective_ctx = *ctx;

    if (effective_ctx.module_abbr[0] == '\0' && base_snake != NULL) {
        (void)snprintf(
            effective_ctx.module_abbr,
            sizeof(effective_ctx.module_abbr),
            "%s",
            base_snake);
    }
    if (effective_ctx.base_snake[0] == '\0' && base_snake != NULL) {
        (void)snprintf(
            effective_ctx.base_snake,
            sizeof(effective_ctx.base_snake),
            "%s",
            base_snake);
    }


    char nested_owner[256];

    if (!type_owner_base_snake(
            &effective_ctx, declared_type, nested_owner,
            sizeof(nested_owner), error, error_capacity)) {
        return false;
    }

    /*
     * In unified/standalone mode (no resolver) every declaration is emitted
     * into the same aether_api.c, so a nested reference is always local and
     * is called through the static *_impl directly. The owner comparison is
     * only meaningful in the legacy project emitter, where each type has its
     * own translation unit and non-owners must go through *_with_depth.
     */
    bool nested_in_same_tu =
        ctx->resolver == NULL ||
        strcmp(nested_owner, effective_ctx.base_snake) == 0;



    if (runtime_depth != NULL) {
        /*
         * Nested call inside another generated impl.
         *
         * Local type: call the static impl directly (cheapest path,
         * unchanged from standalone).
         *
         * External type: the impl is private to the owner's
         * translation unit, so the call must go through the owner's
         * exported *_with_depth entry point. The caller still adds
         * one to the recursion depth because the callee is entered
         * at the impl-equivalent level.
         */
        if (nested_in_same_tu) {
            fprintf(
                output,
                "    {\n"
                "        aether_status_t meta_status = %s_serialize_%s_impl(\n"
                "            tx,\n"
                "            tx_capacity,\n"
                "            &pos,\n"
                "            %s,\n"
                "            %s + 1u);\n"
                "        if (meta_status != AETHER_OK) return meta_status;\n"
                "    }\n",
                nested_prefix,
                nested_name,
                nested_value,
                runtime_depth);
        } else {
            fprintf(
                output,
                "    {\n"
                "        aether_status_t meta_status = %s_serialize_%s_with_depth(\n"
                "            tx,\n"
                "            tx_capacity,\n"
                "            &pos,\n"
                "            %s,\n"
                "            %s + 1u);\n"
                "        if (meta_status != AETHER_OK) return meta_status;\n"
                "    }\n",
                nested_owner,
                nested_name,
                nested_value,
                runtime_depth);
        }
    } else {
        /*
         * No enclosing impl: this call starts a fresh top-level
         * serialization, so depth is zero. Call the public wrapper of
         * whichever module owns the type (in standalone the owner is
         * always the current module).
         */
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
            nested_owner,
            nested_name,
            nested_value);
    }


    return true;
}






static bool emit_struct_definition_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    size_t type_index,
    uint8_t *states,
    array_alias_state_t *alias_state,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid struct definition context");
        return false;
    }

    const adsl_type_t *type;


    if (ctx->resolver != NULL) {
        type = &ctx->document->types[type_index];
    } else if (ctx->document->resolved_type_count > 0u) {
        type = &ctx->document->resolved_types[type_index];
    } else {
        type = &ctx->document->types[type_index];
    }


    if (states[type_index] == 2u) {
        return true;
    }

    if (states[type_index] == 1u) {
        set_error(
            error,
            error_capacity,
            "non-nullable by-value type cycle cannot be represented in generated C");
        return false;
    }

    if (type->enum_count > 0u ||
        type->is_abstract ||
        type->stream != NULL ||
        type->has_tiered_int) {

        states[type_index] = 2u;
        return true;
    }

    states[type_index] = 1u;

    if (!emit_all_struct_dependencies_ctx(
            output,
            ctx,
            type,
            states,
            0u,
            alias_state,
            error,
            error_capacity)) {

        return false;
    }

    char type_name[256];

    if (!declared_type_c_name_ctx(
            ctx,
            type,
            type_name,
            sizeof(type_name),
            error,
            error_capacity)) {
        return false;
    }


    fprintf(
        output,
        "struct %s_t {\n",
        type_name);

    if (!emit_all_struct_field_declarations_ctx(
            output,
            ctx,
            type,
            0u,
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        "};\n\n");

    states[type_index] = 2u;

    return true;
}









static bool emit_array_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const char *type,
    const char *expression,
    const char *prefix,
    bool array_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid array expression context");
        return false;
    }

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

    if (!emit_value_expression_ctx(
            output,
            ctx,
            base_snake,
            element,
            element_expression,
            element_prefix,
            false,
            depth + 1u,
            runtime_depth,
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        "    }\n");

    return true;
}







static bool emit_struct_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *declared_type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid struct value expression context");
        return false;
    }

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

    size_t nullable_count = 0u;

    if (!count_all_nullable_fields_ctx(
            ctx,
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

        size_t bit_index = 0u;

        if (!emit_all_nullable_mask_bits_ctx(
                output,
                ctx,
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

        for (size_t b = 0u; b < mask_width; ++b) {

            fprintf(
                output,
                "    tx[pos++] = (uint8_t)"
                "((%s_nullable_mask >> %zuu) & UINT64_C(0xff));\n",
                prefix,
                b * 8u);
        }
    }

    return emit_all_struct_payload_ctx(
        output,
        ctx,
        base_snake,
        declared_type,
        expression,
        prefix,
        struct_pointer,
        depth,
        runtime_depth,
        error,
        error_capacity);
}







static bool type_has_children_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type) {

    if (ctx == NULL || ctx->document == NULL) {
        return false;
    }


    /*
     * Hierarchy analysis must see every descendant declared anywhere in
     * the project, not only in the current document. ctx_hierarchy_*
     * exposes the resolver's global type view in project mode and the
     * include closure in standalone mode; this is deliberately distinct
     * from ctx_types(), which enumerates only the current module's own
     * declarations for generation.
     */
    size_t count = ctx_hierarchy_type_count(ctx);



    for (size_t i = 0u; i < count; ++i) {

        const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);


        if (candidate->parent == NULL) {
            continue;
        }

        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, candidate->parent);

        if (parent == type) {
            return true;
        }
    }

    return false;
}







static bool type_is_polymorphic_reference_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type) {

    if (ctx == NULL) {
        return false;
    }

    return type != NULL &&
           !type->has_tiered_int &&
           type->enum_count == 0u &&
           (type->is_abstract ||
            type_has_children_ctx(ctx, type));
}


static bool type_is_polymorphic_reference(
    const adsl_document_t *document,
    const adsl_type_t *type) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return type_is_polymorphic_reference_ctx(&ctx, type);
}



static bool type_is_descendant_or_self_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *candidate,
    const adsl_type_t *ancestor) {

    if (ctx == NULL) {
        return false;
    }

    const adsl_type_t *current = candidate;

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
            find_declared_type_ctx(ctx, current->parent);
    }

    return false;
}







static bool emit_hierarchy_ref_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid hierarchy reference context");
        return false;
    }

    if (!type_is_polymorphic_reference_ctx(ctx, type)) {
        return true;
    }


    size_t count = ctx_hierarchy_type_count(ctx);


    char static_name[256];

    if (!declared_type_c_name_ctx(
            ctx,
            type,
            static_name,
            sizeof(static_name),
            error,
            error_capacity)) {
        return false;
    }


    size_t concrete_count = 0u;


    for (size_t i = 0u; i < count; ++i) {

        const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);


        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self_ctx(ctx, candidate, type)) {

            continue;
        }

        concrete_count++;

        if (!candidate->has_id || candidate->id == 0u) {

            char message[512];

            int written = snprintf(
                message,
                sizeof(message),
                "concrete hierarchy type '%s' must have a non-zero one-byte id",
                candidate->name);

            if (written < 0 || (size_t)written >= sizeof(message)) {
                set_error(
                    error,
                    error_capacity,
                    "concrete hierarchy type must have a non-zero one-byte id");
            } else {
                set_error(error, error_capacity, message);
            }

            return false;
        }


        for (size_t j = i + 1u; j < count; ++j) {

            const adsl_type_t *other = ctx_hierarchy_type_at(ctx, j);


            if (other->enum_count > 0u ||
                other->is_abstract ||
                !type_is_descendant_or_self_ctx(ctx, other, type)) {

                continue;
            }

            if (other->has_id && other->id == candidate->id) {

                char message[768];

                int written = snprintf(
                    message,
                    sizeof(message),
                    "hierarchy '%s' has duplicate concrete type id %u for '%s' and '%s'",
                    type->name,
                    (unsigned int)candidate->id,
                    candidate->name,
                    other->name);

                if (written < 0 || (size_t)written >= sizeof(message)) {
                    set_error(
                        error,
                        error_capacity,
                        "hierarchy has duplicate concrete type ids");
                } else {
                    set_error(error, error_capacity, message);
                }

                return false;
            }
        }
    }

    for (size_t i = 0u; i < count; ++i) {


        const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);


        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self_ctx(ctx, candidate, type)) {

            continue;
        }

        char static_upper[256];
        char candidate_upper[256];
        char static_c_name[256];
        char candidate_c_name[256];


        if (!declared_type_c_name_ctx(
                ctx,
                type,
                static_c_name,
                sizeof(static_c_name),
                error,
                error_capacity)) {
            return false;
        }

        if (!declared_type_c_name_ctx(
                ctx,
                candidate,
                candidate_c_name,
                sizeof(candidate_c_name),
                error,
                error_capacity)) {
            return false;
        }


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


    /*
     * Forward typedefs for external concrete descendants.
     *
     * The hierarchy union below holds only pointers to concrete
     * descendants, so a forward declaration is enough. Including the
     * owner's full types header here would create a header cycle when
     * two modules are hierarchically related (G.h → H.h → G.h). The
     * complete definition is only needed by this module's .c file,
     * which includes the owner's types header on its own.
     */

    if (ctx->resolver != NULL) {
        for (size_t i = 0u; i < count; ++i) {

            const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);

            if (candidate->enum_count > 0u ||
                candidate->is_abstract ||
                !type_is_descendant_or_self_ctx(ctx, candidate, type)) {
                continue;
            }

            char owner[256];

            if (!type_owner_base_snake(
                    ctx, candidate, owner, sizeof(owner),
                    error, error_capacity)) {
                return false;
            }

            if (strcmp(owner, ctx->base_snake) == 0) {
                continue;
            }

            char candidate_name[256];

            if (!declared_type_c_name_ctx(
                    ctx,
                    candidate,
                    candidate_name,
                    sizeof(candidate_name),
                    error,
                    error_capacity)) {
                return false;
            }


            fprintf(
                output,
                "typedef struct %s_t %s_t;\n",
                candidate_name,
                candidate_name);
        }
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

        for (size_t i = 0u; i < count; ++i) {


            const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);


            if (candidate->enum_count > 0u ||
                candidate->is_abstract ||
                !type_is_descendant_or_self_ctx(ctx, candidate, type)) {

                continue;
            }

            char candidate_name[256];

            if (!declared_type_c_name_ctx(
                    ctx,
                    candidate,
                    candidate_name,
                    sizeof(candidate_name),
                    error,
                    error_capacity)) {
                return false;
            }


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








static bool emit_hierarchy_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *static_type,
    const char *expression,
    const char *prefix,
    bool reference_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid hierarchy expression context");
        return false;
    }

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


    size_t count = ctx_hierarchy_type_count(ctx);


    size_t concrete_count = 0u;


    for (size_t i = 0u; i < count; ++i) {

        const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);


        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self_ctx(ctx, candidate, static_type)) {

            continue;
        }

        concrete_count++;

        char candidate_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                candidate,
                candidate_name,
                sizeof(candidate_name),
                error,
                error_capacity)) {
            return false;
        }


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


        if (!emit_struct_value_expression_ctx(
                output,
                ctx,
                base_snake,
                candidate,
                child_expression,
                child_prefix,
                true,
                depth + 1u,
                runtime_depth,
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







static bool emit_all_struct_dependencies_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    uint8_t *states,
    unsigned int depth,
    array_alias_state_t *alias_state,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid struct dependency context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");
            return false;
        }

        if (!emit_all_struct_dependencies_ctx(
                output,
                ctx,
                parent,
                states,
                depth + 1u,
                alias_state,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u; f < type->field_count; ++f) {

        const adsl_field_t *field = &type->fields[f];

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
                find_declared_type_ctx(ctx, array_element);

            if (!array_dynamic &&
                element_type != NULL &&
                !element_type->has_tiered_int &&
                element_type->enum_count == 0u &&
                !element_type->is_abstract &&
                element_type->stream == NULL &&
                !type_is_polymorphic_reference_ctx(ctx, element_type)) {


                /*
                 * Inline T[N] needs sizeof(T), so finish T first.
                 * If T belongs to another project module, its complete
                 * body is the owner's responsibility and this module
                 * only records the dependency (handled by the project
                 * dependency generation step). Otherwise finish the
                 * local body and emit the alias immediately before the
                 * owning struct definition.
                 */
                size_t nested_index =
                    ctx_local_type_index(ctx, element_type);

                if (nested_index != SIZE_MAX &&
                    (!emit_struct_definition_ctx(
                        output,
                        ctx,
                        nested_index,
                        states,
                        alias_state,
                        error,
                        error_capacity) ||
                    !emit_array_alias_ctx(
                        output,
                        ctx,
                        alias_state,
                        field->type,
                        error,
                        error_capacity))) {

                    return false;
                }

            }

            continue;
        }

        if (strchr(field->type, '?') != NULL) {
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
            find_declared_type_ctx(ctx, base);

        if (field_type == NULL) {
            set_error(
                error,
                error_capacity,
                "unknown generated struct field type");
            return false;
        }

        if (field_type->has_tiered_int ||
            field_type->enum_count > 0u ||
            type_is_polymorphic_reference_ctx(ctx, field_type)) {

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
            ctx_local_type_index(ctx, field_type);

        if (nested_index != SIZE_MAX &&
            !emit_struct_definition_ctx(
                output,
                ctx,
                nested_index,
                states,
                alias_state,
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

static bool format_c_type_ctx(
    const emitter_ctx_t *ctx,
    const char *type,
    char *base_out,
    size_t base_capacity,
    bool *out_pointer,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL ||
        type == NULL ||
        base_out == NULL ||
        base_capacity == 0u ||
        out_pointer == NULL) {

        set_error(error, error_capacity, "invalid C type request");
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

        if (!array_alias_name_ctx(
                ctx,
                cleaned,
                base_out,
                base_capacity,
                error,
                error_capacity)) {
            if (error != NULL && error[0] == '\0') {
                set_error(error, error_capacity, "field array alias is too long");
            }
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

    const adsl_type_t *declared = find_declared_type_ctx(ctx, base);

    if (declared == NULL) {
        set_error(error, error_capacity, "unknown field type");
        return false;
    }

    char name[256];

    if (!declared_type_c_name_ctx(
            ctx, declared, name, sizeof(name), error, error_capacity)) {
        return false;
    }

    if (declared->stream != NULL ||
        declared->enum_count > 0u ||
        declared->has_tiered_int) {

        int written = snprintf(base_out, base_capacity, "%s_t", name);
        if (written < 0 || (size_t)written >= base_capacity) {
            set_error(error, error_capacity, "field type is too long");
            return false;
        }
        *out_pointer = nullable;
        return true;
    }

    if (type_is_polymorphic_reference_ctx(ctx, declared)) {
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


static bool format_c_type(
    const adsl_document_t *document,
    const char *type,
    char *base_out,
    size_t base_capacity,
    bool *out_pointer,
    char *error,
    size_t error_capacity) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return format_c_type_ctx(
        &ctx, type, base_out, base_capacity, out_pointer, error, error_capacity);
}






static bool emit_field_declaration_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity);

static bool emit_field_declaration(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity);





static bool emit_all_struct_field_declarations_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    unsigned int depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid struct field context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL ||
            !emit_all_struct_field_declarations_ctx(
                output,
                ctx,
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

    for (size_t f = 0u; f < type->field_count; ++f) {

        const adsl_field_t *field = &type->fields[f];


        if (!emit_field_declaration_ctx(
                output,
                ctx,

                field->type,
                field->name,
                error,
                error_capacity)) {
            return false;
        }
    }

    return true;
}







static bool count_all_nullable_fields_ctx(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    unsigned int depth,
    size_t *count,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid nullable field context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");
            return false;
        }

        if (!count_all_nullable_fields_ctx(
                ctx,
                parent,
                depth + 1u,
                count,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u; f < type->field_count; ++f) {

        if (strchr(
                type->fields[f].type,
                '?') != NULL) {

            (*count)++;
        }
    }

    return true;
}







static bool emit_all_nullable_mask_bits_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    size_t *bit_index,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid nullable mask context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL ||
            !emit_all_nullable_mask_bits_ctx(
                output,
                ctx,
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

    for (size_t f = 0u; f < type->field_count; ++f) {

        const adsl_field_t *field = &type->fields[f];

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








static bool emit_all_struct_payload_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *base_snake,
    const adsl_type_t *type,
    const char *expression,
    const char *prefix,
    bool struct_pointer,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid struct payload context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "struct inheritance depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");
            return false;
        }

        if (!emit_all_struct_payload_ctx(
                output,
                ctx,
                base_snake,
                parent,
                expression,
                prefix,
                struct_pointer,
                depth + 1u,
                runtime_depth,
                error,
                error_capacity)) {

            return false;
        }
    }

    for (size_t f = 0u; f < type->field_count; ++f) {

        const adsl_field_t *field = &type->fields[f];

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
            if (!emit_value_expression_ctx(
                    output,
                    ctx,
                    base_snake,
                    field->type,
                    field_expression,
                    field_prefix,
                    false,
                    depth + 1u,
                    runtime_depth,
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
                find_declared_type_ctx(ctx, base);

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


        if (!emit_value_expression_ctx(
                output,
                ctx,
                base_snake,
                underlying_type,
                nullable_expression,
                field_prefix,
                pointer_value,
                depth + 1u,
                runtime_depth,
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





        size_t nullable_count =
            method_nullable_param_count(
                method);

        if (nullable_count > 64u) {
            set_error(
                error,
                error_capacity,
                "API method has more than 64 nullable parameters");

            return false;
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


    char abbr[32];
    char api_name[256];
    char method_name[256];

    abbreviate_from_path(document->root_source_path, abbr, sizeof(abbr));
    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));


    fprintf(
        output,

        "typedef aether_status_t (*%s_%s_%s_fn)(\n"
        "    void *ctx",
        abbr,
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
    char abbr[32];
    char api_name[256];

    upper_name(base_name, guard, sizeof(guard));
    snake_name(base_name, base_snake, sizeof(base_snake));
    abbreviate_from_path(document->root_source_path, abbr, sizeof(abbr));
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

        emit_method_origin(
            output,
            effective[m].owner,
            effective[m].method);


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

            "    %s_%s_%s_fn %s;\n",
            abbr,
            api_name,
            method_name,
            method_name);

    }

    free(effective);


    fprintf(
        output,

        "} %s_%s_local_t;\n\n"
        "aether_status_t %s_%s_dispatch(\n"
        "    %s_%s_local_t *api,\n"
        "    aether_meta_read_context_t *ctx);\n\n"
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n\n"
        "#endif\n",
        abbr,
        api_name,
        abbr,
        api_name,
        abbr,
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
    const char *abbr,
    const adsl_param_t *param,
    const char *base_snake,
    size_t nullable_bit_index,
    char *error,
    size_t error_capacity) {



    const char *type = param->type;


    if (type != NULL &&
        strchr(type, '?') != NULL) {

        char cleaned[256];

        if (!strip_nullable_suffix(
                type,
                cleaned,
                sizeof(cleaned))) {

            set_error(
                error,
                error_capacity,
                "nullable local parameter type is too long");

            return false;
        }

        if (param->stream != NULL) {
            char stream_name[256];


            declared_c_name(
                abbr,
                cleaned,
                stream_name,
                sizeof(stream_name));


            fprintf(
                output,
                "        %s_t *%s = NULL;\n"
                "        if ((_meta_nullable_mask & "
                "(UINT64_C(1) << %zuu)) == 0u) {\n"
                "            %s = (%s_t *)aether_meta_scratch_alloc(\n"
                "                ctx,\n"
                "                sizeof(%s_t),\n"
                "                _Alignof(%s_t));\n"
                "            if (%s == NULL) return AETHER_ERR_OVERFLOW;\n"
                "            memset(%s, 0, sizeof(*%s));\n"
                "            {\n"
                "                const uint8_t *_meta_stream_data = NULL;\n"
                "                size_t _meta_stream_length = 0u;\n"
                "                if (!aether_meta_read_view(\n"
                "                        &ctx->reader,\n"
                "                        &_meta_stream_data,\n"
                "                        &_meta_stream_length)) {\n"
                "                    return AETHER_ERR_PROTOCOL;\n"
                "                }\n"
                "                %s->data = (uint8_t *)_meta_stream_data;\n"
                "                %s->capacity = _meta_stream_length;\n"
                "                %s->length = _meta_stream_length;\n"
                "            }\n"
                "        }\n",
                stream_name,
                param->name,
                nullable_bit_index,
                param->name,
                stream_name,
                stream_name,
                stream_name,
                param->name,
                param->name,
                param->name,
                param->name,
                param->name,
                param->name);

            return true;
        }

        char ctype[272];
        bool ignored_pointer = false;

        if (!format_c_type(
                document,
                cleaned,
                ctype,
                sizeof(ctype),
                &ignored_pointer,
                error,
                error_capacity)) {

            return false;
        }

        (void)ignored_pointer;

        fprintf(
            output,
            "        %s *%s = NULL;\n"
            "        if ((_meta_nullable_mask & "
            "(UINT64_C(1) << %zuu)) == 0u) {\n"
            "            %s = (%s *)aether_meta_scratch_alloc(\n"
            "                ctx,\n"
            "                sizeof(%s),\n"
            "                _Alignof(%s));\n"
            "            if (%s == NULL) return AETHER_ERR_OVERFLOW;\n"
            "            memset(%s, 0, sizeof(*%s));\n",
            ctype,
            param->name,
            nullable_bit_index,
            param->name,
            ctype,
            ctype,
            ctype,
            param->name,
            param->name,
            param->name);

        char target[320];

        int target_size =
            snprintf(
                target,
                sizeof(target),
                "(*%s)",
                param->name);

        if (target_size < 0 ||
            (size_t)target_size >=
                sizeof(target)) {

            set_error(
                error,
                error_capacity,
                "generated nullable decode target is too long");

            return false;
        }


        if (!emit_decode_value_expression_ctx(
                output,
                &(emitter_ctx_t){ .document = document, .resolver = NULL },
                cleaned,
                target,

                base_snake,
                0u,

                NULL,

                error,
                error_capacity)) {

            return false;
        }

        fputs(
            "        }\n",
            output);

        return true;
    }



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
            abbr,
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


            if (!emit_decode_value_expression_ctx(
                    output,
                    &(emitter_ctx_t){ .document = document, .resolver = NULL },

                    type,
                    param->name,
                    base_snake,
                    0u,

                    NULL,

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


        if (!emit_decode_value_expression_ctx(
                output,
                &(emitter_ctx_t){ .document = document, .resolver = NULL },

                type,
                array_target,
                base_snake,
                0u,

                NULL,

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


        if (!emit_decode_value_expression_ctx(
                output,
                &(emitter_ctx_t){ .document = document, .resolver = NULL },

                type,
                param->name,
                base_snake,
                0u,

                NULL,

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
    declared_c_name(abbr, declared->name, declared_name, sizeof(declared_name));


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


    if (declared->enum_count > 0u ||
        declared->has_tiered_int) {
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


        if (!emit_decode_value_expression_ctx(
                output,
                &(emitter_ctx_t){ .document = document, .resolver = NULL },

                type,
                struct_target,
                base_snake,
                0u,

                NULL,

                error,
                error_capacity)) {
            return false;
        }

        return true;
    }



    if (!emit_decode_value_expression_ctx(
            output,
            &(emitter_ctx_t){ .document = document, .resolver = NULL },

            type,
            param->name,
            base_snake,
            0u,

            NULL,

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

        bool nullable =
            param->type != NULL &&
            strchr(param->type, '?') != NULL;

        bool needs_address =
            !nullable &&
            (kind == C_PARAM_KIND_STREAM_PTR ||
             kind == C_PARAM_KIND_REF_PTR);


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
    const char *abbr,
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


        "aether_status_t %s_%s_dispatch(\n"
        "    %s_%s_local_t *api,\n"
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
        abbr,
        api_name,
        abbr,
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


        emit_method_origin(
            output,
            effective[m].owner,
            method);


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


        if (!emit_local_nullable_param_mask(
                output,
                method,
                error,
                error_capacity)) {

            fclose(output);
            return false;
        }



        size_t nullable_bit_index = 0u;

        for (size_t p = 0u;
             p < method->param_count;
             ++p) {

            const adsl_param_t *param =
                &method->params[p];

            size_t param_nullable_bit =
                nullable_bit_index;

            if (param->type != NULL &&
                strchr(param->type, '?') != NULL) {

                nullable_bit_index++;
            }


            if (!emit_local_param_decode(
                    output,
                    document,
                    abbr,
                    param,
                    base_snake,
                    param_nullable_bit,
                    error,
                    error_capacity)) {



                free(effective);
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

            free(effective);
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

    char abbr[32];
    abbreviate_from_path(document->root_source_path, abbr, sizeof(abbr));


    char types_h[1024];
    char types_c[1024];
    char local_h[1024];
    char local_c[1024];

    if (!build_output_path(types_h, sizeof(types_h), output_directory, base_snake, "_types.h") ||
        !build_output_path(types_c, sizeof(types_c), output_directory, base_snake, "_types.c") ||
        !build_output_path(local_h, sizeof(local_h), output_directory, base_snake, "_local.h") ||
        !build_output_path(local_c, sizeof(local_c), output_directory, base_snake, "_local.c")) {

        set_error(
            error,
            error_capacity,
            "generated output path is too long");

        return false;
    }


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
        abbr,
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
         i < document->resolved_api_count;
         ++i) {

        if (document->resolved_apis[i].name != NULL &&
            strcmp(
                document->resolved_apis[i].name,
                name) == 0) {

            return
                &document->resolved_apis[i];
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




static bool emit_field_declaration_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid field declaration context");
        return false;
    }

    if (name == NULL) {
        set_error(error, error_capacity, "result field has no name");
        return false;
    }

    char base[256];
    bool pointer;

    if (!format_c_type_ctx(
            ctx,
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


static bool emit_field_declaration(
    FILE *output,
    const adsl_document_t *document,
    const char *type,
    const char *name,
    char *error,
    size_t error_capacity) {

    emitter_ctx_t ctx = emitter_ctx_standalone(document);
    return emit_field_declaration_ctx(
        output, &ctx, type, name, error, error_capacity);
}







static bool method_has_result_payload(
    const adsl_method_t *method) {

    if (method == NULL) {
        return false;
    }

    if (method->return_field_count > 0u) {
        return true;
    }

    return method->returns_type != NULL &&
           strcmp(
               method->returns_type,
               "void") != 0;
}



static bool method_returns_void(
    const adsl_method_t *method);



static bool response_emit_result_struct(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *abbr,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    /*
     * A throws-only method and `returns: void` both have a successful
     * response envelope but no success payload.
     */
    if (!method_has_result_payload(method)) {
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
        "} %s_%s_%s_result_t;\n\n",
        abbr,
        api_name,
        method_name);

    return true;
}



static bool response_emit_error_struct(
    FILE *output,
    const adsl_document_t *document,
    const adsl_method_t *method,
    const char *abbr,
    const char *api_name,
    const char *method_name,
    char *error,
    size_t error_capacity) {

    if (method == NULL ||
        method->throws_type == NULL) {

        return true;
    }

    fputs(
        "typedef struct {\n",
        output);

    if (!emit_field_declaration(
            output,
            document,
            method->throws_type,
            "value",
            error,
            error_capacity)) {

        return false;
    }

    fprintf(
        output,
        "} %s_%s_%s_error_t;\n\n",
        abbr,
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


    /*
     * A declared `throws` type is supported by the response decoder. Its
     * payload uses the ordinary ADSL value codec after META_ERROR+request_id.
     */


    /*
     * returns: void is a response-bearing method: the wire format still
     * carries META_RESULT + request_id, but no result payload follows.
     * Java ApiGenerator treats it the same way.
     */
    if (method_returns_void(method)) {
        return true;
    }


    if (!method_has_result_payload(method) &&
        method->throws_type == NULL) {

        set_error(
            error,
            error_capacity,
            "selected response method has no result or throws");

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
    const char *abbr,
    const char *api_name,
    const char *method_name,
    const char *abbr_upper,
    const char *api_upper,
    const char *method_upper,
    bool has_result,
    bool has_typed_error) {

    fprintf(
        output,
        "typedef enum {\n"
        "    %s_%s_%s_RESPONSE_RESULT = 0,\n"
        "    %s_%s_%s_RESPONSE_ERROR = 1\n"
        "} %s_%s_%s_response_kind_t;\n\n",
        abbr_upper, api_upper, method_upper,
        abbr_upper, api_upper, method_upper,
        abbr, api_name, method_name);

    fprintf(
        output,
        "typedef struct {\n"
        "    %s_%s_%s_response_kind_t kind;\n"
        "    uint32_t request_id;\n",
        abbr,
        api_name,
        method_name);

    if (has_result &&
        has_typed_error) {

        /*
         * A response can carry either a result payload or a typed error, never
         * both. Keep them in an anonymous union so MCU RAM is max(result,error)
         * while preserving the existing frame.result source API.
         */
        fprintf(
            output,
            "    union {\n"
            "        %s_%s_%s_result_t result;\n"
            "        %s_%s_%s_error_t error;\n"
            "    };\n",
            abbr,
            api_name,
            method_name,
            abbr,
            api_name,
            method_name);
    } else if (has_result) {
        fprintf(
            output,
            "    %s_%s_%s_result_t result;\n",
            abbr,
            api_name,
            method_name);
    } else if (has_typed_error) {
        fprintf(
            output,
            "    %s_%s_%s_error_t error;\n",
            abbr,
            api_name,
            method_name);
    }

    fprintf(
        output,
        "} %s_%s_%s_response_frame_t;\n\n"
        "aether_status_t %s_%s_%s_response_decode(\n"
        "    aether_meta_read_context_t *ctx,\n"
        "    %s_%s_%s_response_frame_t *frame);\n\n",
        abbr,
        api_name,
        method_name,
        abbr,
        api_name,
        method_name,
        abbr,
        api_name,
        method_name);
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
    char abbr[32];
    char api_name[256];
    char method_name[256];
    char abbr_upper[32];
    char api_upper[256];
    char method_upper[256];

    upper_name(base_name, guard, sizeof(guard));
    snake_name(base_name, base_snake, sizeof(base_snake));
    abbreviate_from_path(document->root_source_path, abbr, sizeof(abbr));
    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));
    upper_name(abbr, abbr_upper, sizeof(abbr_upper));
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



    emit_method_origin(
        output,
        api,
        method);



    bool ok =
        response_emit_result_struct(
            output,
            document,
            method,
            abbr,
            api_name,
            method_name,
            error,
            error_capacity);


    if (ok) {

            response_emit_error_struct(
                output,
                document,
                method,
                abbr,
                api_name,
                method_name,
                error,
                error_capacity);

    }

    if (ok) {

        response_emit_frame_declaration(
            output,
            abbr,
            api_name,
            method_name,
            abbr_upper,
            api_upper,
            method_upper,
            method_has_result_payload(method),
            method->throws_type != NULL);

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
    char abbr[32];
    char api_name[256];
    char method_name[256];
    char abbr_upper[32];
    char api_upper[256];
    char method_upper[256];

    snake_name(base_name, base_snake, sizeof(base_snake));
    abbreviate_from_path(document->root_source_path, abbr, sizeof(abbr));
    snake_name(api->name, api_name, sizeof(api_name));
    c_identifier(method->name, method_name, sizeof(method_name));
    upper_name(abbr, abbr_upper, sizeof(abbr_upper));
    upper_name(api_name, api_upper, sizeof(api_upper));
    upper_name(method_name, method_upper, sizeof(method_upper));



    fprintf(
        output,
        "#include \"%s_response.h\"\n"
        "#include \"aether_meta_runtime.h\"\n\n"
        "#include <string.h>\n\n"

        "aether_status_t %s_%s_%s_response_decode(\n"
        "    aether_meta_read_context_t *ctx,\n"
        "    %s_%s_%s_response_frame_t *frame) {\n\n"
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
        "        frame->kind = %s_%s_%s_RESPONSE_ERROR;\n",
        base_snake,
        abbr, api_name, method_name,
        abbr, api_name, method_name,
        abbr_upper, api_upper, method_upper);


    bool ok = true;

    if (method->throws_type != NULL) {
        ok =

            emit_decode_value_expression_ctx(
                output,
                &(emitter_ctx_t){ .document = document, .resolver = NULL },

                method->throws_type,
                "frame->error.value",
                base_snake,
                0u,

                NULL,

                error,
                error_capacity);
    }

    if (ok) {
        fputs(
            "\n"
            "        return AETHER_OK;\n"
            "    }\n\n"
            "    if (command != 0u) {\n"
            "        return AETHER_ERR_PROTOCOL;\n"
            "    }\n\n",
            output);

        fprintf(
            output,

            "    frame->kind = %s_%s_%s_RESPONSE_RESULT;\n\n",
            abbr_upper,
            api_upper,
            method_upper);

    }

    if (!ok) {
        fclose(output);
        return false;
    }

    ok = true;




    if (!method_has_result_payload(method)) {
        ok = true;

    } else if (method->returns_type != NULL) {

        ok = emit_decode_value_expression_ctx(
            output,
            &(emitter_ctx_t){ .document = document, .resolver = NULL },
            method->returns_type,
            "frame->result.value",
            base_snake,
            0u,
            NULL,
            error,
            error_capacity);

    } else {

        adsl_type_t synthetic = {
            .fields = method->return_fields,
            .field_count = method->return_field_count
        };



        ok = emit_decode_struct_fields_ctx(
            output,
            &(emitter_ctx_t){ .document = document, .resolver = NULL },
            &synthetic,
            "(&frame->result)",
            base_snake,
            0u,
            NULL,
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

    if (!build_output_path(types_h, sizeof(types_h), output_directory, base_snake, "_types.h") ||
        !build_output_path(types_c, sizeof(types_c), output_directory, base_snake, "_types.c") ||
        !build_output_path(response_h, sizeof(response_h), output_directory, base_snake, "_response.h") ||
        !build_output_path(response_c, sizeof(response_c), output_directory, base_snake, "_response.c")) {

        set_error(
            error,
            error_capacity,
            "generated output path is too long");

        return false;
    }


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


static bool emit_decode_struct_fields_recursive_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    size_t *bit_index,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid decode struct fields context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated struct decode depth exceeds 64");
        return false;
    }

    if (type->parent != NULL) {
        const adsl_type_t *parent =
            find_declared_type_ctx(ctx, type->parent);

        if (parent == NULL) {
            set_error(
                error,
                error_capacity,
                "generated struct has unknown parent");
            return false;
        }

        if (!emit_decode_struct_fields_recursive_ctx(
                output,
                ctx,
                parent,
                target,
                base_snake,
                bit_index,
                depth + 1u,
                runtime_depth,
                error,
                error_capacity)) {
            return false;
        }
    }

    for (size_t f = 0u; f < type->field_count; ++f) {
        const adsl_field_t *field = &type->fields[f];

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
            if (!emit_decode_value_expression_ctx(
                    output,
                    ctx,
                    field->type,
                    field_target,
                    base_snake,
                    depth + 1u,
                    runtime_depth,
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
        char ctype_buf[272];

        if (field_is_array) {
            if (!array_alias_name_ctx(
                    ctx,
                    underlying,
                    ctype_buf,
                    sizeof(ctype_buf),
                    error,
                    error_capacity)) {
                if (error != NULL && error[0] == '\0') {
                    set_error(
                        error,
                        error_capacity,
                        "nullable array alias name is too long");
                }
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
                    find_declared_type_ctx(ctx, base);

                if (declared == NULL) {
                    set_error(
                        error,
                        error_capacity,
                        "nullable field has unknown type");
                    return false;
                }

                char declared_name[256];

                if (!declared_type_c_name_ctx(
                        ctx,
                        declared,
                        declared_name,
                        sizeof(declared_name),
                        error,
                        error_capacity)) {
                    return false;
                }


                if (declared->enum_count > 0u) {
                    snprintf(
                        ctype_buf,
                        sizeof(ctype_buf),
                        "%s_t",
                        declared_name);
                    ctype = ctype_buf;
                } else if (type_is_polymorphic_reference_ctx(ctx, declared)) {
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

        if (!emit_decode_value_expression_ctx(
                output,
                ctx,
                underlying,
                nullable_target,
                base_snake,
                depth + 1u,
                runtime_depth,
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




static bool emit_decode_struct_fields_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid decode struct fields context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated struct decode depth exceeds 64");
        return false;
    }

    size_t nullable_count = 0u;

    if (!count_all_nullable_fields_ctx(
            ctx,
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
        emit_decode_struct_fields_recursive_ctx(
            output,
            ctx,
            type,
            target,
            base_snake,
            &bit_index,
            depth,
            runtime_depth,
            error,
            error_capacity);

    if (!fields_ok) {
        return false;
    }

    fprintf(output, "    }\n");

    return true;
}





static bool emit_decode_array_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid decode array context");
        return false;
    }

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

        if (!emit_decode_value_expression_ctx(
                output,
                ctx,
                element,
                elem_target,
                base_snake,
                depth + 1u,
                runtime_depth,
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
    char ctype_buf[272];

    if (ctype == NULL) {
        const adsl_type_t *declared =
            find_declared_type_ctx(ctx, element);

        if (declared == NULL) {
            set_error(
                error,
                error_capacity,
                "unknown dynamic array element type");
            return false;
        }

        char declared_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                declared,
                declared_name,
                sizeof(declared_name),
                error,
                error_capacity)) {
            return false;
        }


        if (declared->enum_count > 0u) {
            snprintf(
                ctype_buf,
                sizeof(ctype_buf),
                "%s_t",
                declared_name);
            ctype = ctype_buf;
        } else if (type_is_polymorphic_reference_ctx(ctx, declared)) {
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

    if (!emit_decode_value_expression_ctx(
            output,
            ctx,
            element,
            array_target,
            base_snake,
            depth + 1u,
            runtime_depth,
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




static bool emit_decode_hierarchy_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const adsl_type_t *static_type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid decode hierarchy context");
        return false;
    }

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

    size_t hierarchy_count = ctx_hierarchy_type_count(ctx);

    size_t concrete_count = 0u;

    for (size_t i = 0u; i < hierarchy_count; ++i) {
        const adsl_type_t *candidate = ctx_hierarchy_type_at(ctx, i);

        if (candidate->enum_count > 0u ||
            candidate->is_abstract ||
            !type_is_descendant_or_self_ctx(ctx, candidate, static_type)) {
            continue;
        }

        if (!candidate->has_id || candidate->id == 0u) {
            continue;
        }

        char candidate_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                candidate,
                candidate_name,
                sizeof(candidate_name),
                error,
                error_capacity)) {
            return false;
        }


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

        if (!emit_decode_struct_fields_ctx(
                output,
                ctx,
                candidate,
                child_target,
                base_snake,
                depth + 1u,
                runtime_depth,
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




static bool emit_decode_value_expression_ctx(
    FILE *output,
    const emitter_ctx_t *ctx,
    const char *type,
    const char *target,
    const char *base_snake,
    unsigned int depth,
    const char *runtime_depth,
    char *error,
    size_t error_capacity) {

    if (ctx == NULL || ctx->document == NULL) {
        set_error(error, error_capacity, "invalid decode value context");
        return false;
    }

    if (depth > 64u) {
        set_error(
            error,
            error_capacity,
            "generated decode depth exceeds 64");
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
        return emit_decode_array_expression_ctx(
            output,
            ctx,
            type,
            target,
            base_snake,
            depth,
            runtime_depth,
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
        find_declared_type_ctx(ctx, base);

    if (declared_type == NULL) {
        set_error(
            error,
            error_capacity,
            "unknown generated decode type");
        return false;
    }

    if (declared_type->has_tiered_int) {
        char tiered_name[256];

        if (!declared_type_c_name_ctx(
                ctx,
                declared_type,
                tiered_name,
                sizeof(tiered_name),
                error,
                error_capacity)) {
            return false;
        }


        fprintf(
            output,
            "    {\n"
            "        uint64_t _meta_tiered_int = 0u;\n"
            "        if (!aether_meta_read_tiered_int(\n"
            "                &ctx->reader,\n"
            "                &_meta_tiered_int,\n"
            "                %uu,\n"
            "                %s_tiered_int_limits,\n"
            "                %zuu)) {\n"
            "            return AETHER_ERR_PROTOCOL;\n"
            "        }\n"
            "        (%s) = (%s_t)_meta_tiered_int;\n"
            "    }\n",
            (unsigned int)declared_type->tiered_int_min_bytes,
            tiered_name,
            declared_type->tiered_int_limit_count,
            target,
            tiered_name);

        return true;
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

        if (!declared_type_c_name_ctx(
                ctx,
                declared_type,
                enum_name,
                sizeof(enum_name),
                error,
                error_capacity)) {
            return false;
        }


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

    if (type_is_polymorphic_reference_ctx(ctx, declared_type)) {
        return emit_decode_hierarchy_expression_ctx(
            output,
            ctx,
            declared_type,
            target,
            base_snake,
            depth,
            runtime_depth,
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

    char nested_name[256];

    declared_c_name(
        "",
        declared_type->name,
        nested_name,
        sizeof(nested_name));

    /*
     * See emit_value_expression_ctx: the nested type's decode symbols are
     * prefixed by its own declaring file, not by the caller's base_snake.
     */
    char nested_prefix[32];

    if (!type_effective_prefix_ctx(
            ctx,
            declared_type,
            nested_prefix,
            sizeof(nested_prefix),
            error,
            error_capacity)) {
        return false;
    }






    /*
     * Standalone wrappers sometimes construct a context without
     * module_abbr. Snapshot the context and fill it from the caller's
     * authoritative prefix before calling the strict owner helper.
     * In project mode module_abbr is already set, so this is a no-op.
     */
    emitter_ctx_t effective_ctx = *ctx;

    if (effective_ctx.module_abbr[0] == '\0' && base_snake != NULL) {
        (void)snprintf(
            effective_ctx.module_abbr,
            sizeof(effective_ctx.module_abbr),
            "%s",
            base_snake);
    }
    if (effective_ctx.base_snake[0] == '\0' && base_snake != NULL) {
        (void)snprintf(
            effective_ctx.base_snake,
            sizeof(effective_ctx.base_snake),
            "%s",
            base_snake);
    }


    char nested_owner[256];

    if (!type_owner_base_snake(
            &effective_ctx, declared_type, nested_owner,
            sizeof(nested_owner), error, error_capacity)) {
        return false;
    }

    /* See emit_value_expression_ctx: unified mode is always a local call. */
    bool nested_in_same_tu =
        ctx->resolver == NULL ||
        strcmp(nested_owner, effective_ctx.base_snake) == 0;



    if (runtime_depth != NULL) {
        /*
         * See emit_value_expression_ctx for the rationale: local types
         * keep calling the static impl, external types go through the
         * owner's *_with_depth entry point, and the caller adds one to
         * the recursion depth in both cases.
         */
        if (nested_in_same_tu) {
            fprintf(
                output,
                "    {\n"
                "        aether_status_t meta_status = %s_deserialize_%s_impl(\n"
                "            ctx,\n"
                "            &(%s),\n"
                "            %s + 1u);\n"
                "        if (meta_status != AETHER_OK) return meta_status;\n"
                "    }\n",
                nested_prefix,
                nested_name,
                target,
                runtime_depth);
        } else {
            fprintf(
                output,
                "    {\n"
                "        aether_status_t meta_status = %s_deserialize_%s_with_depth(\n"
                "            ctx,\n"
                "            &(%s),\n"
                "            %s + 1u);\n"
                "        if (meta_status != AETHER_OK) return meta_status;\n"
                "    }\n",
                nested_owner,
                nested_name,
                target,
                runtime_depth);
        }
    } else {
        fprintf(
            output,
            "    {\n"
            "        aether_status_t meta_status = %s_deserialize_%s(\n"
            "            ctx,\n"
            "            &(%s));\n"
            "        if (meta_status != AETHER_OK) return meta_status;\n"
            "    }\n",
            nested_owner,
            nested_name,
            target);
    }


    return true;
}