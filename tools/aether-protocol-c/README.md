
# ADSL C code generator (`aether-adslc`)

`aether-adslc` compiles ADSL protocol schemas into a single C header/source
pair. It is a development-time tool and is never linked into client firmware.

## Output

The generator always emits exactly **two files** into the output directory:

- `aether_api.h`
- `aether_api.c`

Existing files with these names are overwritten on every run.

## Input model

The generator accepts a flat list of files and directories. There are no
"roots", no search paths, and no separate standalone/multi-module modes.

- A **file** input is loaded as a single document.
- A **directory** input is scanned recursively for `*.adsl.yaml`.
- Every document becomes a peer in one flat pool keyed by its basename
  (the file name without the `.adsl.yaml` / `.adsl.yml` suffix).
- `include:` entries name **only the basename of the target document**
  (no path, no extension) and are resolved inside that pool.
- If an include names a document that is not present in the pool, the
  build fails with a clear error.
- Several files may share the same basename as long as their declared
  types and APIs do not collide. Collisions of a type/API name with
  differing contents are reported as errors.

## Symbol prefixes

Every symbol generated from a document carries an abbreviation of that
document's basename as its C prefix:

- `ClientServerApi.adsl.yaml` → `csa_`
- `PeerApi.adsl.yaml` → `pa_`
- `common.adsl.yaml` → `common_`
- `clienttypes.adsl.yaml` → `clienttypes_`

The prefix is applied to **all** symbols originating from that file:
struct/enum/typedef names, serializer/deserializer functions, API remote
structs and API entry points. Stream-builder entry points are additionally
namespaced by the stream type name (for example
`csa_login_stream_authorized_api_back_id(...)`) so that they cannot collide
with the plain API function of the same method.

## Command line
aether-adslc --input <path> [--input <path>...] <output-dir>

text

- `--input <path>` may be repeated any number of times.
- `<path>` may be a file or a directory.
- `<output-dir>` is the last positional argument and is created if missing.

Positional inputs are also accepted: every positional except the last is
treated as an input path, and the last positional is the output directory.

### Example

```sh
aether-adslc \
  --input /path/to/client-server-work-api/ClientServerApi.adsl.yaml \
  --input /path/to/aether-client-java/PeerApi.adsl.yaml \
  --input /path/to/aether-client-java/clienttypes.adsl.yaml \
  --input /path/to/common-api/common.adsl.yaml \
  /tmp/aether-c-adsl-generated
Successful output:

text
generated aether_api.h/.c from 4 documents
Include resolution
include: accepts only a basename without a path or extension:

yaml
include:
  - common
The generator looks up common in the pool of documents collected from
the --input list. To make the include resolvable, pass either the
directory that contains common.adsl.yaml or the file itself:

sh
aether-adslc --input ./schemas --input ./extra/common.adsl.yaml /tmp/out
If the name is not present in the pool the run fails:

text
ADSL load error: cannot resolve include 'common' from '...': no document
with that name in the input pool
Directory scanning
When a directory is passed via --input, every *.adsl.yaml below it is
loaded recursively. Sub-directories are scanned to arbitrary depth. Files
that do not end with .adsl.yaml or .adsl.yml are ignored.

To keep the pool predictable, pass a directory that contains only the
documents you intend to compile. Passing a file compiles just that file
(plus anything it pulls in through include: if the referenced
document is also present in the pool).

Exit codes
0 — success; aether_api.h and aether_api.c were written.

1 — load, merge, validation, or generation error. A human-readable
message is written to stderr.

2 — invalid command line (unknown option, missing output directory,
too many inputs).

What the generator does not do
It does not keep any historical behaviour: the older --api,
--method, --local-api, --response-method and -I flags are gone.

It does not accept search paths for includes.

It does not emit per-module files such as <name>_types.h,
<name>_api.h, <name>_local.h, or <name>_response.h. Everything
lands in the single aether_api.h / aether_api.c pair.

It does not modify or regenerate any hand-written client code.
