#include "Board55.hpp"
#include "HI.magic.hpp"
#include "KA.magic.hpp"
#include "EvaluationValue.hpp"
#include <cstdlib>
#include <exception>
#include <string>
#include <stdexcept>

const int BIT25=0b1111111111111111111111111;
// 各駒の評価
int piece_score[25] = {100,700,1000,1100,900,1000,700,699,600,10000000};

// 歩のおけない場所
int CANNNOT_PUT_FU[2]={
  0b0000011111111111111111111,
  0b1111111111111111111100000
}; // 歩の置けない場所

int CANNNOT_PUT_2FU[25]={
  0b1000010000100001000010000,
  0b0100001000010000100001000,
  0b0010000100001000010000100,
  0b0001000010000100001000010,
  0b0000100001000010000100001,
  0b1000010000100001000010000,
  0b0100001000010000100001000,
  0b0010000100001000010000100,
  0b0001000010000100001000010,
  0b0000100001000010000100001,
  0b1000010000100001000010000,
  0b0100001000010000100001000,
  0b0010000100001000010000100,
  0b0001000010000100001000010,
  0b0000100001000010000100001,
  0b1000010000100001000010000,
  0b0100001000010000100001000,
  0b0010000100001000010000100,
  0b0001000010000100001000010,
  0b0000100001000010000100001,
  0b1000010000100001000010000,
  0b0100001000010000100001000,
  0b0010000100001000010000100,
  0b0001000010000100001000010,
  0b0000100001000010000100001
};

const string Board55::PIECE_NAME_ENG[10]=  {
  "FU",
  "TO",
  "HI",
  "RY",
  "KA",
  "UM",
  "GI",
  "NG",
  "KI",
  "OU"
};

const string Board55::PIECE_NAME_JA[10]=  {
  "歩",
  "と",
  "飛",
  "龍",
  "角",
  "馬",
  "銀",
  "全",
  "金",
  "王"
};

const int Board55::HAND_TO_PIECE[5]={
  FU,
  HI,
  KA,
  GI,
  KI
};

const int Board55::HAND_NO[10]={
  hFU,
  hFU,
  hHI,
  hHI,
  hKA,
  hKA,
  hGI,
  hGI,
  hKI,
  hKI
};

const int REV_PROMOTE[10]={
  Board55::FU,
  Board55::FU,
  Board55::HI,
  Board55::HI,
  Board55::KA,
  Board55::KA,
  Board55::GI,
  Board55::GI,
  Board55::KI,
  Board55::OU
};

const int Board55::PROMOTE[10]={
  TO,
  TO,
  RY,
  RY,
  UM,
  UM,
  NG,
  NG,
  KI,
  OU
};

