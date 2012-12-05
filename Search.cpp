#include <vector>
#include <array>
#include "Search.hpp"
#include <limits>
#include <climits>
#include "Util.hpp"
#include "55shogi.hpp"
#include <boost/format.hpp>
using namespace std;
#define dout cout
namespace Search{
  struct Transposition{
    board_hash_t hash;
    int depth;
    int e_value;
  };
  
  Transposition t_table[2][1<<18]; // max_depth - depth

  inline void add_transposition(board_hash_t hash, int v, int d,int turn){
    if(t_table[turn][hash & 262143].hash == hash && t_table[turn][hash & 262143].depth <= d)return;
    t_table[turn][hash & 262143].e_value = v;
    t_table[turn][hash & 262143].depth = d;
    t_table[turn][hash & 262143].hash = hash;
  }
  
  int debug1,debug2;
  inline int get_transposition(board_hash_t hash, int d, int turn){
    debug2++;
    if(t_table[turn][hash & 262143].depth < d)return INT_MIN;
    if(t_table[turn][hash & 262143].hash == hash){
      debug1++;
      return t_table[turn][hash & 262143].e_value;
    }
    return INT_MIN;
  }
  
  vector<vector<PlayLog>> moves(1024,vector<PlayLog>(256));
  int board_cnt; // 探索盤面数
  const int INF = numeric_limits<int>::max()-10; 

  // 静止探索
  namespace SlientSearch{
    vector<vector<PlayLog>> moves(1024,vector<PlayLog>(256));
    int search(int turn, int depth, Board55 &board, int alpha, int beta){
      if(board.is_lose(1-turn))return INF;
      if(board.is_lose(turn))return -INF;
      board_cnt++;
      if(depth < 0){
        return board.calc_evaluation_value(turn) - board.calc_evaluation_value(1-turn);
      }
      int cnt = board.gen_legal_take_piece(turn, &moves[depth][0]);
      if(cnt == 0){
        return board.calc_evaluation_value(turn) - board.calc_evaluation_value(1-turn);
      }
      for(int i = 0; i <cnt; i++){
        board.move_piece(moves[depth][i]);
        int x = -search(!turn, depth - 1 , board, -beta, -alpha);

        board.reverse_move_piece(moves[depth][i]);
        if(x > alpha){
          alpha = x;
        }
        if(alpha >= beta){break;}
      }
      return alpha;
    }
  }
  bool use_slient_search;
  uint64 sort_tmp[256];
  int search(int turn,int depth, Board55 &board, int max_depth,int alpha, int beta){
    board_cnt++;
    if(board.is_lose(1-turn))return INF-depth; // 相手の負けなら勝ちである
    if(depth >= max_depth){
      // ちょっと静止探索する
      if(use_slient_search){
        return SlientSearch::search(turn, 0, board, alpha,beta);
      }else{
        return board.calc_evaluation_value(turn) - board.calc_evaluation_value(1-turn);
      }
    }
    int t = get_transposition(board.get_hash(), max_depth - depth, turn);
    if(t > INT_MIN)return t;
    int cnt = board.gen_legal2(turn, &moves[depth][0]);
    /*PlayLog test[333];
    int cnt2 = board.gen_legal(turn, test);
    if(cnt != cnt2){
    cout << cnt << "," << cnt2 << endl;
    for(int i=0;i<cnt;i++){
      cout<<board.move_to_string_ja(moves[depth][i])<<endl;

    }
    cout<<"---"<<endl;
    for(int i=0;i<cnt2;i++){
      cout<<board.move_to_string_ja(test[i])<<endl;
    }
    board.output_board_ja(cout);
    }
    assert(cnt == cnt2);*/
    if(cnt == 0)return -INF+depth;
    if(max_depth - 4 > depth){
      // sort
      for(int i = 0; i < cnt; i++){
        board.move_piece(moves[depth][i]);
        sort_tmp[i] = ((static_cast<long long>(get_transposition(board.get_hash(), 0, turn)) - INT_MIN) << 32) + board.calc_evaluation_value(turn);
        board.reverse_move_piece(moves[depth][i]);
      }
      for(int i = 1; i < cnt;i++){
        uint64 t = sort_tmp[i];
        PlayLog tmp = moves[depth][i];
        if(sort_tmp[i-1] < t){
          int j = i;
          do{
            sort_tmp[j]=sort_tmp[j-1];
            moves[depth][j]=moves[depth][j-1];
            --j;
          }while(j > 0 && sort_tmp[j-1] < t);
            sort_tmp[j] = t;
            moves[depth][j]=tmp;
        }
      }
    }

    for(int i = 0; i <cnt; i++){
      board.move_piece(moves[depth][i]);
      int x = -search(!turn, depth + 1  , board, max_depth, -beta, -alpha);

      board.reverse_move_piece(moves[depth][i]);
      if(x > alpha){
        alpha = x;
      }
      if(alpha >= beta){break;}
    }
   add_transposition(board.get_hash(), alpha, max_depth - depth, turn);
    return alpha;
  }
  int eval;
  PlayLog search_root(int turn, Board55 &board, int max_depth, bool win_sennichi, unordered_map<board_hash_t, int> &hash){
    board_cnt++;
    int max = -INF;
    PlayLog res = numeric_limits<int>::max();
    int cnt = board.gen_legal(turn & 1, moves[0]);
    for(int i = 0; i < cnt; i++){
      board.move_piece(moves[0][i]);
      if(hash[board.get_hash()] == 2 && win_sennichi){
        board.reverse_move_piece(moves[0][i]);
        return moves[0][i];
      }
      if(hash[board.get_hash()] <= 1){
        int x = -search(!turn, 1, board, max_depth, -INF, -max);
        if(x > max){
          max = x;
          res = moves[0][i];
        }
      }
      board.reverse_move_piece(moves[0][i]);
    }
    eval = max;
    return res;
  }
   

