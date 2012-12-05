#include <unistd.h>
#include <sys/time.h>

using namespace std;

namespace Util{
  double get_time_of_day_sec(void){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec*1e-6;
  }
}
