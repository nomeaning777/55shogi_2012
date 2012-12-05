#ifndef BOARD_HPP_
#define BOARD_HPP_

#include "PlayLog.hpp"
#include "55shogi.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <random>
#include <climits>
#include <functional>
#include <exception>
using namespace std;
//! 55将棋の盤面を表す
/*!
  55将棋の盤面を表すクラスなのです
 */
typedef unsigned long long board_hash_t;

typedef union _bit_board{
  uint64 all_board;
  uint32 board[2];
} BitBoard;

class Board55{
  private:

    uint16 hand[2]; // whiteが先手,blackは後手
    //
    static const int MAX_PIECES=10;
    static const int MAX_HANDS=5;
    inline int bit_pos(int y, int x){
      return y * 5 + x;
    }
    static const int HAND_TO_PIECE[MAX_HANDS];
  public:
    static const int HAND_NO[MAX_PIECES];
    static const int PROMOTE[MAX_PIECES];
  private:
    BitBoard bit_board[MAX_PIECES]; // 各駒に関する情報
    static const string PIECE_NAME_ENG[MAX_PIECES];
    static const string PIECE_NAME_JA[MAX_PIECES];

    __inline uint64 white(uint64);
    __inline uint64 black(uint64);
    void output_board(ostream &os,const string PIECE_NAME[10]);


    // アタックボード
    static const int ATTACK_BIT_BOARD[2][MAX_PIECES][25];
    static const int MAGIC_BIT_BOARD[2][25];
    // このマスでなることが可能かどうか
    static const int CAN_PROMOTE[2][25];

    int evaluation_value[2];
    board_hash_t board_hash;
    board_hash_t piece_hash[2][MAX_PIECES][25];
    board_hash_t hands_hash[2][1 << (MAX_HANDS << 1)];

    int evaluation_value_of_hand_cache[1<<10];

    void init_evaluation_value();
  public:
    enum Pieces{
      FU,TO,HI,RY,KA,UM,GI,NG,KI,OU
    };

    enum Hands{
      hFU,hHI,hKA,hGI,hKI
    };
    Board55();

    static string get_piece_name(int a);
    static int get_piece_from_name(string name);

    void remove_hands(uint64 turn, int piece);
    void add_hands(uint64 turn, int piece);

    // 駒を配置する
    void put_piece(uint64 turn, int y, int x, int piece){
      bit_board[piece].all_board |= (1LL << bit_pos(y,x)) << (turn * 32);
    }

    // 乱数を生成する
    void random_number_generate();

    board_hash_t get_hash(){
      return board_hash;
    }

    // ハッシュ値の生成する
    board_hash_t get_hash_naive();

    // 初期状態を生成する
    void init_board();
    // 盤面の状態を出力します
    void output_board_eng(ostream &os);
    void output_board_ja(ostream &os);

    void output_hands_eng(ostream &os);
    void output_hands_ja(ostream &os);

    // ビジュアライザーのための出力
    void output_for_visualizer(ostream &os);

    // 駒を棋譜に従って移動します
    void move_piece(const PlayLog &mv);
    // 駒を棋譜に従ってUNDOします
    void reverse_move_piece(const PlayLog &mv);

    // 現在の合法手一覧を作成します。
    int gen_legal(int turn, vector<PlayLog> &pl);
    int gen_legal2(int turn, PlayLog *out);
    
    // 合法手(ただし相手の駒を取るもののみ)
    int gen_legal_take_piece(int turn, PlayLog *pl);
  

    /** 王手判定
     *
     * 王手かどうかを判定する
     * @param[in] turn 手番
     * @return 王手をかけている駒の数
     */ 

    int is_checked(int turn, uint32 my_place, uint32 all_place);
    
    int is_checked(int turn);
    // ある地点にある駒を返します
    // 駒が存在しない場合は-1が帰ります
    int get_piece(int turn, int place){
      int shift = turn << 5;
      for(int i = 0; i < MAX_PIECES; i++){
        if(((bit_board[i].all_board >> shift) >> place) & 1){
          return i;
        }
      }
      return -1;
    }

    int get_hand(int x){
      return hand[x];
    }

    // 評価値を計算する
    int calc_evaluation_value(int turn){
      //return calc_evaluation_value_naive(turn);
      return evaluation_value[turn];
    }

    // 評価値を再計算する
    int calc_evaluation_value_naive(int turn);
    bool is_lose(int turn ){
      if(((bit_board[OU].all_board >> (turn<<5))&0b1111111111111111111111111) == 0)return true;
      return false;
    }

    // 配列にする
    // 戻り値：配列の大きさ
    int to_char_array(char *ary){
      if(ary){
        for(int i=0;i<MAX_PIECES;i++){
          *((uint64*)(ary + i * 8)) = bit_board[i].all_board; // バグ怖い
        }
        for(int j = 0; j < 2; j++){
          *((uint16*)(ary + MAX_PIECES * 8 + j * 2)) = hand[j]; // バグ怖い
        }
      }
      return 84;
    }
    // 配列から生成する
    static Board55 from_char_array(char *ary){
      Board55 bb;
      for(int i=0;i<MAX_PIECES;i++){
        bb.bit_board[i].all_board = *((uint64*)(ary + i * 8)); // バグ怖い
      }
      for(int j = 0; j < 2; j++){
        bb.hand[j] = *((uint16*)(ary + MAX_PIECES * 8 + j * 2)); // バグ怖い
      }
      bb.calc_evaluation_value_naive(0);
      bb.calc_evaluation_value_naive(1);

      bb.get_hash_naive();
      return bb;
    }
    string move_to_string_ja(PlayLog p){
      string kan[5] = {"一","二","三","四","五"};
      string num[5] = {"１","２","３","４","５"};
      string s;
      string forward_pos = num[4 - p.get_forward_pos() % 5] + kan[p.get_forward_pos() / 5];
      string piece = PIECE_NAME_JA[p.get_type()];
      if(p.get_back_pos() == PlayLog::PUT_PIECE){
        return forward_pos + piece + "打";
      }else{
        string back_pos = num[4 - p.get_back_pos() % 5] + kan[p.get_back_pos() / 5];
        return /* back_pos + "->"+ */ forward_pos + piece + (p.get_promote()?"成": "");
      }
    }
};

#endif
