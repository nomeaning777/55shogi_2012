#ifndef IGame_HPP_
#define IGame_HPP_

#include "55shogi.hpp"
#include "Board55.hpp"
#include "PlayLog.hpp"

/*! ひとつの対局を管理するクラス。
 *
 */
class IGame{
public:
  // 55将棋での行動を表す
  struct Move;
  // 盤面を初期化する
  // start: 初期盤面
  // first: 先手かどうか
  // first_start: 先手番からかどうか
  virtual void initialize(Board55 start, bool first, bool first_start) = 0;

  // あなたのターンの行動を返す。
  virtual Move get_move() = 0;

  // 相手のターンになったことの通知
  virtual void opposite_move(Move opposite) = 0;

  // 1手戻す
  virtual void retract() = 0;
  

  virtual ~IGame(){}
};

struct IGame::Move{
  PlayLog move;
  bool resign;
};
#endif
