#ifndef ___LOGINTACT_IO_HPP___
#define ___LOGINTACT_IO_HPP___

#include "sample.hpp"

namespace LogInTact
{
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
                   size_t,
                   const sample::sim_param_intervals<n> &,
                   std::string,
                   Func);

template <size_t n, size_t G, size_t CHUNK_SIZE, typename Func>
void producer_process(sim::RealType,
                      sim::RealType,
                      size_t,
                      int,
                      const sample::sim_param_intervals<n> &,
                      Func);

template <size_t n, size_t G, size_t CHUNK_SIZE>
void consumer_process(int, FILE *);
}
}

#include "io.tpp"

#endif /* ___LOGINTACT_IO_HPP___ */
