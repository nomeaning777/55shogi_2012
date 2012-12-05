#include "55shogi.hpp"
#include "Board55.hpp"
#include "PlayLog.hpp"
#include "unordered_map"
using namespace std;
namespace Search{
  void alphabeta();

  PlayLog iddfs(Board55 board, int turn, bool win_sennichi, unordered_map<board_hash_t,int> &hash); 
}
