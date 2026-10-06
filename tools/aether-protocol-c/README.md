
# Aether ADSL -> C Generator Contract

This document is the **public contract of `aether-protocol-c`**.

Its purpose is to let a developer or AI use generated Aether C bindings without
reading `adsl.c`, `emitter.c`, or thousands of lines of generated output.

When this document and generated output disagree, treat that as a generator bug:
fix the generator and this specification together. Do not manually patch generated
bindings.

The C target is designed for MCU firmware:

- generated firmware code does not require the ADSL schema at runtime;
- generated/runtime code does not allocate from the heap;
- TX buffers, RX buffers, decode scratch, collapse tables and stream storage are
  caller-owned;
- generated code is intended to be friendly to `-Os` / LTO and normal C compiler
  optimization.

The **generator executable itself is a development-time tool** and may allocate
memory while parsing ADSL. That memory is never linked into firmware.

---

## 1. The fastest way to find generated C from ADSL

Generated public declarations contain grep-friendly origin comments.

For a declared type:

```c
/* ADSL: path/to/File.adsl.yaml:123 types.PeerHello */
```

For an API method:

```c
/* ADSL: path/to/File.adsl.yaml:456 api.PeerApi.methods.hello */
```

The path is the physical file from which the declaration was loaded, including
declarations brought in through include:. The line is the declaration line in
that file.
Therefore the preferred lookup workflow is:
grep -R "types.PeerHello" generated/
grep -R "api.PeerApi.methods.hello" generated/

Do this before inspecting generator code.
For inherited APIs the comment identifies the API that actually declares the
method, not merely the child API through which the method is exposed.
## 2. CLI
Executable:
aether-adslc

General form:
aether-adslc \
    [-I <include-dir>]... \
    [--api ApiName |
     --method ApiName.methodName |
     --local-api ApiName |
     --response-method ApiName.methodName] \
    <input.adsl.yaml> \
    <output-dir> \
    [base-name]

Only one selection mode may be used at a time.
No selector
Generates normal remote bindings for the loaded document:
<base>_types.h
<base>_types.c
<base>_api.h
<base>_api.c

--api ApiName
Generates remote bindings restricted to the selected effective API.
Output file names are still:
<base>_types.h
<base>_types.c
<base>_api.h
<base>_api.c

--method ApiName.methodName
Generates one selected remote method and its type support.
Output file names are again:
<base>_types.h
<base>_types.c
<base>_api.h
<base>_api.c

--local-api ApiName
Generates an inbound/local dispatcher:
<base>_types.h
<base>_types.c
<base>_local.h
<base>_local.c

--response-method ApiName.methodName
Generates the decoder for the result/error envelope of one response-bearing
method:
<base>_types.h
<base>_types.c
<base>_response.h
<base>_response.c

Include paths
Use one or more -I options when included ADSL files are not resolvable from the
normal source-relative locations:
aether-adslc \
    -I /path/to/common-api/src/main/resources \
    PeerApi.adsl.yaml \
    generated

Includes are recursively loaded and merged before semantic validation and C
generation.
## 3. Base name and generated file names
If [base-name] is omitted, the input file name is used after removing:
.adsl.yaml

or:
.adsl.yml

Example:
ClientServerApi.adsl.yaml

has logical base name:
ClientServerApi

The generated C file stem is the snake-case form:
client_server_api

so normal generation produces:
client_server_api_types.h
client_server_api_types.c
client_server_api_api.h
client_server_api_api.c

An explicit [base-name] is useful when several selected generations are emitted
from the same source schema.
## 4. C naming rules
### 4.1 Snake-case conversion
Protocol type/API/method names used in generated symbol prefixes are converted to
lower snake case.
Examples:
PeerHello              -> peer_hello
ServerRegistrationApi  -> server_registration_api
registrationDirect     -> registration_direct
IPAddressV4            -> i_p_address_v4
SignHYDROGEN           -> sign_h_y_d_r_o_g_e_n

