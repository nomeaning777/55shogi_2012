/**
 * readlineライブラリのC++用ラッパー
 *
 * 利用前にinit関数を，利用後にfinalize関数呼び出すことで履歴が保存される．
 * @author nomeaning
 * @version 1.0
 */
#include <string>
using namespace std;
namespace ReadLine{
  /**
   * 履歴記録ファイルのパスを表す
   */
  extern string history_file;

  /**
   * コンソールから一行読み出す
   *
   * コンソールから一行読み出しその結果を返す．
   * @param[in] prompt 表示されるプロンプト
   * @return ユーザーの入力した文字列
   * @attention EOFの場合も空文字が返される
   */
  string readline(std::string prompt = "> "); // 一行読み出す
  /**
   * ReadLineライブラリの初期化を行う
   *
   * ReadLineライブラリの初期化を行う
   * @param[in] history_file ヒストリーファイルのパス
   * @param[in] home ファイルのパスをホームフォルダからの相対パスにするか
   */
  void init(std::string history_file, bool home);
  
  /**
   * Readlineライブラリの終了処理を行う．
   *
   * readlineライブラリの終了処理を行う．ヒストリーファイルはこのタイミングで書き出される．
   */
  void finalize();
  /**
   * EOFに達したかどうかを返す
   *
   * 入力がEOFに達したかを返す．
   * @return EOFならtrue．そうではないならfalse
   */
  bool is_eof();

  /**
   * EOFに達したかどうか
   */
  static bool eof;
};
