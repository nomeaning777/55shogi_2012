#ifndef PLAYLOG_HPP_
#define PLAYLOG_HPP_

#include <iostream>
#include "55shogi.hpp"

// 差し手
class PlayLog{
private:
	uint32 state;
public:
	enum{
		PUT_PIECE=31,
		NOT_GET=31
	};
	/*
	 * turn : 先手(0) 後手(1)
	 * type : 移動対象の駒
	 * back_pos: 前の位置(PUT_PIECEの時駒を置く)
	 * forward_pos : 移動先の位置
	 * promote : 成るかどうか
	 * get_type : 取った駒
	 */
	PlayLog();
	// PlayLog(int turn, int type, int back_pos, int forward_pos, bool promote, int take_type = NOT_GET);
	
	PlayLog(int turn, int type, int back_pos, int forward_pos, bool promote, int take_type = NOT_GET){
	// type;何も取っていない場合は15(0b1111)
		state = turn | (type << 1) | (back_pos << 5) |  (forward_pos << 10) | (promote << 15) | (take_type << 16);
	}
	
  PlayLog(uint32 state): state(state){  }
	
  inline const bool is_take() const{
		return ((state >> 16) & (0b11111)) == NOT_GET;
	}

	inline const int get_take_type() const{
		return ((state >> 16) & (0b11111));
	}
	inline const uint32 operator()(){
		return state;
	}

	inline const bool put_piece() const{
		return ((state >> 5) & (0b11111)) == PUT_PIECE;
	}

	inline const int get_type() const{
		return (state >> 1) & (0b1111);
	}

	inline const int get_forward_pos() const{
		return (state >> 10) & (0b11111);
	}

	inline const int get_back_pos() const{
		return (state >> 5) & (0b11111);
	}

	inline const uint64 get_turn() const{
		return state & 1;
	}
	inline const bool get_promote() const{
		return (state >> 15) & 1;
	}

  void debug(){
    
  }

};

#endif
