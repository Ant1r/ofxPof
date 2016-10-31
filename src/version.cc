#include "version.h"
#include "m_pd.h"

#define POF_VERSION_MAJOR 0
#define POF_VERSION_MINOR 1
#define POF_VERSION_PATCH 5

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

const char* versionString() {
return ( "" STRINGIFY(POF_VERSION_MAJOR) "." STRINGIFY(POF_VERSION_MINOR) "." STRINGIFY(POF_VERSION_PATCH));
}

void printVersionMessage() {
	post(		"Pof: openFrameworks for Pure Data.");
	verbose(-1, " Pof: %s compiled on " __DATE__, versionString());
  verbose(-1, " Pof: (BSD) Antoine Rousseau @ metalu.net");
}