const int Board55::CAN_PROMOTE[2][25]={
  {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1}
};
const int Board55::MAGIC_BIT_BOARD[2][25] = {
  {100730888,268470849,5261860,35753984,20514,9438274,6309898,8533537,594587912,16916616,655504,2621552,304349256,771817569,2102672,272576,66097,50436097,1141901867,217232,67275290,1090523265,8814784,2162720,54016},
  //角のマジックビットード
  {71304268,4260928,605029633,1107759120,565267,18089188,541216,2130952,22053008,814760458,1208029312,578830722,4720800,66056,41288,270796824,16922,8462368,33563650,1073746180,32901,1121076290,269764616,578824204,1342181648}
  // 飛車のマジックボード
};
const int Board55::ATTACK_BIT_BOARD[2][MAX_PIECES][25]={
  {
    {0, 0, 0, 0, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288},
    {34, 69, 138, 276, 520, 1091, 2215, 4430, 8860, 16664, 34912, 70880, 141760, 283520, 533248, 1117184, 2268160, 4536320, 9072640, 17063936, 2195456, 5472256, 10944512, 21889024, 9175040},
    {33838, 67660, 135306, 270598, 541198, 34240, 67968, 135488, 270528, 541120, 47136, 77888, 141440, 268544, 539136, 459808, 395328, 331904, 205056, 475648, 14713888, 12650560, 10621056, 6562048, 15221248},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {266304, 8320, 320, 2176, 69888, 133120, 266240, 10240, 69632, 139264, 65600, 131200, 328000, 131200, 262400, 2176, 4352, 10240, 4160, 8320, 69888, 139264, 327680, 133120, 266304},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {64, 160, 320, 640, 256, 2051, 5127, 10254, 20508, 8216, 65632, 164064, 328128, 656256, 262912, 2100224, 5250048, 10500096, 21000192, 8413184, 98304, 229376, 458752, 917504, 786432},
    {34, 69, 138, 276, 520, 1091, 2215, 4430, 8860, 16664, 34912, 70880, 141760, 283520, 533248, 1117184, 2268160, 4536320, 9072640, 17063936, 2195456, 5472256, 10944512, 21889024, 9175040},
    {34, 69, 138, 276, 520, 1091, 2215, 4430, 8860, 16664, 34912, 70880, 141760, 283520, 533248, 1117184, 2268160, 4536320, 9072640, 17063936, 2195456, 5472256, 10944512, 21889024, 9175040},
    {98, 229, 458, 916, 776, 3139, 7335, 14670, 29340, 24856, 100448, 234720, 469440, 938880, 795392, 3214336, 7511040, 15022080, 30044160, 25452544, 2195456, 5472256, 10944512, 21889024, 9175040}
  },{
    {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 0, 0, 0, 0, 0},
    {98, 229, 458, 916, 776, 3137, 7330, 14660, 29320, 24848, 100384, 234560, 469120, 938240, 795136, 3212288, 7505920, 15011840, 30023680, 25444352, 2129920, 5308416, 10616832, 21233664, 8912896},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {96, 224, 448, 896, 768, 3074, 7173, 14346, 28692, 24584, 98368, 229536, 459072, 918144, 786688, 3147776, 7345152, 14690304, 29380608, 25174016, 65536, 163840, 327680, 655360, 262144},
    {98, 229, 458, 916, 776, 3137, 7330, 14660, 29320, 24848, 100384, 234560, 469120, 938240, 795136, 3212288, 7505920, 15011840, 30023680, 25444352, 2129920, 5308416, 10616832, 21233664, 8912896},
    {98, 229, 458, 916, 776, 3137, 7330, 14660, 29320, 24848, 100384, 234560, 469120, 938240, 795136, 3212288, 7505920, 15011840, 30023680, 25444352, 2129920, 5308416, 10616832, 21233664, 8912896},
    {98, 229, 458, 916, 776, 3139, 7335, 14670, 29340, 24856, 100448, 234720, 469440, 938880, 795392, 3214336, 7511040, 15022080, 30044160, 25452544, 2195456, 5472256, 10944512, 21889024, 9175040}
  }
};
Board55::Board55(){
  random_number_generate();
  hand[0]=hand[1]=0;
  evaluation_value[0]=evaluation_value[1]=0;
  memset(bit_board,0,sizeof(bit_board));
  init_evaluation_value();
}

string Board55::get_piece_name(int a){
  return PIECE_NAME_ENG[static_cast<int>(a)];
}

int Board55::get_piece_from_name(string name){
  for(int i = 0 ; i < MAX_PIECES; i++){
    if(name == PIECE_NAME_ENG[i]){
      return static_cast<Pieces>(i);
    } 
  }
  throw std::runtime_error("Invalid piece name");
}

inline uint64 Board55::white(uint64 t){
  return t&0b1111111111111111111111111;
}

inline uint64 Board55::black(uint64 t){
  return white(t>>32);
}

inline void Board55::remove_hands(uint64 turn, int piece){
  hand[turn] -= 1 << (HAND_NO[piece] << 1);
}
void Board55::add_hands(uint64 turn, int piece){
  hand[turn] += 1 << (HAND_NO[piece] << 1);
}

