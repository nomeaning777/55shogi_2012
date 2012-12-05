#include "55shogi.hpp"
#include "PlayLog.hpp"
#include "Board55.hpp"
#include "Battle.hpp"
#include "ReadLine.hpp"
#include <ctime>
#include <cassert>
#include <unordered_map>
#include "CSADirectClient.hpp"
#include "Game.hpp"

#ifdef MPI_ENABLE
  #include "mpi.h"
  int mpi_rank;
  int mpi_p_num;
#endif

using namespace std;

std::unordered_map<board_hash_t, int> count_board; // 千日手用
std::unordered_map<board_hash_t, int> transposition[20];

static int debug=0;
vector<PlayLog> plss[50];
void iddfs(Board55 &board,int depth,int max_depth){
	debug++;
	if(depth>=max_depth)return;
	int cnt=board.gen_legal(depth&1,plss[depth]);
	for(int i = 0; i < cnt; i++){
		board.move_piece(plss[depth][i]);
		iddfs(board,depth+1,max_depth);
		board.reverse_move_piece(plss[depth][i]);
	}
}
PlayLog result[50];
const int INF=99999999;
int search(int turn,int depth,Board55 &board,int max_depth,int alpha = -INF,int beta = INF){
	debug++;
	if(depth>=max_depth){
		return board.calc_evaluation_value((turn+depth)&1);
	}
	int max = alpha;
	int cnt=board.gen_legal((turn+depth)&1,plss[depth]);

	if(cnt == 0)return -INF+depth;
	for(int i = 0; i < cnt; i++){
		board.move_piece(plss[depth][i]);

		int x = -search(turn,depth+1,board,max_depth,-beta,-max);
		board.reverse_move_piece(plss[depth][i]);
		if(x >= beta){return x;}
		if(x > max){
			max = x;
		}
	}
	return max;
}
#ifdef MPI_ENABLE
int mpi_search(int turn,Board55 *board_tmp,int max_depth){
  int mes = 1;
  MPI_Bcast((void*)&mes, 1, MPI_INTEGER4, 0, MPI_COMM_WORLD);
  if(mes == 0)return false; // 終了
  MPI_Bcast((void*)&turn, 1, MPI_INTEGER4, 0, MPI_COMM_WORLD);
  MPI_Bcast((void*)&max_depth, 1, MPI_INTEGER4, 0, MPI_COMM_WORLD);
  int size = (*board_tmp).to_char_array(0);
  char c_brd[size];
  // rank = 0なら送信
  static int moves[256];
  int move_cnt = 0;
  Board55 board;
  if(mpi_rank == 0){
    (*board_tmp).to_char_array(c_brd);
    MPI_Bcast((void*)c_brd, size, MPI_CHAR, 0, MPI_COMM_WORLD);
    board = *board_tmp;
    vector<PlayLog> tmp(256);
    int cnt=board.gen_legal((turn)&1, tmp);
    tmp.resize(cnt);
    for(int i = 0; i < cnt; i++){
      bool endless_move = false; // 千日手？
      board.move_piece(tmp[i]);
      if(count_board[board.get_hash()] == 3)endless_move = true;
      board.reverse_move_piece(tmp[i]);
      if(endless_move){
        tmp.erase(tmp.begin()+i);
        i--;
      }
    }
    cnt = tmp.size();
    random_shuffle(tmp.begin(),tmp.end());
    static uint32 sender[16][256];
    uint32 counter[mpi_p_num];
    for(int i = 0; i < mpi_p_num; i++)counter[i] = 0;
    for(int i = 0; i < cnt; i++){
      sender[i % mpi_p_num][counter[i % mpi_p_num]++] = tmp[i]();
    }
    // とりあえず送ってみる
    for(int i = 1; i < mpi_p_num; i++){
      MPI_Send(&counter[i], 1, MPI_INTEGER4, i, 2, MPI_COMM_WORLD);
      MPI_Send(sender[i], counter[i], MPI_INTEGER4, i, 1, MPI_COMM_WORLD);
    }
    move_cnt = counter[0];
    for(int i = 0; i < move_cnt; i++){
      moves[i] = sender[0][i];
    }
  }else{
    MPI_Status recv_status;
    MPI_Bcast((void*)c_brd, size, MPI_CHAR, 0, MPI_COMM_WORLD);
    board = Board55::from_char_array(c_brd);
    MPI_Recv(&move_cnt, 1, MPI_INTEGER4, 0, 2, MPI_COMM_WORLD, &recv_status);
    cout << "MoveCnt " << mpi_rank <<" , "<< move_cnt << endl;
    MPI_Recv(moves, move_cnt, MPI_INTEGER4, 0, 1, MPI_COMM_WORLD, &recv_status);
  }
  debug = 1;
  int max = -INF;
  uint32 next = 0;
  for(int i = 0; i < move_cnt; i++){
    board.move_piece(moves[i]);
    int x = -search(turn, 1, board, max_depth, -INF, -max);
    board.reverse_move_piece(moves[i]);
    if(x > max){
      max = x;
      next = moves[i];
    }
  }
  cout << "calc exited: "<< mpi_rank << endl;
  int r_debugs[mpi_p_num],r_scores[mpi_p_num], r_moves[mpi_p_num];
  MPI_Gather(&debug, 1, MPI_INTEGER4, r_debugs, 1, MPI_INTEGER4, 0 , MPI_COMM_WORLD);
  MPI_Gather(&max, 1, MPI_INTEGER4, r_scores, 1, MPI_INTEGER4, 0 , MPI_COMM_WORLD);
  MPI_Gather(&next, 1, MPI_INTEGER4, r_moves, 1, MPI_INTEGER4, 0 , MPI_COMM_WORLD);

  if(mpi_rank == 0){
    debug = 0;
    for(int i = 0; i < mpi_p_num; i++)debug+=r_debugs[i];
    for(int i = 1; i < mpi_p_num; i++){
      if(r_scores[i] > max){
        max = r_scores[i];
        next = r_moves[i];
      }
    }
    return next;
  }
  return true;
}

