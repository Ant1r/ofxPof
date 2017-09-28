#include "version.h"
#include "m_pd.h"

#ifdef INCLUDE_VERSION_FILE
#include "VERSION"
#endif

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

void printVersionMessage() {
	post(		"Pof: openFrameworks for Pure Data.");
	verbose(-1, " Pof: version " STRINGIFY(POF_VERSION) " compiled on " __DATE__);
	verbose(-1, " Pof: (BSD) Antoine Rousseau @ metalu.net");
}
