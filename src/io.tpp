#include <fstream>
#include <climits>
#include <iostream>
#include <csignal>

/* linux */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

template <typename Func>
make_except_safe<Func>::make_except_safe(Func f)
    : f_internal(f)
{
}

template <typename Func>
make_except_safe<Func>::~make_except_safe()
{
  f_internal();
}

template <typename Func>
make_except_safe<Func> run_except_safe(Func f)
{
  return make_except_safe<Func>(f);
}

template <size_t n, size_t G, size_t CHUNK_SIZE, typename Func>
void run_sim_async(sim::RealType t_f,
                   sim::RealType epsilon,
                   const sample::sim_param_intervals<n> & intervals,
                   std::string outfile,
                   Func f)
{
  int pipe_fds[2];
  pipe(pipe_fds);
  pid_t pid = fork();
  if (-1 == pid) {
    throw io_exception("fork could not complete");
  } else if (pid) { /* is parent */
    close(pipe_fds[0]);
    signal(SIGCHLD, SIG_IGN);
    producer_process<n, G, CHUNK_SIZE, Func>(t_f, epsilon, pipe_fds[1],
                                             intervals, f);
    close(pipe_fds[1]);
    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) or WEXITSTATUS(status) != EXIT_SUCCESS) {
      throw io_exception(
          std::string("child exited with status " + std::to_string(status)));
    }
    /* TODO: add some sort of speed testing for optimal chunk size */
  } else { /* is child */
    try {
      close(pipe_fds[1]);
      FILE * out;
      auto cleanup __attribute__((unused)) = run_except_safe([&]() {
        close(pipe_fds[0]);
        if (out) {
          fclose(out);
        }
      });
      out = open_writex_or_except(outfile);
      consumer_process<n, G, CHUNK_SIZE>(pipe_fds[0], out);
    } catch (io_exception & e) {
      std::cerr << e.what() << std::endl;
      _Exit(EXIT_FAILURE);
    }
    _Exit(EXIT_SUCCESS);
  }
}

template <size_t n, size_t G, size_t CHUNK_SIZE, typename Func>
void producer_process(sim::RealType t_f,
                      sim::RealType epsilon,
                      int write_pipe,
                      const sample::sim_param_intervals<n> & intervals,
                      Func f)
{
  using simu = sim::linear_sim<n, G>;
  sample::do_simulated_sample<n, G, CHUNK_SIZE>(
      t_f, epsilon, intervals,
      [&](const std::array<simu, CHUNK_SIZE> & results) {
        constexpr size_t num_bytes = sizeof(simu) * results.size();
        static_assert(num_bytes <= SSIZE_MAX, "undefined");
        const simu * results_ptr = results.data();
        ssize_t written_bytes = write(write_pipe, results_ptr, num_bytes);
        if (-1 == written_bytes) {
          throw io_exception("failed write to parent pipe");
        } else if (num_bytes != written_bytes) {
          throw io_exception("write of different size to parent pipe");
        }
        std::cerr << "here!" << std::endl;
        return f();
      });
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void consumer_process(int read_pipe, FILE * handle)
{
  using simu = sim::linear_sim<n, G>;
  std::unique_ptr<simu[]> arr(new simu[CHUNK_SIZE]);
  constexpr size_t expected = sizeof(simu) * CHUNK_SIZE;
  static_assert(expected <= SSIZE_MAX, "undefined");
  static_assert(expected != 0, "no empty reads");
  while (true) {
    ssize_t read_bytes = read(read_pipe, arr.get(), expected);
    if (-1 == read_bytes) {
      throw io_exception(std::string("can't read from child pipe: ") +
                         strerror(errno));
    } else if (0 == read_bytes) { /* pipe closed */
      return;
    }
    size_t written_bytes = fwrite(arr.get(), 1, read_bytes, handle);
    if (static_cast<size_t>(read_bytes) != written_bytes) {
      throw io_exception("write to child file of unexpected length");
    }
  }
}
}