void Board55::move_piece(const PlayLog &mv){
  if(mv.put_piece()){ // 駒を置く場合
    // 現在の持駒からその駒を消去する
    evaluation_value[mv.get_turn()] -= evaluation_value_of_hand_cache[hand[mv.get_turn()]];
    board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
    remove_hands(mv.get_turn(), mv.get_type());
    board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
    evaluation_value[mv.get_turn()] += evaluation_value_of_hand_cache[hand[mv.get_turn()]];

    // その駒を座標に配置する
    bit_board[mv.get_type()].all_board |= (1LL << mv.get_forward_pos()) << (mv.get_turn() << 5);
    board_hash ^= piece_hash[mv.get_turn()][mv.get_type()][mv.get_forward_pos()];
    evaluation_value[mv.get_turn()] += evaluation_value_of_piece[mv.get_turn()][mv.get_type()][mv.get_forward_pos()];
  } else { // 駒を移動する場合
    bit_board[mv.get_type()].all_board &= ~((1LL << mv.get_back_pos()) << (mv.get_turn() << 5));// 現在の駒を消去する
    board_hash ^= piece_hash[mv.get_turn()][mv.get_type()][mv.get_back_pos()];
    evaluation_value[mv.get_turn()] -= evaluation_value_of_piece[mv.get_turn()][mv.get_type()][mv.get_back_pos()];

    int next_type = mv.get_type();
    if(mv.get_promote()){
      // 成る場合、駒の種類を成りにする
      next_type=PROMOTE[next_type];
    }

    // 駒を設置する
    bit_board[next_type].all_board |= (1LL << mv.get_forward_pos()) << (mv.get_turn() << 5);
    board_hash ^= piece_hash[mv.get_turn()][next_type][mv.get_forward_pos()];
    evaluation_value[mv.get_turn()] += evaluation_value_of_piece[mv.get_turn()][next_type][mv.get_forward_pos()];

    // 駒を取る
    int take_type = mv.get_take_type();
    if(take_type != PlayLog::NOT_GET){
      bit_board[take_type].all_board &= ~((1LL << mv.get_forward_pos()) << ((1 - mv.get_turn()) << 5));// 現在の駒を消去する
      board_hash ^= piece_hash[1 - mv.get_turn()][take_type][mv.get_forward_pos()];
      evaluation_value[1 - mv.get_turn()] -= evaluation_value_of_piece[1 - mv.get_turn()][take_type][mv.get_forward_pos()];

      // cout << "Add Hands : "<<piece_score[REV_PROMOTE[take_type]] << endl;
      evaluation_value[mv.get_turn()] -= evaluation_value_of_hand_cache[hand[mv.get_turn()]];
      board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
      add_hands(mv.get_turn(), take_type);
      board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
      evaluation_value[mv.get_turn()] += evaluation_value_of_hand_cache[hand[mv.get_turn()]];
    }
  }
}

void Board55::reverse_move_piece(const PlayLog &mv){
  if(mv.put_piece()){ // 駒を置く場合
    // 現在の持駒からその駒を追加する
    evaluation_value[mv.get_turn()] -= evaluation_value_of_hand_cache[hand[mv.get_turn()]];
    board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
    add_hands(mv.get_turn(), mv.get_type());
    board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
    evaluation_value[mv.get_turn()] += evaluation_value_of_hand_cache[hand[mv.get_turn()]];
    // その駒を消去
    
    bit_board[mv.get_type()].all_board &= ~((1LL << mv.get_forward_pos()) << (mv.get_turn() << 5));
    board_hash ^= piece_hash[mv.get_turn()][mv.get_type()][mv.get_forward_pos()];
    evaluation_value[mv.get_turn()] -= evaluation_value_of_piece[mv.get_turn()][mv.get_type()][mv.get_forward_pos()];
  } else { // 駒を移動する場合
    bit_board[mv.get_type()].all_board |= ((1LL << mv.get_back_pos()) << (mv.get_turn() << 5));// 現在の駒を追加する
    board_hash ^= piece_hash[mv.get_turn()][mv.get_type()][mv.get_back_pos()];
    evaluation_value[mv.get_turn()] += evaluation_value_of_piece[mv.get_turn()][mv.get_type()][mv.get_back_pos()];

    int next_type = mv.get_type();
    if(mv.get_promote()){
      // 成る場合、駒の種類を成りにする
      next_type=PROMOTE[next_type];
    }

    // 駒を消去する
    bit_board[next_type].all_board &= ~((1LL << mv.get_forward_pos()) << (mv.get_turn() << 5));
    board_hash ^= piece_hash[mv.get_turn()][next_type][mv.get_forward_pos()];
    evaluation_value[mv.get_turn()] -= evaluation_value_of_piece[mv.get_turn()][next_type][mv.get_forward_pos()];

    // 駒を戻す
    int take_type = mv.get_take_type();
    if(take_type != PlayLog::NOT_GET){
      bit_board[take_type].all_board |= ((1LL << mv.get_forward_pos()) << ((1 - mv.get_turn()) << 5));// 現在の駒を回復する
      board_hash ^= piece_hash[1 - mv.get_turn()][take_type][mv.get_forward_pos()];
      evaluation_value[1 - mv.get_turn()] += evaluation_value_of_piece[1 - mv.get_turn()][take_type][mv.get_forward_pos()];

      // 持駒を消去する
      evaluation_value[mv.get_turn()] -= evaluation_value_of_hand_cache[hand[mv.get_turn()]];
      board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
      remove_hands(mv.get_turn(), take_type);
      board_hash ^= hands_hash[mv.get_turn()][hand[mv.get_turn()]];
      evaluation_value[mv.get_turn()] += evaluation_value_of_hand_cache[hand[mv.get_turn()]];
    }
  }
}


