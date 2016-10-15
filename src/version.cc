#include "version.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

const char* versionString() {
return ( "" STRINGIFY(POF_VERSION_MAJOR) "." STRINGIFY(POF_VERSION_MINOR) "." STRINGIFY(POF_VERSION_PATCH));
}