Uppercase letters are treated individually. Acronyms therefore intentionally
produce names such as i_p_address_v4; do not guess acronym folding.
Non-alphanumeric separators become _.
### 4.2 Declared ADSL type names
A declared type normally becomes:
<TypeName> -> <type_name>_t

Example:
PeerHello -> peer_hello_t

A declared name whose generated form starts with the reserved runtime prefix
aether_ receives an adsl_ prefix so generated schema types cannot collide
with handwritten runtime types.
### 4.3 Fields and parameter names
Field and parameter identifiers are emitted from the schema name and should be
valid C identifiers. Schema authors should not rely on the generator to rename
arbitrary field/parameter names.
### 4.4 C keywords
Some generated identifier positions use a keyword-safe form in which a C keyword
gets a trailing _.
Schema authors should nevertheless avoid C keywords for field and parameter names.
## 5. Primitive ADSL -> C ABI mapping
ADSL	C representation
bool, boolean	bool
byte	int8_t
short	int16_t
int	int32_t
long	int64_t
intpack	int64_t
date	int64_t
float	float
double	double
uuid	aether_uuid_t
string	aether_bytes_view_t
uri	aether_bytes_view_t
byte[]	aether_bytes_view_t


string and uri are byte views in the C ABI. The generator does not create
NUL-terminated C strings.
aether_bytes_view_t is a pointer + length view. On decode it normally borrows
bytes directly from the RX frame.
## 6. Arrays
Dynamic arrays
ADSL:
T[]

normally becomes:
typedef struct {
    const T *data;
    size_t length;
} <t>_array_view_t;

Example:
CryptoLib[] -> crypto_lib_array_view_t

byte[] is special and reuses:
aether_bytes_view_t

Dynamic arrays are encoded as:
packed element count
element 0
element 1
...

For byte[], the packed count is followed directly by the raw bytes.
On decode, non-byte dynamic arrays require caller-provided scratch storage for
their element array. byte[] can remain a borrowed RX view.
Fixed arrays
ADSL:
T[N]

becomes an inline wrapper:
typedef struct {
    T data[N];
} <t>_array_<N>_t;

Example:
byte[32] -> byte_array_32_t

Fixed arrays do not carry a wire length; exactly N elements are emitted.
A root fixed-array API parameter is pointer-shaped so large MCU arrays are not
copied onto the generated dispatcher stack.
## 7. Enums
A declared ADSL enum becomes a C enum:
MyEnum -> my_enum_t

Enumerators use the generated type name plus an uppercase value name.
Example:
typedef enum {
    crypto_lib_HYDROGEN = 0,
    crypto_lib_SODIUM = 1
} crypto_lib_t;

The exact names come from the schema.
Wire representation is one byte containing the Java-compatible ordinal.
Enums do not receive standalone public
<base>_serialize_<enum>() / <base>_deserialize_<enum>() functions.
Their codec is emitted inline wherever the enum is used.
## 8. Named tieredInt
A named tieredInt is represented as:
typedef uint64_t <name>_t;

Generated schema constants contain the configured tier limits:
extern const uint32_t <name>_tiered_int_limits[...];

The actual wire algorithm is shared in the handwritten runtime:
aether_meta_write_tiered_int(...)
aether_meta_read_tiered_int(...)

The generated code supplies only the schema-specific minTierBytes and limits.
Values are restricted to the Java-compatible signed-long domain
(<= INT64_MAX).
A named tieredInt does not get a standalone public generated type codec.
## 9. Concrete structs
A normal declared object type:
types:
  PeerFrame:
    fields:
      connectionId: uuid
      type: PeerFrameType
      payload: byte[]

becomes approximately:
typedef struct peer_frame_t peer_frame_t;

struct peer_frame_t {
    aether_uuid_t connectionId;
    peer_frame_type_t type;
    aether_bytes_view_t payload;
};

