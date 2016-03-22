#include <cerrno>
#include <cstring>
#include <iostream>

namespace io
{
FILE * open_or_except(std::string path, std::string mode)
{
  FILE * handle = fopen(path.c_str(), mode.c_str());
  if (!handle) {
    std::string arg(std::string("error opening file ") + path + " with mode " +
                    mode);
    if (errno) {
      arg += std::string(": ") + strerror(errno);
    }
    throw io_exception(arg);
  }
  return handle;
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
async_out_handle<n, G, CHUNK_SIZE>::async_out_handle(std::string path)
    : ready(false), quit(false), handle(open_or_except(path, "a")), mtx(), cv(),
      internal_thread()
{
  /* kicks off thread */
  internal_thread = std::thread([&]() { wait_to_receive_data(); });
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
async_out_handle<n, G, CHUNK_SIZE>::~async_out_handle()
{
  fclose(handle);
  quit = true;
  std::unique_lock<std::mutex> lock(mtx);
  cv.notify_all();
  lock.unlock();
  internal_thread.join();
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
template <typename Func>
typename async_out_handle<n, G, CHUNK_SIZE>::process_fun
    async_out_handle<n, G, CHUNK_SIZE>::get_async_io_fun(Func quitp)
{
  /* NOTE: this only allows an io queue of one set of results at a time, which
     helps reduce memory pressure. if we find the io takes longer than
     computing each chunk, we can just increase the chunk size */
  using lsr = sample::linear_sim_result<n, G>;
  return [&](const_lsimr_arr results) {
    lsr * __restrict__ internal_ptr      = internal_array.data();
    const lsr * __restrict__ results_ptr = results.data();
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    if (!lock.try_lock()) {
      /* FIXME: consider some other stream than stderr */
      std::cerr << "COMPUTATION BLOCKING ON IO: INCREASE CHUNK SIZE MAYBE"
                << std::endl;
      lock.lock();
    }
    memcpy(internal_ptr, results_ptr,
           internal_array_element_size * internal_array.size());
    cv.notify_all();
    return !quitp();
  };
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void async_out_handle<n, G, CHUNK_SIZE>::turn_off()
{
  std::unique_lock<std::mutex> lock(mtx);
  quit = true;
  std::cerr << "quitting..." << std::endl;
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void async_out_handle<n, G, CHUNK_SIZE>::wait_to_receive_data()
{
  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    while (!ready && !quit) {
      cv.wait(lock);
    }
    if (quit) {
      return;
    }
    fwrite(internal_array.data(), internal_array_element_size,
           internal_array.size(), handle);
  }
}
}
