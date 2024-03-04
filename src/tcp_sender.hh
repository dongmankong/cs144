#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
//my
class Timer
{
  private:
    bool status;
    uint64_t initial_RTO;
    uint64_t cur_RTO;
    uint64_t curTime;
  public:
    bool isRunning(){return status;}
    void start(){status=true;curTime=0; }
    void close(){status=false;}
    void tick(uint64_t passedTime){if(status) curTime+=passedTime;}
    bool isOverTime(){return status==true && curTime>=cur_RTO;}
    void overTime(){cur_RTO*=2;}
    void reset(){cur_RTO=initial_RTO;}
    Timer(uint64_t initial_RTO_ms_):status(false)
    ,initial_RTO(initial_RTO_ms_)
    ,cur_RTO(initial_RTO_ms_)
    ,curTime(0)
    {}
    // {std::cout<<"-----------------------------"<<std::endl;}
};
//
class TCPSender
{
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;
//my
  Timer timer;
  std::queue<TCPSenderMessage> sentQueue;
  std::queue<TCPSenderMessage> tobeSentQueue;
  uint64_t ack_seq=0;
  uint16_t window_size=1;
  uint64_t retransmissionsCnt=0;
  uint64_t cur_seq=0;//发送未确定的队列的最后的一个值的下一个值，也就是说最期望返回确认的值
  uint64_t outstandingCnt=0;
  bool haveSYN=false;
  bool haveFin=false;
public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  /*使用给定的默认重传超时和可能的ISN构造TCP发送器*/
  TCPSender( uint64_t initial_RTO_ms, std::optional<Wrap32> fixed_isn );

  /* Push bytes from the outbound stream */
  /*从出站流推送字节*/
  void push( Reader& outbound_stream );

  /* Send a TCPSenderMessage if needed (or empty optional otherwise) */
  /*如果需要，发送TCPSenderMessage（否则为空可选）*/
  std::optional<TCPSenderMessage> maybe_send();

  /* Generate an empty TCPSenderMessage */
  /*生成空TCPSenderMessage*/
  TCPSenderMessage send_empty_message() const;

  /* Receive an act on a TCPReceiverMessage from the peer's receiver */
  /*从对等方的接收器接收TCPReceiverMessage上的动作*/
  void receive( const TCPReceiverMessage& msg );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called. */
  /*自上次调用tick（）方法以来，时间已经过去了给定的毫秒数。*/
  void tick( uint64_t ms_since_last_tick );

  /* Accessors for use in testing */
  /*有多少序列号是突出的*/
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  /*发生了多少次连续的重新传输？*/
  uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
  
//   void read( Reader& reader, uint64_t len, std::string& out )
// {
//   out.clear();

//   while ( reader.bytes_buffered() and out.size() < len ) {
//     auto view = reader.peek();

//     if ( view.empty() ) {
//       throw std::runtime_error( "Reader::peek() returned empty string_view" );
//     }

//     view = view.substr( 0, len - out.size() ); // Don't return more bytes than desired.
//     out += view;
//     reader.pop( view.size() );
//   }
// }
};