void Board55::output_board(ostream &os,const string PIECE_NAME[10]){
  os << "+---+---+---+---+---+" << '\n';
  for(int y = 0;y < 5;y++){
    os << "|";
    for(int x = 0;x < 5;x++){
      int pos = bit_pos(y, x);
      bool found = false;
      for(int k = 0;k < MAX_PIECES;k++){
        if(((bit_board[k].all_board) >> pos) & 1){
          os << " " << PIECE_NAME[k];
          found=true;
          break;
        }
      }

      for(int k = 0;k < MAX_PIECES;k++){
        if(((bit_board[k].all_board >> 32) >> pos) & 1){
          os << "v" << PIECE_NAME[k];
          found=true;
          break;
        }
      }
      if(!found){
        os << "   ";
      }
      os << "|";
    }
    os << '\n';
    os << "+---+---+---+---+---+" << '\n';
  }
  os << flush;
}

void Board55::output_board_eng(ostream &os){
  output_board(os, PIECE_NAME_ENG);
}

void Board55::output_board_ja(ostream &os){
  output_board(os, PIECE_NAME_JA);
}

void Board55::init_board(){
  hand[0]=hand[1]=0;
  memset(bit_board,0,sizeof(bit_board));

  bit_board[FU].all_board = 1 << 15;
  bit_board[OU].all_board = 1 << 20;
  bit_board[KI].all_board= 1 << 21;
  bit_board[GI].all_board  = 1 << 22;
  bit_board[KA].all_board = 1 << 23;
  bit_board[HI].all_board = 1 << 24;

  bit_board[FU].all_board |= (1LL << 9) << 32;
  bit_board[HI].all_board |= (1LL << 0) << 32;
  bit_board[KA].all_board |= (1LL << 1) << 32;
  bit_board[GI].all_board |= (1LL << 2) << 32;
  bit_board[KI].all_board |= (1LL << 3) << 32;
  bit_board[OU].all_board |= (1LL << 4) << 32;

  calc_evaluation_value_naive(0);
  calc_evaluation_value_naive(1);

  get_hash_naive();
}

void Board55::output_hands_eng(ostream &os){
  os << "White Hands :";
  for(int i = 0;i < MAX_HANDS;i++){
    int h_count = (hand[0] >> (i << 1)) & 3;
    for(int j = 0;j < h_count;j++){
      os << ' ' << PIECE_NAME_ENG[HAND_TO_PIECE[i]];
    }
  }
  os << '\n';

  os << "Black Hands :";
  for(int i = 0;i < MAX_HANDS;i++){
    int h_count = (hand[1] >> (i << 1)) & 3;
    for(int j = 0;j < h_count;j++){
      os << ' ' << PIECE_NAME_ENG[HAND_TO_PIECE[i]];
    }
  }
  os << flush;
}

void Board55::output_hands_ja(ostream &os){
  os << "先手持ち駒 :";
  for(int i = 0;i < MAX_HANDS;i++){
    int h_count = (hand[0] >> (i << 1)) & 3;
    for(int j = 0;j < h_count;j++){
      os << ' ' << PIECE_NAME_JA[HAND_TO_PIECE[i]];
    }
  }
  os << '\n';

  os << "後手持ち駒 :";
  for(int i = 0;i < MAX_HANDS;i++){
    int h_count = (hand[1] >> (i << 1)) & 3;
    for(int j = 0;j < h_count;j++){
      os << ' ' << PIECE_NAME_JA[HAND_TO_PIECE[i]];
    }
  }
  os << endl;
}
void Board55::output_for_visualizer(ostream &os){
  os << "VIS:";
  for(int y = 0;y < 5;y++){
    for(int x = 0;x < 5;x++){
      bool found = false;
      int pos = bit_pos(y, x);
      for(int k = 0;k < MAX_PIECES;k++){
        if(((bit_board[k].all_board) >> pos) & 1){
          os << static_cast<char>('0'+k);
          found=true;
          break;
        }
      }

      for(int k = 0;k < MAX_PIECES;k++){
        if(((bit_board[k].all_board >> 32) >> pos) & 1){
          os << static_cast<char>('0'+k+10);
          found=true;
          break;
        }
      }
      if(!found){
        os << static_cast<char>('0'-1);
      }
    }
  }
  os<<endl;
}

#define NORMAL_ATTACK(i) \
    attack_bit = ATTACK_BIT_BOARD[turn][i][place];
#define KA_ATTACK \
    attack_bit = magic_bit_board_KA[place][(((uint64)MAGIC_BIT_BOARD[0][place]*(all_place&(ATTACK_BIT_BOARD[0][KA][place])))>>20)&255]; 
