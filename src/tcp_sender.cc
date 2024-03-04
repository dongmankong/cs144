#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <random>
//my
// #include<byte_stream_helpers.cc>
using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
// TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
//   : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms ),timer(initial_RTO_ms)
// {}
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), 
  initial_RTO_ms_( initial_RTO_ms ),
  timer(initial_RTO_ms),
  sentQueue(),
  tobeSentQueue()
{}
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  // return {};
//my
  return outstandingCnt;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  // return {};
//my
  return retransmissionsCnt;
}

optional<TCPSenderMessage> TCPSender::maybe_send()
{
  // Your code here.
  // return {};
//my
  // cout<<"send"<<endl;
  if(tobeSentQueue.empty()) return {};
  TCPSenderMessage msg=tobeSentQueue.front();
  // sentQueue.push(msg);
  tobeSentQueue.pop();
  if(timer.isRunning()==false){
    timer.start();
  }
  return msg;
}

void TCPSender::push( Reader& outbound_stream )
{
  // Your code here.
  // (void)outbound_stream;
//my
  // const uint64_t MAX_PAYLOAD_SIZE=1452;
  // cout<<"push"<<endl;
  size_t cur_window_size=window_size==0?1:window_size;
  while(outstandingCnt<cur_window_size){
    TCPSenderMessage msg;
    msg.seqno=Wrap32::wrap(cur_seq,isn_);
    if(haveSYN==false){
      outstandingCnt++;
      haveSYN=true;
      msg.SYN=true;
    }
    uint64_t payload_size=min(cur_window_size-outstandingCnt,TCPConfig::MAX_PAYLOAD_SIZE);//发送窗口包括SYN和FIN
    payload_size=min(payload_size,outbound_stream.bytes_buffered());//判断流中剩余的大小
    read(outbound_stream,payload_size,msg.payload);
    outstandingCnt+=payload_size;
    if(haveFin==false && outbound_stream.is_finished() && cur_window_size>outstandingCnt){
      haveFin=true;
      outstandingCnt++;
      msg.FIN=true;
    }
    if(msg.sequence_length()==0) break;//如果读的东西为空
    tobeSentQueue.push(msg);
    sentQueue.push(msg);
    cur_seq+=msg.sequence_length();
    if(haveFin==true) break;
  }

}

TCPSenderMessage TCPSender::send_empty_message() const
{
  // Your code here.
  // return {};
//my
  // cout<<"pushEmpty"<<endl;
  TCPSenderMessage msg;
  msg.seqno=Wrap32::wrap(cur_seq,isn_);
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  // (void)msg;
//my
  // cout<<"receive"<<endl;
  this->window_size=msg.window_size;
  if(msg.ackno.has_value()){
      ack_seq= msg.ackno.value().unwrap(isn_,cur_seq);
      if(ack_seq>cur_seq) return ;
      // uint64_t rightWindow=ack_seq+window_size;//[)
      while(!sentQueue.empty()){
        uint64_t tmp=sentQueue.front().seqno.unwrap(isn_,cur_seq);
        tmp+=sentQueue.front().sequence_length();//最后一个位置的下一个位置
        if(tmp<=ack_seq){
          outstandingCnt-=sentQueue.front().sequence_length();
          sentQueue.pop();
          timer.reset();
          retransmissionsCnt=0;
          if(!sentQueue.empty()){
            timer.start();
          }
        }else{
          break;
        }
      }
      if(sentQueue.empty()) timer.close();
  }
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  // (void)ms_since_last_tick;
//my
  // cout<<"tick"<<endl;
  if(!timer.isRunning()) return ;
  timer.tick(ms_since_last_tick);
  if(timer.isOverTime()){
    tobeSentQueue.push(sentQueue.front());
    // maybe_send();
    // timer.overTime();
    if(window_size!=0){
      retransmissionsCnt++;
      timer.overTime();
    }
    timer.start();
  }
}
