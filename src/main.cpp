#include <iostream>
#include <atomic>
#include <csignal>
#include <climits>

/* linux */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "io.hpp"

/* checkout branch old-generic for how we were going to make this a little more
   generic with respect to linear/nonlinear systems before we went ham on
   performance */

template <typename Vect>
std::string PrintVect(const Vect & v)
{
  std::vector<std::string> ret;
  for (auto el : v) {
    ret.push_back(std::to_string(el));
    ret.push_back(",");
  }
  ret.pop_back();
  return std::accumulate(ret.begin(), ret.end(), std::string("["),
                         [](auto a, auto b) { return a + b; }) +
         "]";
}

std::atomic<bool> terminate(false);

void catch_sigint(int)
{
  terminate.store(true);
}

int main()
{
  using ri     = sample::real_interval;
  using res    = sim::linear_sim<3, 5>;
  using int_rv = std::array<ri, 3>;
  using int_rm = std::array<ri, 9>;
  std::cout << sizeof(res) << std::endl;
  std::cout << static_cast<double>(5e4 * sizeof(res)) / 1e6 << std::endl;
  sample::sim_param_intervals<3> ints(
      /* s_0 */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* D */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* W */
      int_rm{ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1},
             ri{0, 1}, ri{0, 1}, ri{0, 1}});
  constexpr size_t chunk = 5;
  /* TODO: read intervals from file, get output file from argv */
  int pipe_fds[2];
  pipe(pipe_fds);
  pid_t pid = fork();
  if (-1 == pid) {
    /* TODO: error handling */
  } else if (pid) { /* is parent */
    close(pipe_fds[0]);
    signal(SIGINT, catch_sigint);
    /* TODO: catch SIGCHLD */
    size_t num_chunks = 0;
    sample::do_simulated_sample<3, 5, chunk>(
        3, .3, ints, [&](const std::array<res, chunk> & results) {
          constexpr size_t num_bytes = sizeof(res) * results.size();
          const res * results_ptr    = results.data();
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
          ++num_chunks;
          return num_chunks < 5;
          /* return terminate.load(); */
        });
    close(pipe_fds[1]);
    waitpid(pid, nullptr, 0);
  } else { /* is child */
    close(pipe_fds[1]);
    std::unique_ptr<res[]> arr(new res[chunk]);
    FILE * out                = io::open_writex_or_except("outfile.out");
    constexpr size_t expected = sizeof(res) * chunk;
    static_assert(expected <= SSIZE_MAX, "undefined");
    while (true) {
      ssize_t read_bytes = read(pipe_fds[0], arr.get(), expected);
      if (-1 == read_bytes) {
        std::cerr << "help: " << std::strerror(errno) << std::endl;
        break;
        /* TODO: error handling */
      } else if (expected != read_bytes) {
        std::cerr << "weird: " << read_bytes << std::endl;
        break;
        /* TODO: error handling */
      }
      size_t written_num = fwrite(arr.get(), sizeof(res), chunk, out);
      if (expected != (written_num * sizeof(res))) {
        std::cerr << "weird2: " << written_num * sizeof(res) << std::endl;
        return -1;
        /* TODO: error handling */
      }
    }
    close(pipe_fds[0]);
    fclose(out);
  }
}