#define UM_ATTACK \
    KA_ATTACK; attack_bit |= ATTACK_BIT_BOARD[turn][OU][place];
#define HI_ATTACK \
    attack_bit = magic_bit_board_HI[place][(((uint64)MAGIC_BIT_BOARD[1][place]*(all_place&(ATTACK_BIT_BOARD[0][HI][place])))>>20)&255];
#define RY_ATTACK\
    HI_ATTACK; attack_bit |= ATTACK_BIT_BOARD[turn][OU][place];
#define NORMAL_CHECKED(i) \
    NORMAL_ATTACK(i); attack_bit &= bit_board[i].board[1 - turn];
int Board55::is_checked(int turn){
  uint32 my_place = 0, all_place = 0;
  for(int i = 0; i < MAX_PIECES; i++){
    all_place |= bit_board[i].board[1 - turn];
    my_place |= bit_board[i].board[turn];
  }
  all_place |= my_place;
  return is_checked(turn, my_place, all_place);
}

int Board55::is_checked(int turn, uint32 my_place, uint32 all_place){
  uint32 place  = __builtin_ctz(bit_board[OU].board[turn]); // 王の場所
  int ret = 0; // 王手の数
  uint32 attack_bit;

  NORMAL_CHECKED(FU);
  ret += __builtin_popcount(attack_bit);
  NORMAL_CHECKED(TO);
  ret += __builtin_popcount(attack_bit);
  NORMAL_CHECKED(GI);
  ret += __builtin_popcount(attack_bit);
  NORMAL_CHECKED(NG);
  ret += __builtin_popcount(attack_bit);
  NORMAL_CHECKED(KI);
  ret += __builtin_popcount(attack_bit);
  NORMAL_CHECKED(OU);
  ret += __builtin_popcount(attack_bit);
  
  HI_ATTACK;
  attack_bit &= bit_board[HI].board[1 - turn];
  ret += __builtin_popcount(attack_bit);
  
  RY_ATTACK;
  attack_bit &= bit_board[RY].board[1 - turn];
  ret += __builtin_popcount(attack_bit);
   
  KA_ATTACK;
  attack_bit &= bit_board[KA].board[1 - turn];
  ret += __builtin_popcount(attack_bit);
  
  UM_ATTACK;
  attack_bit &= bit_board[UM].board[1 - turn];
  ret += __builtin_popcount(attack_bit);

  return ret;
}
#undef NORMAL_CHECKED
#define GET_WORK(X, cmds) \
  cmds; \
  attack_bit &= bit_board[X].board[1 - turn]; \
  tmp = attack_bit; \
  while(tmp>0){get_piece = X; get_place =__builtin_ctz(tmp); work[__builtin_ctz(tmp)]++; tmp = (tmp - 1) & tmp;}
#define GEN_ONE(i, cmds) \
  tmp = bit_board[i].board[turn]; \
  while(tmp){ \
    place = __builtin_ctz(tmp);\
    cmds; \
    attack_bit &= move_place_candi; \
    while(attack_bit > 0) { \
      int attack_pos = __builtin_ctz(attack_bit); \
      int get = PlayLog::NOT_GET; \
      if(all_place >> attack_pos & 1){ \
        for(int j = 0; j < MAX_PIECES; j++){ \
          if(bit_board[j].board[1 - turn] & (1 << attack_pos)){ \
            get = j; break; \
          } \
        } \
      } \
      int PROMOTE = CAN_PROMOTE[turn][attack_pos]; \
      out[count++] = PlayLog(turn, i, place, attack_pos, PROMOTE, get); \
      if(i == GI && PROMOTE)\
        out[count++] = PlayLog(turn, i, place, attack_pos, PROMOTE, get); \
      attack_bit &= attack_bit - 1; \
    } \
    tmp &= tmp - 1; \
  }
#define GEN_NORMAL \
        GEN_ONE(FU, NORMAL_ATTACK(FU)); \
        GEN_ONE(GI, NORMAL_ATTACK(GI)); \
        GEN_ONE(NG, NORMAL_ATTACK(NG)); \
        GEN_ONE(TO, NORMAL_ATTACK(TO)); \
        GEN_ONE(KI, NORMAL_ATTACK(KI)); \
        GEN_ONE(OU, NORMAL_ATTACK(OU)); \
        GEN_ONE(HI, HI_ATTACK); \
        GEN_ONE(RY, RY_ATTACK); \
        GEN_ONE(KA, KA_ATTACK); \
        GEN_ONE(UM, UM_ATTACK); 
