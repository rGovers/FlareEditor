#include "IO.h"

#include <stdlib.h>

std::string_view IO::GetHomePath()
{
    return getenv("HOME");
}