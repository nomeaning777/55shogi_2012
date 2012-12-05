#ifndef Game_HPP_
#define Game_HPP_
#include "IGame.hpp"
#include <unordered_map>

class Game : public IGame{
  private:
    Board55 board;
    bool first;
    Game(){};
    static Game *game;
    unordered_map<board_hash_t, int> board_cnt;
  public:
    static Game &get_instance(){
      if(!game){
        game = new Game();
      } 
      return *game;
    }
    void initialize(Board55 start, bool first, bool first_start){
      start.get_hash_naive();
      start.calc_evaluation_value_naive(0);
      start.calc_evaluation_value_naive(1);

      this->board = start;
      this->first = first;
      board_cnt.clear();
    }

    Move get_move();

    void opposite_move(Move opposite){
      this->board.move_piece(opposite.move);
      board.output_board_ja(cout);
      cout << "First: " << board.calc_evaluation_value(0) << endl;
      cout << "Second: " << board.calc_evaluation_value(1) << endl;
    }

    void retract(){
      throw "Not implemented";
    }
};

#endif