#define GEN_HANDS \
  for(int i = 0; i < 5; i++){ \
    int piece = HAND_TO_PIECE[i]; \
    if((hand[turn] >> (i << 1))&3) { \
      int bit_pos = move_place_candi;    \
      if(i == 0){ \
        tmp = bit_board[FU].board[turn]; \
        bit_pos &= CANNNOT_PUT_FU[turn]; \
        while(tmp > 0){ \
          int j = __builtin_ctz(tmp); \
          bit_pos &= ~CANNNOT_PUT_2FU[j]; \
          tmp &= tmp - 1; \
        } \
      } \
      while(bit_pos > 0){ \
        int j = __builtin_ctz(bit_pos); \
        out[count++] = PlayLog(turn, piece, PlayLog::PUT_PIECE, j, false); \
        bit_pos &= bit_pos - 1; \
      } \
    } \
  }
int Board55::gen_legal2(int turn, PlayLog *out){
  if(bit_board[OU].board[turn] == 0){
    return 0;
  }
  uint32 my_place = 0, all_place = 0;
  for(int i = 0; i < MAX_PIECES; i++){
    all_place |= bit_board[i].board[1 - turn];
    my_place |= bit_board[i].board[turn];
  }
  all_place |= my_place;
  // 王手を判定する
  int ret, count = 0;
  ret = is_checked(turn, my_place, all_place);
  if(ret){
    int work[25] = {}; // 各マスの利きの数
    uint32 attack_bit;
    uint32 place  = __builtin_ctz(bit_board[OU].board[turn]); // 王の場所
    uint32 tmp;
    uint32 tar;
    uint32 get_piece = -1; // 一枚しか無い場合，利きの駒の種類
    uint32 get_place = -1;
    GET_WORK(FU, NORMAL_ATTACK(FU));
    GET_WORK(TO, NORMAL_ATTACK(TO));
    GET_WORK(GI, NORMAL_ATTACK(GI));
    GET_WORK(NG, NORMAL_ATTACK(NG));
    GET_WORK(KI, NORMAL_ATTACK(KI));
    GET_WORK(OU, NORMAL_ATTACK(OU));
    GET_WORK(HI, HI_ATTACK);
    GET_WORK(RY, RY_ATTACK);
    GET_WORK(KA, KA_ATTACK);
    GET_WORK(UM, UM_ATTACK);
    
    // 王手されているのなら、回避する手を生成
    if(ret == 1){
      // 他の駒で取る・割り込み・打ち込みを考慮
      // 他の駒で取る
      uint32 move_place_candi = 1 << get_place; // 移動場所の候補
      if(get_piece == HI || get_piece == RY){
        HI_ATTACK;
        tmp = attack_bit;
        swap(place, get_place);
        HI_ATTACK;
        swap(place, get_place);
        tmp &= attack_bit;
        move_place_candi |= tmp;
      }else if(get_piece == KA || get_piece == UM){
        KA_ATTACK;
        tmp = attack_bit;
        swap(place, get_place);
        KA_ATTACK;
        swap(place, get_place);
        tmp &= attack_bit;
        move_place_candi |= tmp;
      }
      // 普通に移動手を生成する(ただし，移動先候補はmove_piece_candiのみ)
      move_place_candi &= ~my_place;
      GEN_NORMAL;
      move_place_candi &= ~all_place;
      GEN_HANDS;
    }
    place  = __builtin_ctz(bit_board[OU].board[turn]);

    // 王を移動させる
    uint32 moves = ATTACK_BIT_BOARD[turn][OU][place] & (~my_place);
    if(ret == 1) moves &= ~(1 << get_place);
    uint32 get;
    tmp = moves;
    while(tmp) {
      tar = __builtin_ctz(tmp);
      if(work[tar] == 0){
        get = PlayLog::NOT_GET;
        if(all_place >> tar & 1){
          for(int j = 0; j < MAX_PIECES; j++){
            if(bit_board[j].board[1 - turn] & (1 << tar)){
              get = j;
              break;
            }
          }
        }  
        out[count++]=(PlayLog(turn, OU, place, tar, 0, get));
      }
      tmp = (tmp - 1) & tmp;
    }
  }else{
    uint32 attack_bit, place, tmp;
    // そうでないのならば、飛車角は別個にして合法手を生成
    uint32 move_place_candi = (~my_place) & BIT25;
    GEN_NORMAL;
    move_place_candi = (~all_place) & BIT25;
    GEN_HANDS;
  }
  return count;
}
#undef GET_WORK
#undef NORMAL_ATTACK
#undef KA_ATTACK
#undef UM_ATTACK
#undef HI_ATTACK
#undef RY_ATTACK
int Board55::gen_legal(int turn, vector<PlayLog> &pl){
  int shift=turn<<5; // シフト量
  int my_place = 0; // 自分の駒がない場所
  int all_place = 0;
  int pl_cnt=0;

  if(((bit_board[OU].all_board >> shift)&0b1111111111111111111111111) == 0)return 0;
  for(int i = 0; i < MAX_PIECES; i++){
    my_place |= (bit_board[i].all_board >> shift) & 0b1111111111111111111111111;
    all_place |= (bit_board[i].all_board >> (32-shift)) & 0b1111111111111111111111111;
  }
  all_place |= my_place;
  my_place = ~my_place;
  for(int i = 0; i < MAX_PIECES; i++){
    int tmp = (bit_board[i].all_board >> shift) & 0b1111111111111111111111111;
    while(tmp > 0){
      int place = __builtin_ctz(tmp);
      // 攻撃場所
      int attack_bit = 0;
      if(i == HI || i == RY){
        attack_bit = magic_bit_board_HI[place][(((long long)MAGIC_BIT_BOARD[1][place]*(all_place&(ATTACK_BIT_BOARD[0][HI][place])))>>20)&255]&my_place;
        if( i == RY ){
          attack_bit |= ATTACK_BIT_BOARD[turn][OU][place] &my_place;
        }
      }else if(i == KA || i == UM){
        attack_bit = magic_bit_board_KA[place][(((long long)MAGIC_BIT_BOARD[0][place]*(all_place&(ATTACK_BIT_BOARD[0][KA][place])))>>20)&255]&my_place;
        if( i == UM ){
          attack_bit |= ATTACK_BIT_BOARD[turn][OU][place] &my_place;
        }
      }else{
        attack_bit = ATTACK_BIT_BOARD[turn][i][place] & my_place;
      }

      // 実際に攻撃してみる
      while (attack_bit > 0) {
        int attack_pos = __builtin_ctz(attack_bit);
        int target = -1;
        if(all_place >> attack_pos & 1){
          for(int j = 0; j < MAX_PIECES; j++){
            if((bit_board[j].all_board>>((!turn)<<5)) & (1<<attack_pos)){
              target = j;
              break;
            }
          }
        }
        if(target == -1){
          target = PlayLog::NOT_GET;
        }
        int PROMOTE = CAN_PROMOTE[turn][attack_pos];
        pl[pl_cnt++]=(PlayLog(turn, i, place, attack_pos, PROMOTE, target));
        if(i == Board55::GI && PROMOTE){
          // 銀の場合は成らない方が良い場合もある
          //pl
          pl[pl_cnt++]=PlayLog(turn, i, place, attack_pos, false, target);
        }
        attack_bit &= attack_bit - 1;
      }
      tmp &= tmp - 1;
    }
  }
  // もちごまを考慮する
  for(int i = 0; i< 5;i++){
    int piece = HAND_TO_PIECE[i];
    if((hand[turn] >> (HAND_NO[piece] << 1)) & 3){
      // もちごまがある
      int bit_pos = (~all_place) & 0b1111111111111111111111111;
      if(i == 0) {
        int tmp=(bit_board[i].all_board >> (shift)) & 0b1111111111111111111111111;
        bit_pos &= CANNNOT_PUT_FU[turn];
        while(tmp>0){
          int j = __builtin_ctz(tmp);
          bit_pos &= ~CANNNOT_PUT_2FU[j];
          tmp &= tmp-1;
        }
      }
      while (bit_pos > 0) {
        int j = __builtin_ctz(bit_pos);
        pl[pl_cnt++]=PlayLog(turn, piece, PlayLog::PUT_PIECE, j, false);
        bit_pos &= bit_pos - 1;
      }
    }
  }
  return pl_cnt;
}

