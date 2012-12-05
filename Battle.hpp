#ifndef Battle_HPP_
#define Battle_HPP_
#include <string>
#include "IProtocol.hpp"
#include "IGame.hpp"
// 先手の場合はクライアントを、後手の場合はサーバー動作を行うこと。
namespace Shogi55{
  // クライアントとしてゲームを開始する
  void client(IProtocol &protoco, IGame &game);

  // サーバーとしてゲームを開始する
  void server(int port, string name, IProtocol *protocol, IGame *game);
};
#endif
