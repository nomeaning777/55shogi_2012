#include "55shogi.hpp"
#include "Board55.hpp"
#include "PlayLog.hpp"
#include "Game.hpp"
#include "Search.hpp"
#include <limits>
Game* Game::game = NULL;

IGame::Move Game::get_move(){
  PlayLog result = Search::iddfs(board, !first, !first, board_cnt);
  IGame::Move res;
  res.resign=false;
  if((int)result() == numeric_limits<int>::max()){
    res.resign=true;
    return res;
  }
  res.move = result;
  board.move_piece(res.move);
  board_cnt[board.get_hash()]++;
  board.output_board_ja(cout);
  board.output_hands_ja(cout);
  cout << "First: "<<board.calc_evaluation_value(0) << endl;
  cout << "Second: "<<board.calc_evaluation_value(1) << endl;

  return res;
}