int Board55::gen_legal_take_piece(int turn, PlayLog *pl){
  int shift=turn<<5; // シフト量
  int my_place = 0; // 自分の駒がない場所
  int all_place = 0;
  int pl_cnt=0;

  if(((bit_board[OU].all_board >> shift)&0b1111111111111111111111111) == 0)return 0;
  for(int i = 0; i < MAX_PIECES; i++){
    my_place |= (bit_board[i].all_board >> shift) & 0b1111111111111111111111111;
    all_place |= (bit_board[i].all_board >> (32-shift)) & 0b1111111111111111111111111;
  }
  all_place |= my_place;
  my_place = ~my_place;
  for(int i = 0; i < MAX_PIECES; i++){
    int tmp = (bit_board[i].all_board >> shift) & 0b1111111111111111111111111;
    while(tmp > 0){
      int place = __builtin_ctz(tmp);
      // 攻撃場所
      int attack_bit = 0;
      if(i == HI || i == RY){
        attack_bit = magic_bit_board_HI[place][(((long long)MAGIC_BIT_BOARD[1][place]*(all_place&(ATTACK_BIT_BOARD[0][HI][place])))>>20)&255]&my_place;
        if( i == RY ){
          attack_bit |= ATTACK_BIT_BOARD[turn][OU][place] &my_place;
        }
      }else if(i == KA || i == UM){
        attack_bit = magic_bit_board_KA[place][(((long long)MAGIC_BIT_BOARD[0][place]*(all_place&(ATTACK_BIT_BOARD[0][KA][place])))>>20)&255]&my_place;
        if( i == UM ){
          attack_bit |= ATTACK_BIT_BOARD[turn][OU][place] &my_place;
        }
      }else{
        attack_bit = ATTACK_BIT_BOARD[turn][i][place] & my_place;
      }
      attack_bit &= all_place;
      // 実際に攻撃してみる
      while (attack_bit > 0) {
        int attack_pos = __builtin_ctz(attack_bit);
        int target = -1;
        if(all_place >> attack_pos & 1){
          for(int j = 0; j < MAX_PIECES; j++){
            if((bit_board[j].all_board>>((!turn)<<5)) & (1<<attack_pos)){
              target = j;
              break;
            }
          }
        }
        if(target == -1){
          target = PlayLog::NOT_GET;
        }
        int PROMOTE = CAN_PROMOTE[turn][attack_pos];
        *(pl++)=(PlayLog(turn, i, place, attack_pos, PROMOTE, target));
        pl_cnt++;
        if(i == Board55::GI && PROMOTE){
          *(pl++)=PlayLog(turn, i, place, attack_pos, false, target);
          pl_cnt++;
        }
        attack_bit &= attack_bit - 1;
      }
      tmp &= tmp - 1;
    }
  }
  return pl_cnt;
}

