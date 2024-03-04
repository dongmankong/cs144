#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

using namespace std;

// ethernet_address: Ethernet (what ARP calls "hardware") address of the interface
// ip_address: IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( const EthernetAddress& ethernet_address, const Address& ip_address )
  : ethernet_address_( ethernet_address ), 
  ip_address_( ip_address ),
  ip2mac(),
  ip2macTime(),
  arpSendTime(),
  nextIp_dgrams(),
  ethFrameQue(),
  ARPTTL(5000),
  IPTTL(30000)
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  // (void)dgram;
  // (void)next_hop;
//my
  uint32_t ipAddr=next_hop.ipv4_numeric();
  if(ip2mac.count(ipAddr)){
    // cout<<"ipddddddddddddddd   "<<ipAddr<<endl;
    EthernetFrame ethFrameAns;
    ethFrameAns.payload=serialize(dgram);
    ethFrameAns.header.type=EthernetHeader::TYPE_IPv4;
    ethFrameAns.header.dst=ip2mac[ipAddr];
    ethFrameAns.header.src=this->ethernet_address_;
    ethFrameQue.push(ethFrameAns);
    return ;
    // maybe_send();
  }
  if(!arpSendTime.count(ipAddr)){
    ARPMessage arpgram;
    arpgram.opcode=ARPMessage::OPCODE_REQUEST;
    arpgram.sender_ethernet_address=this->ethernet_address_;
    // arpgram.target_ethernet_address=ETHERNET_BROADCAST;
    arpgram.sender_ip_address=this->ip_address_.ipv4_numeric();
    arpgram.target_ip_address=ipAddr;
    EthernetFrame ethFrameAns;
    ethFrameAns.header.type=EthernetHeader::TYPE_ARP;
    ethFrameAns.header.dst=ETHERNET_BROADCAST;
    ethFrameAns.header.src=this->ethernet_address_;
    ethFrameAns.payload=serialize(arpgram);
    ethFrameQue.push(ethFrameAns);

    arpSendTime[ipAddr]=0;
    // cout<<"send time  "<<arpSendTime[ipAddr]<<endl;
    nextIp_dgrams[ipAddr].push_back(dgram);
    //发送
  }
}

// frame: the incoming Ethernet frame
//帧：传入的以太网帧
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  // (void)frame;
  // return {};
//my
  if(frame.header.dst!=ETHERNET_BROADCAST && frame.header.dst!=this->ethernet_address_){
    return nullopt;
  }
  // cout<<"aaaaaaaaaaaaaaaaaaaa"<<endl;
  InternetDatagram ipgramAns;
  if(frame.header.type==EthernetHeader::TYPE_IPv4){
    if(parse(ipgramAns,frame.payload))
    {
      return ipgramAns;
    }
    return nullopt;
  }
  ARPMessage arpgram;
  if(frame.header.type==EthernetHeader::TYPE_ARP && parse(arpgram,frame.payload)){
    uint32_t ipAddr=arpgram.sender_ip_address;
    EthernetAddress ethAddre=arpgram.sender_ethernet_address;
    ip2mac[ipAddr]=ethAddre;
    ip2macTime[ipAddr]=0;
    if(arpgram.opcode==ARPMessage::OPCODE_REQUEST && this->ip_address_.ipv4_numeric()==arpgram.target_ip_address){
      ARPMessage arpgramReply;
      arpgramReply.opcode=ARPMessage::OPCODE_REPLY;
      arpgramReply.sender_ethernet_address=this->ethernet_address_;
      arpgramReply.target_ethernet_address=ethAddre;
      arpgramReply.sender_ip_address=this->ip_address_.ipv4_numeric();
      arpgramReply.target_ip_address=ipAddr;
      EthernetFrame ethFrameAns;
      ethFrameAns.header.type=EthernetHeader::TYPE_ARP;
      ethFrameAns.header.dst=ethAddre;
      ethFrameAns.header.src=this->ethernet_address_;
      // ethFrameAns.header.dst=arpgramReply.target_ethernet_address;
      // ethFrameAns.header.src=arpgramReply.sender_ethernet_address;
      ethFrameAns.payload=serialize(arpgramReply);
      ethFrameQue.push(ethFrameAns);
    }else if(arpgram.opcode==ARPMessage::OPCODE_REPLY ){
      if(nextIp_dgrams.count(ipAddr)){
        for(auto &dgram:nextIp_dgrams[ipAddr]){
          // cout<<"sssssssssssssssssssssssssssssss"<<endl;
          // Address next_hop=Address::from_ipv4_numeric(ipAddr);
          send_datagram(dgram,Address::from_ipv4_numeric(ipAddr));
        }
        // cout<<"aaaaaaaaaaaaaaaaa"<<ipAddr<<endl;
        nextIp_dgrams.erase(ipAddr);
        // arpSendTime.erase(ipAddr);
      }
    }
  }

  return nullopt;
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  // (void)ms_since_last_tick;
//my
  for(auto it=ip2macTime.begin();it!=ip2macTime.end();){
    it->second+=ms_since_last_tick;
    if(it->second>=NetworkInterface::IPTTL){
      // cout<<"ip2macTime------------------------------"<<endl;
      ip2mac.erase(it->first);
      it=ip2macTime.erase(it);//返回删除的下一个元素的迭代器
    }else{
      ++it;
    }
  }
  for(auto it=arpSendTime.begin();it!=arpSendTime.end();){
    it->second+=ms_since_last_tick;
    // cout<<"arptime  "<<it->second<<"  ";
    if(it->second>=NetworkInterface::ARPTTL){
      // cout<<"arpSendTime------------------------------"<<endl;

      it=arpSendTime.erase(it);
    }
    else{
      ++it;
    }
  }
}

optional<EthernetFrame> NetworkInterface::maybe_send()
{
  // return {};
//my
  if(ethFrameQue.empty())return nullopt;
  EthernetFrame ans=ethFrameQue.front();
  ethFrameQue.pop();
  return ans;
}