The exact C representation of every field follows the rules in this document.
Public serializer/deserializer lookup
For a normal concrete struct the public codec names are:
<base_snake>_serialize_<type_snake>
<base_snake>_deserialize_<type_snake>

Example for base PeerApi and type PeerHello:
aether_status_t peer_api_serialize_peer_hello(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_hello_t *value);

aether_status_t peer_api_deserialize_peer_hello(
    aether_meta_read_context_t *ctx,
    peer_hello_t *value);

To find them without knowing the base name:
grep -R "types.PeerHello" generated/

The origin comment is emitted immediately beside the public codec declarations.
Serializer transaction rule
The public serializer receives a caller-owned output buffer and position.
Generated serializers use a local position internally and commit the caller's
position only on success. A failed serialization therefore does not expose a
partially advanced public position.
## 10. Struct inheritance
ADSL inheritance is resolved at generation time.
Inherited fields participate in the same serialized object in parent-first
order. C firmware does not need runtime schema reflection.
Nullable-mask handling includes inherited nullable fields in the same logical
object.
Invalid non-pointer by-value cycles are rejected during validation. Recursive
shapes that are representable through generated pointer/reference forms are
supported with a recursion limit.
Default runtime recursion guard:
AETHER_META_MAX_RECURSION_DEPTH

## 11. Polymorphic type hierarchies
An abstract type, or another declared type that acts as the root of concrete
descendants, is represented as a tagged C reference:
typedef struct {
    uint8_t type_id;
    union {
        const concrete_a_t *concrete_a;
        const concrete_b_t *concrete_b;
    } as;
} root_ref_t;

Concrete hierarchy children must have a non-zero one-byte ADSL id.
Generated type-id constants follow the pattern:
<ROOT_UPPER>_TYPE_<CONCRETE_UPPER>

Example:
KEY_TYPE_HYDROGEN_SIGN_PUBLIC

Wire representation is:
1-byte concrete type id
concrete payload

Public hierarchy codec lookup
The public hierarchy codec is named exactly like a concrete struct codec, but its
value type is <root>_ref_t:
<base>_serialize_<root>(
    ...,
    const <root>_ref_t *value);

<base>_deserialize_<root>(
    ...,
    <root>_ref_t *value);

Again, search:
grep -R "types.<RootName>" generated/

A concrete leaf's own struct serializer serializes the leaf payload only. Use the
hierarchy/root serializer when the one-byte polymorphic type discriminator is
part of the required wire value.
## 12. Nullable values
ADSL suffix:
?

marks a nullable value.
Nullable fields and nullable API parameters are represented with pointer-shaped
C ABI where necessary. NULL means absent.
Nullable values are not encoded with a marker immediately before every
field. A containing struct/method emits an aggregate nullable bit mask first.
Mask width is:
Nullable values	Mask bytes
1..8	1
9..16	2
17..32	4
33..64	8


A set bit means the corresponding value is NULL.
Non-null values then serialize their normal underlying wire form.
More than 64 nullable parameters in one API method are rejected.
## 13. Collapsible ~
The suffix:
~

does not change the ordinary field wire representation. Outside collapsible
method semantics it uses the underlying type.
For an API METHOD containing ~ parameters, the generator emits caller-owned
collapse state. Calls with the same non-~ key replace the retained ~ values
instead of appending another logical call.
Important MCU properties:
- no heap;
- caller supplies the retained entry table;
- insertion order of distinct keys is preserved;
- updating an existing key keeps its original FIFO position;
- flush serializes pending logical calls;
- shared collapse queues can preserve FIFO order across methods.
The current exact key-comparison subset is intentionally limited to the types
needed by the implementation:
- integer-like scalar values;
- enum;
- named tieredInt;
- UUID;
- String byte content;
- float/double using Java bit semantics.

