#ifndef IProtocol_HPP_
#define IProtocol_HPP_
#include "55shogi.hpp"
#include "Board55.hpp"
#include "PlayLog.hpp"

class IProtocol{
public:
  // 対局を開始する
  virtual void start(Board55 board, int my_turn, int start_turn) = 0;

  // 対局を強制終了する 
  virtual void quit() = 0;

  // 指し手を指す
  virtual void move(PlayLog move) = 0;

  // 投了する
  virtual void resign() = 0;

  // 相手の手を取得する（相手の手を取得するまで同期的に処理が行われる）
  virtual void get_move(Board55 &board, PlayLog &move, bool &resign) = 0;
  
  // 初期盤面を取得する
  virtual Board55 get_initial_board() = 0;
  
  // 先手かどうか
  virtual bool is_your_turn_first() = 0;

  // 先手番かどうか
  virtual bool get_start_turn() = 0;
  virtual ~IProtocol(){};
};
#endif
