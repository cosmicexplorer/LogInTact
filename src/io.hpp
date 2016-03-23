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

template <typename Func>
struct make_except_safe {
  Func f_internal;
  make_except_safe(Func);
  ~make_except_safe();
};

template <typename Func>
make_except_safe<Func> run_except_safe(Func);

/* POSIX-SPECIFIC: USES READ/WRITE/FORK/PIPE */
template <size_t n,
          size_t G,
          size_t CHUNK_SIZE = sample::DEFAULT_CHUNK_SIZE,
          typename Func>
void run_sim_async(sim::RealType,
                   sim::RealType,
                   const sample::sim_param_intervals<n> &,
                   Func);
}

#include "io.tpp"

#endif /* ___IO_HPP___ */