URI keys are intentionally rejected for collapsible METHODs because the current
C byte-view representation cannot reproduce `java.net.URI.equals` exactly.
Complex DTO/ref/array key expansion is intentionally deferred because production
schemas currently do not use collapsible methods.
Do not confuse this restriction with ordinary array support. T[] and T[N]
remain fully supported in normal structs and APIs.
## 14. Remote API bindings
For:
api:
  ServerRegistrationApi:
    methods:
      registrationDirect:
        ...

the remote C type is:
server_registration_api_remote_t

and an ordinary method function is:
server_registration_api_registration_direct(...)

General formula:
<api_snake>_remote_t
<api_snake>_<method_snake>

Remote state contains:
void *send_ctx;
<base>_send_fn send;
uint8_t *tx;
size_t tx_capacity;

tx is caller-owned. Generated code serializes one complete API frame into it
and then invokes:
send(send_ctx, tx, length)

The generator does not own a network transport.
Request frame
An ordinary outgoing method frame is:
1 byte command id
[4 byte little-endian request id, only for response-bearing methods]
[nullable parameter mask, if needed]
parameters in ADSL order

A method is response-bearing if it has a return value, structured result, or
typed throws.
Method IDs and API inheritance
C API inheritance is flattened at generation time using Java-compatible
parent-first ordering.
Effective methods are collected:
1. parents recursively, in declared order;
2. then the API's own methods;
3. duplicate method names keep the first encountered method.
Explicit method IDs are respected.
For methods without an explicit ID the contextual command ID follows the Java
generator rule equivalent to:
effective-method-index + 3

Because that ID is contextual, the same inherited method can have a different
automatic ID when reached through another effective API.
The generated origin comment points to the API that declares the method.
## 15. How to find a remote C method from YAML
Given:
api.ServerRegistrationApi.methods.registrationDirect

first search:
grep -R \
  "api.ServerRegistrationApi.methods.registrationDirect" \
  generated/

You should land beside declarations/definitions such as:
server_registration_api_registration_direct(...)

You do not need to inspect emitter.c.
If only this method is needed, it may be generated directly:
aether-adslc \
    --method ServerRegistrationApi.registrationDirect \
    ClientServerApi.adsl.yaml \
    generated

## 16. Local/inbound API bindings
Generate an inbound dispatcher with:
aether-adslc \
    --local-api PeerApi \
    PeerApi.adsl.yaml \
    generated

Public callback typedef formula:
<api_snake>_<method_identifier>_fn

Local table:
<api_snake>_local_t

Dispatcher:
<api_snake>_dispatch

Conceptually:
typedef aether_status_t (*peer_api_hello_fn)(
    void *ctx,
    ...decoded parameters...);

typedef struct {
    void *ctx;
    peer_api_hello_fn hello;
    ...
} peer_api_local_t;

aether_status_t peer_api_dispatch(
    peer_api_local_t *api,
    aether_meta_read_context_t *ctx);

The dispatcher consumes one command from ctx->reader. Reuse the same read
context to process another command concatenated in the same RX buffer.
For a response-bearing method the callback also receives the decoded
uint32_t request_id.
Generated ADSL: comments are emitted beside the callback declaration and beside
the matching dispatcher case, so searching the YAML method path leads directly
to the inbound handling path.
## 17. Response decoder
Generate a response decoder with:
aether-adslc \
    --response-method ServerRegistrationApi.registrationDirect \
    ClientServerApi.adsl.yaml \
    generated

The principal public symbols are:
<api>_<method>_response_kind_t
<api>_<method>_response_frame_t
<api>_<method>_response_decode

Example shape:
aether_status_t
server_registration_api_registration_direct_response_decode(
    aether_meta_read_context_t *ctx,
    server_registration_api_registration_direct_response_frame_t *frame);

Response envelope:
1 byte response command
4 byte little-endian request id
payload

Response command:
0 = result
1 = error