int Board55::calc_evaluation_value_naive(int turn){
  evaluation_value[turn] = 0;
  int shift=turn<<5;
  for(int i=0;i<MAX_PIECES;i++){
    int tmp = (bit_board[i].all_board >> shift & 0b1111111111111111111111111);
    while(tmp > 0){
      int pos = __builtin_ctz(tmp);
      evaluation_value[turn] += evaluation_value_of_piece[turn][i][pos];
      tmp &= tmp - 1;
    }
  }
  evaluation_value[turn] += evaluation_value_of_hand_cache[hand[turn]];
  return evaluation_value[turn];
}

void Board55::random_number_generate(){
  vector<int> v(3);
  v[0] = 27778473;  v[1] = 98336442; v[2] = 64646456; // seed_value
  seed_seq seed(v.begin(), v.end());
  std::mt19937 engine(seed);
  uniform_int_distribution<board_hash_t> distribution(0, (board_hash_t)ULONG_LONG_MAX);
  auto rng = bind(distribution, engine);
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < MAX_PIECES; j++){
      for(int k = 0; k < 25; k++){
        piece_hash[i][j][k] = rng() | (rng() << 32);
      }
    }
    for(int j = 0; j < 1 << (MAX_HANDS * 2); j++){
      hands_hash[i][j] = rng() | (rng() << 32);
    }
  }
}

board_hash_t Board55::get_hash_naive(){
  board_hash_t hash = 0;
  for(int turn = 0; turn < 2; turn++){
    // 標準
    for(int i = 0; i < MAX_PIECES; i++){
      int tmp=(bit_board[i].all_board >> (turn << 5));
      while(tmp > 0) {
        int j = __builtin_ctz(tmp);
        hash ^= piece_hash[turn][i][j];
        tmp &= tmp-1;
      }
    }
    hash ^= hands_hash[turn][hand[turn]];
  }
  return board_hash = hash;
}

void Board55::init_evaluation_value(){
  for(int i = 0; i < (1 << 10); i++){
    int value = 0;
    for(int j = 0; j < 5; j++){
      int tmp = (i >> (j << 2)) & 3;
      if(tmp >= 3)break;
      if(tmp == 0)continue;
      value += evaluation_value_of_hand[j][tmp - 1];
    }
    evaluation_value_of_hand_cache[i] = value;
  }
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 25; j++){
      evaluation_value_of_piece[1][i][j] = evaluation_value_of_piece[0][i][24-j];
    }
  }
}
