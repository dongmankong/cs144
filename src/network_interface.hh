#pragma once

#include "address.hh"
#include "ethernet_frame.hh"
#include "ipv4_datagram.hh"

#include <iostream>
#include <list>
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>

// A "network interface" that connects IP (the internet layer, or network layer)
// with Ethernet (the network access layer, or link layer).

// This module is the lowest layer of a TCP/IP stack
// (connecting IP with the lower-layer network protocol,
// e.g. Ethernet). But the same module is also used repeatedly
// as part of a router: a router generally has many network
// interfaces, and the router's job is to route Internet datagrams
// between the different interfaces.

// The network interface translates datagrams (coming from the
// "customer," e.g. a TCP/IP stack or router) into Ethernet
// frames. To fill in the Ethernet destination address, it looks up
// the Ethernet address of the next IP hop of each datagram, making
// requests with the [Address Resolution Protocol](\ref rfc::rfc826).
// In the opposite direction, the network interface accepts Ethernet
// frames, checks if they are intended for it, and if so, processes
// the the payload depending on its type. If it's an IPv4 datagram,
// the network interface passes it up the stack. If it's an ARP
// request or reply, the network interface processes the frame
// and learns or replies as necessary.
class NetworkInterface
{
private:
  // Ethernet (known as hardware, network-access, or link-layer) address of the interface
  //接口的以太网（称为硬件、网络访问或链路层）地址
  EthernetAddress ethernet_address_;

  // IP (known as Internet-layer or network-layer) address of the interface
  //接口的IP（称为互联网层或网络层）地址
  Address ip_address_;
//my
  std::unordered_map<uint32_t,EthernetAddress> ip2mac;//ip->mac转换
  std::unordered_map<uint32_t,size_t> ip2macTime;//ip->mac转换剩余时间
  std::unordered_map<uint32_t,size_t>arpSendTime;//广播ip数据报的mac帧之后的时间
  std::unordered_map<uint32_t,std::vector<InternetDatagram>> nextIp_dgrams;//下一跳的ip地址对应的ip数据报
  std::queue<EthernetFrame> ethFrameQue;//要发送的mac帧
  const size_t ARPTTL;
  const size_t IPTTL;

//
public:
  // Construct a network interface with given Ethernet (network-access-layer) and IP (internet-layer)
  // addresses
  //构建具有给定以太网（网络接入层）和IP（互联网层）的网络接口
  NetworkInterface( const EthernetAddress& ethernet_address, const Address& ip_address );

  // Access queue of Ethernet frames awaiting transmission
  //等待传输的以太网帧的访问队列
  std::optional<EthernetFrame> maybe_send();

  // Sends an IPv4 datagram, encapsulated in an Ethernet frame (if it knows the Ethernet destination
  // address). Will need to use [ARP](\ref rfc::rfc826) to look up the Ethernet destination address
  // for the next hop.
  // ("Sending" is accomplished by making sure maybe_send() will release the frame when next called,
  // but please consider the frame sent as soon as it is generated.)
  //   发送封装在以太网帧中的IPv4数据报（如果它知道以太网目的地
  //   地址）。需要使用[ARP]（\ref rfc:：rfc826）查找以太网目标地址下一跳。
  // （“发送”是通过确保maybe_send（）将在下次调用时释放帧来完成的，
  // 但请考虑在生成帧后立即发送的帧。）
  void send_datagram( const InternetDatagram& dgram, const Address& next_hop );

  //接收以太网帧并做出适当响应。
  //如果类型为IPv4，则返回数据报。
  //如果类型是ARP请求，则从“发送方”字段中学习映射，并发送ARP回复。  
  //如果类型是ARP回复，请从“发件人”字段中学习映射。
  std::optional<InternetDatagram> recv_frame( const EthernetFrame& frame );

  // Called periodically when time elapses
  void tick( size_t ms_since_last_tick );
};
