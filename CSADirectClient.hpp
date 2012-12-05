#ifndef CSADirect_HPP_
#define CSADirect_HPP_
#include "IProtocol.hpp"
#include <iostream>
#include <boost/asio.hpp>

using namespace std;
namespace ip = boost::asio::ip;
class CSADirectClient: public IProtocol
{
  enum TURN{FIRST,SECOND};
    
  struct Time{
    string time_unit;
    int least_time_per_move;
    bool time_round_up;
    int total_time;
    int byoyomi;
    static Time get_default(){
      return (Time){"sec", 0, false, -1, -1};
    }
  };

  struct GameSummary{
    string protocol_version;
    string protocol_mode;
    string format;
    string declaration;
    string game_id;
    string name_first, name_second;
    TURN your_turn;
    bool rematch_on_draw;
    TURN start_move;
    Time time_first, time_second;
    Board55 initial;
  };

  GameSummary summary;
  
  void read_game_summary();
  
  Time read_game_time();

  Board55 read_position();

  ip::tcp::iostream s;
public:
  // CSAに接続する
  CSADirectClient(string host, int port = 4081);
  // ログインする
  void login(string username, string password = "tekitou");
  
  // ゲームを開始する
  // CSADirectの仕様として、引数は無視される。
  void start(Board55 board, int my_turn, int start_turn);

  // 対戦を強制終了する
  void quit();
  
  // 投了する
  void resign();
  
  const GameSummary& get_game_summary(){
    return summary;
  }

  // 相手の手を取得する
  void get_move(Board55 &board, PlayLog &move, bool &resign);

  void move(PlayLog move);
  
  Board55 get_initial_board(){
    return summary.initial;
  }

  bool is_your_turn_first(){
    return summary.your_turn == FIRST;
  }
  
  bool get_start_turn(){
    return summary.start_move == FIRST;
  }
  ~CSADirectClient();
};

void test_csa_direct_client();
#endif
