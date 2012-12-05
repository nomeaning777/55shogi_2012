#include "CSADirectClient.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
using namespace std;
#define dout cout
#define ALL_OUTPUT
bool starts_with(const string &s, const string &t){
  return s.substr(0,t.size()) == t;
}

CSADirectClient::CSADirectClient(string host, int port) : 
    s(host, boost::lexical_cast<string>(port)){
  dout << "connection" << endl;
}

CSADirectClient::~CSADirectClient(){
  s << "LOGOUT" << endl;
}

void CSADirectClient::login(string username, string password){
  dout << "LOGIN " << username << " " << password << "\n" << flush;
  s << (boost::format("LOGIN %s %s\n") % username % password).str();
  s << flush;

  string in;
  getline(s, in);

  if(in.size() > 6 + username.size() + 1 && in.substr(6 + username.size() + 1) == "OK"){
    dout << "Success Login" << endl;
  }else{
    cerr << "Login Failed" << endl;
    throw std::runtime_error("Login Failed");
  }
  
  read_game_summary();

}

void CSADirectClient::read_game_summary(){
  string in;
  summary.rematch_on_draw=false;
  summary.time_first = summary.time_second = CSADirectClient::Time::get_default();
  while(getline(s,in)){
    #ifdef ALL_OUTPUT
      cout << in << endl;
    #endif
    if(in == "END Game_Summary"){
      break;
    }else if(in == "BEGIN Game_Summary"){
      continue;
    }else if(in == "BEGIN Time"){
      summary.time_first = summary.time_second = read_game_time();
    }else if(in == "BEGIN Time+"){
      summary.time_first = read_game_time();
    }else if(in == "BEGIN Time-"){
      summary.time_second = read_game_time();
    }else if(starts_with(in, "BEGIN Position")){
      summary.initial = read_position();
    }else{
      vector<string> tmp;
      boost::algorithm::split(tmp, in, boost::is_any_of(":"));
      if(tmp.size() == 2){
        if(tmp[0] == "Protocol_Version"){
          summary.protocol_version = tmp[1];
        }else if(tmp[0] == "Protocol_Mode"){
          summary.protocol_mode = tmp[1];
        }else if(tmp[0] == "Format"){
          summary.format = tmp[1];
        }else if(tmp[0] == "Declaration"){
          summary.declaration = tmp[1];
        }else if(tmp[0] == "Game_ID"){
          summary.game_id = tmp[1];
        }else if(tmp[0] == "Name+"){
          summary.name_first = tmp[1];
        }else if(tmp[0] == "Name-"){
          summary.name_second = tmp[1];
        }else if(tmp[0] == "Your_Turn"){
          if(tmp[1] == "+"){
            summary.your_turn = FIRST;
          }else{
            summary.your_turn = SECOND;
          }
        }else if(tmp[0] == "Rematch_On_Draw"){
          if(tmp[1] == "YES"){
            summary.rematch_on_draw=true;
          }else{
            summary.rematch_on_draw=false;
          }
        }else if(tmp[0] == "To_Move"){
          if(tmp[1] == "+"){
            summary.start_move = FIRST;
          }else{
            summary.start_move = SECOND;
          }
        }
      }else{
        dout << "Error(Format):" << in << endl;
      } 
    }
  }
}

CSADirectClient::Time CSADirectClient::read_game_time(){
  string in;
  CSADirectClient::Time ret = CSADirectClient::Time::get_default();
  while(getline(s,in)){
    #ifdef ALL_OUTPUT
      cout << in << endl;
    #endif
    if(starts_with(in,"BEGIN Time")){
      continue;
    }else if(starts_with(in,"END Time")){
      break;
    }else{
      vector<string> tmp;
      boost::algorithm::split(tmp, in, boost::is_any_of(":"));
      if(tmp.size() == 2){
        if(tmp[0] == "Time_Unit"){
          ret.time_unit = tmp[1];
        }else if(tmp[1] == "Least_Time_Per_Move"){
          ret.least_time_per_move = boost::lexical_cast<int>(tmp[0]);
        }else if(tmp[1] == "Total_Time"){
          ret.least_time_per_move = boost::lexical_cast<int>(tmp[0]);
        }else if(tmp[1] == "Byoyomi"){
          ret.least_time_per_move = boost::lexical_cast<int>(tmp[0]);
        }else if(tmp[1] == "Time_Rountup"){
          if(tmp[2] == "YES"){
            ret.time_round_up = true;
          }
        }
      }
    }
  }
  return ret;
}