void mpi_exit(){
  int mes = 0;
  MPI_Bcast((void*)&mes, 1, MPI_INTEGER4, 0, MPI_COMM_WORLD);
}

#endif
int main(int argc, char* argv[]) {
#ifdef MPI_ENABLE
  MPI_Init(&argc, &argv);
  cout << sizeof(Board55) << endl;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_p_num);
  cout << "MPI Initalized: rank" << mpi_rank << endl;
  if(mpi_rank == 0){
  cout << "Processes: "<<mpi_p_num << endl;
#endif
  while(false){
    string s = ReadLine::readline("> ");
    if(ReadLine::is_eof())break;
    if(s == "exit") break;
    cout << "Command not Found: \"" << s << "\""<<endl;
  }
  CSADirectClient p("localhost", 4081);
  p.login("testclient");
  Shogi55::client(p, Game::get_instance());
  return 0;
	// ios_base::sync_with_stdio(false);
	Board55 board;
	board.init_board();
	// cin.tie(0);
	for(int i=0;i<50;i++){
		plss[i] = vector<PlayLog>(256);
	}
	board.output_board_ja(cout);
	board.output_hands_ja(cout);
	board.output_for_visualizer(cout);
  count_board[board.get_hash()]++;
	for(int i=0;i<100;i++){
		if(board.is_lose(i&1)){
			if(i&1){
				cout<<"先手の勝ちです"<<endl;
			}else{
				cout<<"後手の勝ちです"<<endl;
			}
			break;
		}
#ifndef MPI_ENABLE
		debug=0;
		clock_t start=clock();
		search(i&1,0,board,9);
		cout<<(int)(debug/((double)(clock()-start)/CLOCKS_PER_SEC))<<"NPS"<<endl;
		board.move_piece(*result);
#else
    double t_start = MPI_Wtime();
    uint32 ret = mpi_search(i&1 ,&board, 9);
    double t_end = MPI_Wtime();
    board.move_piece(ret);
    cout << (int)(debug/(t_end-t_start)) << "NPS" << endl;
#endif
		board.output_board_ja(cout);
		board.output_hands_ja(cout);
    cout << "Hash: "<< board.get_hash() << endl;
    cout << "Hash_Naive: "<< board.get_hash_naive() << endl;
    count_board[board.get_hash()]++;
	}
#ifdef MPI_ENABLE
  mpi_exit();
  }else{
	  for(int i=0;i<50;i++){
		  plss[i] = vector<PlayLog>(256);
	  }
    while(mpi_search(0,0,0));   
    cout<<"MPI Exit:" << mpi_rank<<endl;
  }
  MPI_Finalize(); 
#endif
	return 0;
}
