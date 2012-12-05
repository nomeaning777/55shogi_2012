#include "Battle.hpp"
#include "Board55.hpp"
namespace Shogi55{
  void client(IProtocol &protocol, IGame &game){
    Board55 board;
    board.init_board();
    protocol.start(board, 0, 0);
    game.initialize(protocol.get_initial_board(), protocol.is_your_turn_first(), protocol.get_start_turn());
    
    board = protocol.get_initial_board();
    if(protocol.get_start_turn() != protocol.is_your_turn_first()){
      PlayLog move;
      bool resign;
      protocol.get_move(board, move, resign);
      if(resign){
        cout << "You Win!!" << endl;
        return;
      }
      IGame::Move m;
      m.move = move;
      m.resign = false;
      game.opposite_move(m);
      board.move_piece(move);
    }
    while(true){
      IGame::Move m = game.get_move();
      if(m.resign){
        protocol.resign();
        cout << "You Lose!!" << endl;
        break;
      }else{
        protocol.move(m.move);
      }
      board.move_piece(m.move);
      PlayLog move;
      bool resign;
      protocol.get_move(board, move, resign);
      if(resign){
        cout << "You Win!!" << endl;
        break;
      }
      m.move = move;
      m.resign = false;
      game.opposite_move(m);
      board.move_piece(move);
    }
  }
}