Board55 CSADirectClient::read_position(){
  string in;
  Board55 ret;
  for(int y = 0; y < 9; y++){
    getline(s, in);
    if(in == "BEGIN Position"){
      getline(s, in);
    }
    #ifdef ALL_OUTPUT
      cout << in << endl;
    #endif
    // "P*"を消去する
    in = in.substr(2);
    vector<string> info;
    for(int x = 0; x < 9; x++){
      if(in[0] == '+' || in[0] == '-'){
        info.push_back(in.substr(0,3));
        in = in.substr(3);
      }else{
        info.push_back("");
        while(!in.empty() && in[0]==' ')in = in.substr(1);
        while(!in.empty() && in[0]=='*')in = in.substr(1);
        while(!in.empty() && in[0]==' ')in = in.substr(1);
      }
    }
    for(int x = 0; x < 4; x++)info.erase(info.begin());
    if(y < 5){
      for(int x = 0; x < 5; x++){
        if(info[x] == ""){
          continue;
        }
        int turn = 0;
        if(info[x][0] == '-'){
          turn = 1;
        }
        info[x] = info[x].substr(1);
        #define case(t) if(info[x] == #t){ret.put_piece(turn, y, x, Board55::t);}
        case(FU);
        case(TO);
        case(HI);
        case(RY);
        case(KA);
        case(UM);
        case(GI);
        case(NG);
        case(KI);
        case(OU);
        #undef case
      }
    }
  }
  while(getline(s,in)){
    #ifdef ALL_OUTPUT
      cout << in << endl;
    #endif
    if(in == "END Position"){
      break;
    }
    // 持駒取得
    if(starts_with(in, "P+")||starts_with(in, "P-")){
      int turn = 0;
      if(in[1] == '-')turn = 1;
      in = in.substr(4);
      for(int i = 0; i < static_cast<int>(in.size()); i += 4){
        #define case(t) if(in.substr(0,2) == #t){ret.add_hands(turn, Board55::t);}  
        case(FU);
        case(TO);
        case(HI);
        case(RY);
        case(KA);
        case(UM);
        case(GI);
        case(NG);
        case(KI);
        case(OU);
      }
    }
  }

  #ifdef ALL_OUTPUT
  ret.output_board_ja(cout);
  ret.output_hands_ja(cout);
  #endif
  return ret;
}

void CSADirectClient::start(Board55 board, int my_turn, int start_turn){
  // 対局に同意する
  s << "AGREE\n";
  s << flush;
  string in;
  getline(s, in); // START
}

void CSADirectClient::quit(){
  s << "%CHUDAN\r" << flush;
  s << "LOGOUT\r" << flush;
}

void CSADirectClient::resign(){
  s << "%TORYO\r" << flush; 
}

// boardは変更されない
void CSADirectClient::get_move(Board55 &board, PlayLog &move, bool &resign){
  string in;
  getline(s, in);
  resign = false;
  if(in == "%TORYO"){
    resign = true;
    return;
  }
  #ifdef ALL_OUTPUT
  cout << in << endl;
  #endif
  int turn = 0;
  if(in[0] == '-')turn = 1;
  int back =  5 * (in[2] - '1') + 5 - (in[1] - '0');
  int forward =  5 * (in[4] - '1') + 5 - (in[3] - '0');
  int next_piece = Board55::get_piece_from_name(in.substr(5,2));
  if(in[2] == '0'){
    move = PlayLog(turn, next_piece, PlayLog::PUT_PIECE,forward,  false);
  }else{
    int piece = board.get_piece(turn, back);
    int take = board.get_piece(1 - turn, forward);
    bool promote = (piece != next_piece);
    if(take == -1)take = PlayLog::NOT_GET;
    move = PlayLog(turn, piece, back, forward, promote, take);
  }
}

string get_place_str(int place){
  int y = place / 5, x = place % 5;
  string res = "00";
  res[1] += y + 1;
  res[0] += 5 - x;
  return res;
}

void CSADirectClient::move(PlayLog move){
  string turn;
  if(summary.your_turn == FIRST){
    turn = "+";
  }else{
    turn = "-";
  }
  string back;
  if(move.put_piece()){
    back = "00";
  }else{
    back = get_place_str(move.get_back_pos());
  }
  string forward;
  forward = get_place_str(move.get_forward_pos());
  int type = move.get_type();
  if(move.get_promote()){
    type = Board55::PROMOTE[type];
  }
  string piece = Board55::get_piece_name(static_cast<Board55::Pieces>(type)); 
  string mes = turn + back + forward + piece;
  dout << "YourMove" << mes << endl << flush;
  s << mes + "\n" << flush;
}
# if 0
CSADirectClient test("192.168.11.4", 4081);
void test_csa_direct_client(){
  test.login("testProgram");
  test.start(Board55(), 0, 0);
  test.move(PlayLog(0, Board55::GI, 22, 17, false));
  PlayLog p;
  bool resign;
  PlayLog move;
  Board55 board = test.get_game_summary().initial;
  test.get_move(board, move, resign);
  board.move_piece(move);
  
  board.output_board_ja(cout);
  test.resign();
  test.quit();
}
#endif
