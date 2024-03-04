#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  // Your code here.
  // (void)message;
  // (void)reassembler;
  // (void)inbound_stream;
  //my
  // cout<<"-----------------------------"<<endl;
  if(message.SYN){
    this->haveSYN=true;
    zero_point=message.seqno;
  }
  if(this->haveSYN==false){
    return ;
  }

  uint64_t absolute_seqno=message.seqno.unwrap(this->zero_point,this->cur_seqno);
  // if(absolute_seqno+message.payload.length()+message.SYN>cur_seqno){
  //   cur_seqno=absolute_seqno+message.payload.length()+message.SYN;
  // }
  // cout<<cur_seqno<<endl;
  // string data=message.payload.release();
  // cout<<"absolute_seqno: "<<absolute_seqno<<endl;
  // cout<<"string: "<<data<<endl;
  if(message.FIN){
    // data=data.substr(0,data.size()-1);
    if(message.SYN){
      reassembler.insert(0,message.payload,true,inbound_stream);
    }else{
      reassembler.insert(absolute_seqno-1,message.payload,true,inbound_stream);
    }
  }else{
    if(message.SYN){
      reassembler.insert(0,message.payload,false,inbound_stream);
    }else{
      reassembler.insert(absolute_seqno-1,message.payload,false,inbound_stream);
    }
  }


}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  // Your code here.
  // (void)inbound_stream;
  // return {};
  //my
  TCPReceiverMessage ans{}; 
  // if(this->haveSYN==false){
  //   ans.window_size=inbound_stream.available_capacity()>UINT16_MAX?UINT16_MAX:inbound_stream.available_capacity();
  //   return ans;
  // }
  ans.window_size=inbound_stream.available_capacity()>UINT16_MAX?UINT16_MAX:inbound_stream.available_capacity();
  if(this->haveSYN==false){
    return ans;
  }
  //序列号为已经写入inbound流的字节数+1，因为FIN特殊情况，特殊考虑
  ans.ackno=Wrap32::wrap(inbound_stream.bytes_pushed()+1+inbound_stream.is_closed(),zero_point);
  return ans;
}