If both a result and typed error exist, the generated frame stores them in a C
union so RAM usage is max(result,error) instead of their sum.
The response header carries the same ADSL: method-origin comment used by normal
remote bindings.
## 18. Streams
A generated stream is a caller-owned byte builder/view:
typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} <stream>_t;

When used for outbound building:
- data points to caller storage;
- capacity is total writable storage;
- length is the currently committed encoded byte count.
For a stream bound to an API, generated builder functions follow:
<stream_name>_<api_name>_<method_name>

Example conceptual name:
login_client_stream_authorized_api_ping(...)

The builder serializes the normal API method frame into the stream storage rather
than sending it to a transport.
Collapsible stream methods can additionally generate:
<stream>_<api>_<method>_flush

Every public stream method wrapper receives the same
api.<Api>.methods.<method> origin comment as the underlying API method.
Inbound decoded streams generally borrow their payload from the RX frame. Their
contents must not be retained past the documented callback lifetime unless copied
by the application.
## 19. Decode context and noheap ownership
All generated decoders use:
aether_meta_read_context_t

which contains:
aether_meta_reader_t reader;

uint8_t *scratch;
size_t scratch_capacity;
size_t scratch_position;

Scratch is a caller-owned linear bump arena.
No generated decoder calls malloc.
Scratch is used when decoding requires materialized C objects, including cases
such as:
- dynamic non-byte arrays;
- polymorphic concrete hierarchy objects;
- pointer-shaped nullable decoded values;
- root concrete structs used by local dispatch;
- root fixed arrays used by local dispatch.
Borrowed RX views, especially byte[], string, uri and stream payload bytes,
normally do not require copying.
Lifetime
Pointers returned from scratch remain valid until the caller resets
scratch_position or otherwise reuses/discards the decode context.
Pointers/views into RX remain valid only while the underlying RX buffer remains
valid.
The local dispatcher treats decoded pointer/view parameters as callback-lifetime
values. An asynchronous application must copy anything it wants to retain before
the callback returns.
## 20. Wire rules relevant when debugging interoperability
These rules match the Java FastMeta generator.
Fixed-width integers
short, int, long, date and fixed-width integer pieces use explicit
little-endian wire order.
Never serialize host integer memory with memcpy.
float and double
Wire bits use Java-compatible canonicalization semantics, including Java's
canonical NaN behavior and distinct +0.0 / -0.0.
Shared runtime helpers are used so ordinary serialization and collapsible-key
comparison agree.
UUID
aether_uuid_t contains:
uint64_t msb;
uint64_t lsb;

Wire form is the canonical 16 UUID bytes:
MSB bytes first, then LSB bytes

Do not replace UUID encoding with memcpy(aether_uuid_t).
intpack
Uses Aether/FastMeta packed integer encoding, not protobuf/LEB128.
String / URI / byte[]
Wire form:
packed byte length
raw bytes

Dynamic non-byte array
Wire form:
packed element count
encoded elements

Fixed array
Wire form is exactly the encoded elements with no length prefix.
Enum
One byte ordinal.
Polymorphic hierarchy
One byte concrete type ID followed by the selected concrete payload.
Struct nullable values
Aggregate nullable mask first, then all non-null field payloads in schema order.
API request
command id
optional request id
optional nullable mask
parameters

## 21. Which ADSL types have standalone public codecs?
This distinction is important.
Public generated serializer/deserializer exists
For:
- normal concrete struct/object types;
- polymorphic hierarchy reference types.
Formula:
<base>_serialize_<type>
<base>_deserialize_<type>

No standalone public generated codec
Normally no standalone generated function exists for:
- primitive scalars;
- enum;
- tieredInt;
- raw array aliases;
- string/URI/byte view;
- stream view/builder type.
Those codecs are emitted inline at their containing DTO/API call site or use a
shared runtime primitive helper.
Therefore, if an AI cannot find:
<base>_serialize_crypto_lib

