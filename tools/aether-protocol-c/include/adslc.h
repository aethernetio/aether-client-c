

/*
- Aether ADSL -> C compiler interface.
-
- The compiler has two deliberately separate phases:
-
-
  1. adsl.c parses YAML-like ADSL files, recursively resolves includes and
- builds one merged semantic document.
-
  2. emitter.c validates/selects APIs from that document and writes static C
- bindings with no runtime schema/reflection dependency.
-
- The data structures below are the small semantic model shared by those
- phases. Strings and arrays are owned by adsl_document_t and are released by
- adsl_document_free().
-
- Generated production files are outputs. Generator defects should be fixed in
- this compiler and covered by a focused fixture/test rather than patched in
- generated C manually.
   */
#ifndef AETHER_ADSLC_H
#define AETHER_ADSLC_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* One named field inside a declared ADSL type or structured return value. */
typedef struct {

    char *name;
    char *type;
} adsl_field_t;

typedef struct {
    char *api;
    char *remote_api;
} adsl_stream_api_t;

typedef struct {
    char *api;
    char *remote_api;

    bool has_crypto;
    bool crypto;

    adsl_stream_api_t *apis;
    size_t api_count;

    adsl_field_t *keys;
    size_t key_count;
} adsl_stream_t;


/*
- Declared ADSL type.
-
- A type can represent a struct, enum, inheritance node, abstract hierarchy
- root, or stream declaration. parent names the inherited ADSL type when one
- exists. The emitter resolves hierarchy relationships against the complete
- merged document rather than storing C-specific information here.
   */
  typedef struct {

    char *name;
    bool has_id;
    uint8_t id;
    bool is_abstract;
    char *parent;

    adsl_field_t *fields;
    size_t field_count;

    char **enum_values;
    size_t enum_count;

    adsl_stream_t *stream;
} adsl_type_t;

typedef struct {
    char *name;
    char *type;
    adsl_stream_t *stream;
} adsl_param_t;


typedef struct {
    char *name;
    bool has_id;
    uint8_t id;

    adsl_param_t *params;
    size_t param_count;

    char *returns_type;
    adsl_field_t *return_fields;
    size_t return_field_count;

    char *throws_type;
} adsl_method_t;


typedef struct {
    char *name;

    char **parents;
    size_t parent_count;

    adsl_method_t *methods;
    size_t method_count;
} adsl_api_t;


/*
- Fully loaded semantic document.
-
- includes records source-level dependency names, while types and apis
- contain the merged declarations visible to generation after recursive include
- loading.
   */
  typedef struct {

    char *name;

    char **includes;
    size_t include_count;

    adsl_type_t *types;
    size_t type_count;

    adsl_api_t *apis;
    size_t api_count;
} adsl_document_t;


void adsl_document_init(
    adsl_document_t *document);

void adsl_document_free(
    adsl_document_t *document);



/*
- Parse only one physical file into a document.
-
- Most CLI/production callers should use adsl_load_file*() instead so include
- dependencies are resolved and merged before validation.
   */
  bool adsl_parse_file(
   const char *path,
   adsl_document_t *document,
   char *error,
   size_t error_capacity);
/*
- Load an ADSL dependency closure using default resolution rules.
   */
  bool adsl_load_file(
   const char *path,
   adsl_document_t *document,
   char *error,
   size_t error_capacity);
/*
- Load an ADSL dependency closure with additional include search roots.
-
- Included declarations are merged into one semantic document. Cycles and
- duplicate physical files are suppressed by the loader.
   */
  bool adsl_load_file_with_include_paths(
   const char *path,
   const char *const *include_paths,
   size_t include_path_count,
   adsl_document_t *document,
   char *error,
   size_t error_capacity);




/*
- Validate cross-reference and wire-model constraints after dependency loading.
- Validation errors are returned as human-readable text in error.
   */
  bool adsl_validate(
   adsl_document_t *document,
   char *error,
   size_t error_capacity);





/*
- Emit one selected remote method and the type support required by that method.
   */
  bool adsl_emit_c_method(
   const adsl_document_t *document,
   const char *output_directory,
   const char *base_name,
   const char *selected_api_name,
   const char *selected_method_name,
   char *error,
   size_t error_capacity);
/*
- Emit a local dispatcher for one selected API.
   */
  bool adsl_emit_c_local_api(
   const adsl_document_t *document,
   const char *output_directory,
   const char *base_name,
   const char *selected_api_name,
   char *error,
   size_t error_capacity);
/*
- Emit a decoder for a selected method's result/error response envelope.
   */
  bool adsl_emit_c_response_method(
   const adsl_document_t *document,
   const char *output_directory,
   const char *base_name,
   const char *selected_api_name,
   const char *selected_method_name,
   char *error,
   size_t error_capacity);
/*
- Emit all C bindings represented by the supplied semantic document.
   */
  bool adsl_emit_c(
   const adsl_document_t *document,
   const char *output_directory,
   const char *base_name,
   char *error,
   size_t error_capacity);



#endif