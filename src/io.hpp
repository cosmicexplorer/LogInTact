#ifndef ___IO_HPP___
#define ___IO_HPP___
/* TODO: prefix all #ifndef guards! */

#include "sample.hpp"
#include <cstdio>

namespace io
{
struct io_exception : std::runtime_error {
  io_exception(std::string p) : std::runtime_error(p)
  {
  }
};

FILE * open_writex_or_except(std::string);
}

#include "io.tpp"

#endif /* ___IO_HPP___ */
