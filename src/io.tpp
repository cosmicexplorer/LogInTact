#include <cerrno>
#include <fstream>

namespace io
{
FILE * open_writex_or_except(std::string path)
{
  /* check-then-write is a race condition, but whatever */
  if (std::ifstream(path).good()) {
    throw io_exception(std::string("file \"") + path + "\" already exists");
  }
  errno         = 0;
  FILE * handle = fopen(path.c_str(), "w");
  if (!handle) {
    std::string arg(std::string("error opening file \"") + path +
                    "\" for writing");
    if (errno) {
      arg += std::string(": ") + strerror(errno);
    }
    throw io_exception(arg);
  }
  return handle;
}
}
