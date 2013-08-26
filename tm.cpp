#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <time.h>

using namespace std;
int main(int argc, char**argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s /dev/urandom number_of_ids\n", argv[0]);
    exit(1);
  }
  time_t begin_time;
  time(&begin_time);

  typedef vector<uint64_t> myvec;
  myvec bigger_ids;
  long to_read = strtol(argv[2], NULL, 10);
  int fd = open(argv[1], 0);
  bigger_ids.resize(to_read);
  long to_read_bytes = to_read * sizeof(uint64_t);
  
  ssize_t ret = read(fd, &bigger_ids[0], to_read_bytes);
  if (ret != to_read_bytes) {
    fprintf(stderr, "underflow on read() %ld != %ld\n", ret, to_read_bytes);
    exit(1);
  }
#define TIMESTAMP() ((int)difftime(time(NULL), begin_time))
  fprintf(stderr, "%d: Read in %ld bytes\n", TIMESTAMP(), ret);
  //for (uint64_t i = 0;i < bigger_ids.size();i++)
  //  printf("%lX\n",bigger_ids[i]); 

  myvec incoming_ids;
  // incoming data is 10% of overall
  long incoming_to_read = to_read * 10/200;
  incoming_ids.resize(incoming_to_read * 2);
  //50% of incoming data is old ids
  for (long i = 0;i < incoming_to_read;i++) {
    incoming_ids[i] = bigger_ids[rand() % to_read];
  }
  //50% more urandom
  to_read_bytes = incoming_to_read * sizeof(uint64_t);
  ret = read(fd, &incoming_ids[incoming_to_read], to_read_bytes);
  if (ret != to_read_bytes) {
    fprintf(stderr, "underflow2 on read() %ld != %ld\n", ret, to_read_bytes);
    exit(1);
  }

  fprintf(stderr, "%d: Sorting\n", TIMESTAMP());
  std::sort(bigger_ids.begin(), bigger_ids.end());
  std::sort(incoming_ids.begin(), incoming_ids.end());

  myvec intersection(incoming_ids.size());
  myvec::iterator it;
  fprintf(stderr, "%d: Intersecting\n", TIMESTAMP());  
  it=std::set_intersection (bigger_ids.begin(), bigger_ids.end(), incoming_ids.begin(), incoming_ids.end(),  intersection.begin());
  intersection.resize(it-intersection.begin());                      // 10 20
  fprintf(stderr, "%d: %ld(%ld%%) elements intersect\n", TIMESTAMP(), intersection.size(), intersection.size() * 100 / bigger_ids.size());

  return 0;
}
