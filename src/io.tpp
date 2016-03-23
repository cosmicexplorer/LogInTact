#include <cerrno>
#include <fstream>
#include <climits>

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
                   Func f)
{
  using simu = sim::linear_sim<n, G>;
  int pipe_fds[2];
  pipe(pipe_fds);
  pid_t pid = fork();
  if (-1 == pid) {
    /* TODO: error handling */
  } else if (pid) { /* is parent */
    close(pipe_fds[0]);
    auto cleanup __attribute__((unused)) = run_except_safe([&]() {
      close(pipe_fds[1]);
      waitpid(pid, nullptr, 0);
    });
    sample::do_simulated_sample<n, G, CHUNK_SIZE>(
        t_f, epsilon, intervals,
        [&](const std::array<simu, CHUNK_SIZE> & results) {
          constexpr size_t num_bytes = sizeof(simu) * results.size();
          const simu * results_ptr   = results.data();
          ssize_t written_bytes = write(pipe_fds[1], results_ptr, num_bytes);
          if (-1 == written_bytes) {
            std::cerr << "no write" << std::endl;
            exit(1);
            /* TODO: error handling */
          } else if (num_bytes != written_bytes) {
            std::cerr << "WEIRD" << std::endl;
            exit(1);
            /* TODO: error handling */
          }
          std::cerr << "here!" << std::endl;
          return f();
        });
    /* TODO: add some sort of speed testing for optimal chunk size */
  } else { /* is child */
    close(pipe_fds[1]);
    std::unique_ptr<simu[]> arr(new simu[CHUNK_SIZE]);
    FILE * out                = open_writex_or_except("outfile.out");
    constexpr size_t expected = sizeof(simu) * CHUNK_SIZE;
    static_assert(expected <= SSIZE_MAX, "undefined");
    auto cleanup __attribute__((unused)) = run_except_safe([&]() {
      close(pipe_fds[0]);
      fclose(out);
    });
    while (true) {
      ssize_t read_bytes = read(pipe_fds[0], arr.get(), expected);
      if (-1 == read_bytes) {
        std::cerr << "help: " << std::strerror(errno) << std::endl;
        exit(1);
        /* TODO: error handling */
      } else if (expected != read_bytes) {
        std::cerr << "weird: " << read_bytes << std::endl;
        exit(0);
        /* TODO: error handling */
      }
      size_t written_num = fwrite(arr.get(), sizeof(simu), CHUNK_SIZE, out);
      if (expected != (written_num * sizeof(simu))) {
        std::cerr << "weird2: " << written_num * sizeof(simu) << std::endl;
        exit(1);
        /* TODO: error handling */
      }
    }
  }
}
}