  PlayLog iddfs(Board55 board, int turn, bool win_sennichi, unordered_map<board_hash_t,int> &hash){
    const int P = 4; // 一手深さを上げる時に読む手数の増え方（倍率）（適当）
    const int MAX_BOARD_CNT = 200000000; // 何局面まで調べるか
    board.output_board_ja(cout);
    board.output_hands_ja(cout);
    PlayLog res = numeric_limits<int>::max();
    board_cnt = 0;
    use_slient_search = false; 
    for(int depth = 4; ; depth += 1){
      if((long long)board_cnt * P * P > MAX_BOARD_CNT){
        //use_slient_search = true;
      }
      board_cnt = 0;
      double start=Util::get_time_of_day_sec();
      
      eval = 0;
      PlayLog tmp;
      debug1 = debug2 = 0;
      tmp = search_root(turn, board, depth, win_sennichi, hash);
      
      double end=Util::get_time_of_day_sec();
      
      dout << "探索：最大深さ(" <<depth << ") 平均深さ("<<  (depth + 1) / 2 << ")" << endl;
      dout << "探索時間：" << boost::format("%.6f sec") % (end-start)<< endl;
      dout << "探索盤面数：" << board_cnt << endl;
      dout << "一秒間当りの探索盤面数：" << (int)(board_cnt / (end - start)) << endl;
      dout << "静止探索：" << (use_slient_search? "あり" : "なし") << endl;
      dout << "探索評価値：" << eval << endl;
      if(eval <= -INF){
        dout << "詰みです" << endl;
      }else{
        dout << "探索手：" << board.move_to_string_ja(tmp) << endl;
        res = tmp;
      }
      dout << "ヒット率" << (double)debug1 * 100 / debug2 << endl;
      dout << endl;
      if((long long)board_cnt * P > MAX_BOARD_CNT)break;
    }
    return res;
  }
}
