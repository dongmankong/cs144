#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

class TCPReceiver
{
  //my
private:
  bool haveSYN=false;
  Wrap32 zero_point{0};
  uint64_t cur_seqno=0;
public:
  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  //*TCPReceiver接收TCPSender消息，将其有效载荷插入重新组装器在正确的流索引处。 inbound_stream为输入的流
  void receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream );

  /* The TCPReceiver sends TCPReceiverMessages back to the TCPSender. */
  //TCPReceiver将TCPReceiverMessages发送回TCPSender。
  TCPReceiverMessage send( const Writer& inbound_stream ) const;
};