for an enum CryptoLib, that is expected. It should not search the generator for
a missing function.
## 22. Includes and physical source origin
The loader recursively resolves included ADSL files and merges declarations into
one semantic document before validation.
Every merged type/API/method retains its physical declaration origin.
That is why a generated file from:
IncludeRoot.adsl.yaml

can legitimately contain:
/* ADSL: CommonTypes.adsl.yaml:... types.SensorReading */

This behavior is intentional and is specifically designed to make generated code
navigable without examining include resolution internals.
## 23. Anonymous wire-visible types
Do not use anonymous wire-visible return DTOs for the C target.
Use a named ADSL type instead.
Named types provide:
- stable schema identity;
- predictable C names;
- reusable serializers/deserializers;
- reliable source-origin navigation;
- cleaner Java/C parity.
Production registration/recovery schemas use named result types for this reason.
## 24. Deferred/unsupported constructs
The following are intentionally not part of the current production C contract
unless a real schema starts requiring them:
- multiplexor;
- syncmap;
- expanded complex collapsible-METHOD key equality;
- anonymous wire-visible DTOs.
ADSL constants are also not a current C-generator requirement.
Do not add speculative implementation for these features merely to imitate every
historical Java-generator capability. Production schema usage and MCU cost are
the priority.
## 25. Practical AI workflow
When asked to use an ADSL API or type, follow this order.
A. Find the schema declaration
Search the .adsl.yaml files for the exact API/method/type name.
B. Find generated symbols from the origin tag
For a type:
grep -R "types.TypeName" generated/

For a method:
grep -R "api.ApiName.methods.methodName" generated/

C. Apply naming formulas from this README
Typical symbols are immediately predictable:
TypeName
    -> type_name_t

ApiName
    -> api_name_remote_t

ApiName.methodName
    -> api_name_method_name(...)

TypeName public DTO codec
    -> <base>_serialize_type_name(...)
    -> <base>_deserialize_type_name(...)

local ApiName
    -> api_name_local_t
    -> api_name_dispatch(...)

response ApiName.methodName
    -> api_name_method_name_response_decode(...)

stream StreamName + ApiName.methodName
    -> stream_name_api_name_method_name(...)

D. Read only the small public declaration you need
Usually the relevant generated header declaration plus this README is enough.
Do not read the whole generated .c file just to discover naming or
serialization conventions.
Do not read emitter.c unless changing the generator itself.
## 26. Verified Java/C parity
The current generator has production Java<->C wire goldens covering:
- PeerFrame:
  UUID + enum + dynamic byte[];
- PeerHello:
  nested DTOs, polymorphic key/signature hierarchies, fixed byte arrays,
  UUID and dynamic enum array;
- ServerRegistrationApi.registrationDirect:
  method id, little-endian request id, strings, int[], UUID and polymorphic
  Key;
- ServerRegistrationApi.recovery:
  method id, request id, UUID and two polymorphic Key values.
The generated C frames matched the Java generator byte-for-byte.
This README therefore documents the behavior that is validated against the Java
wire implementation, not an independently invented C protocol.
## 27. One concrete lookup example
Given this ADSL method:
ServerRegistrationApi.registrationDirect

the normal remote C function is:
server_registration_api_registration_direct(...)

Its generated declaration is preceded by a comment of the form:
/* ADSL: .../ClientServerApi.adsl.yaml:<line>
 * conceptually: api.ServerRegistrationApi.methods.registrationDirect
 */

(the actual generated comment is one line).
The wire request starts with method command id 7. Because the method has a
response, the next four bytes are the little-endian request_id.
A production Java/C golden confirmed the complete 75-byte frame is identical on
both generators.
For a type such as:
PeerHello

generated from base PeerApi, use:
peer_hello_t

peer_api_serialize_peer_hello(...)
peer_api_deserialize_peer_hello(...)

or simply search:
grep -R "types.PeerHello" generated/

That is the intended usage model of this generator.
