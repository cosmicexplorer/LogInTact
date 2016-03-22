#ifndef ___IO_HPP___
#define ___IO_HPP___
/* TODO: prefix all #ifndef guards! */

#include "sample.hpp"
#include <cstdio>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace io
{
struct io_exception : std::runtime_error {
  io_exception(std::string p) : std::runtime_error(p)
  {
  }
};

FILE * open_or_except(std::string, std::string);

template <size_t n, size_t G, size_t CHUNK_SIZE = sample::DEFAULT_CHUNK_SIZE>
struct async_out_handle {
  async_out_handle(std::string);
  ~async_out_handle();

  typedef std::array<sample::linear_sim_result<n, G>, CHUNK_SIZE> lsimr_arr;
  typedef const lsimr_arr & const_lsimr_arr;
  typedef std::function<bool(const_lsimr_arr)> process_fun;

  /* Func tells it whether to quit or not */
  template <typename Func>
  inline process_fun get_async_io_fun(Func);
  void turn_off();

private:
  volatile bool ready;
  volatile bool quit;
  FILE * handle;
  std::mutex mtx;
  std::condition_variable cv;
  std::thread internal_thread;

  lsimr_arr internal_array;
  static constexpr size_t internal_array_element_size =
      sizeof(typename decltype(internal_array)::value_type);

  void wait_to_receive_data();
};
}

#include "io.tpp"

#endif /* ___IO_HPP___ */
