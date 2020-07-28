// Please note that includes are not neccesary for the tracealloc to function.
// We just link in our own new and delete function and the linker takes care of all the rest.
// Only requirement is that we define DETECT_LEAKS in the project settings.
// See tracealloc.cpp for further instructions.

// If you get multiple defined symbols (overloaded new and delete)
// add linker switch /FORCE:MULTIPLE on the exe and make sure the
// tracealloc new and delete is the ones used. If not, reorder the
// included libraries until they do.

#include "windows.h"

void generate_memoryleak()
{
  new int; // Generate a memory leak
}

int main(int argc, char** argv)
{
  generate_memoryleak();
  return 0;
}
