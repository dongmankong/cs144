#include "router.hh"

#include <iostream>
#include <limits>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
//route_prefix：与数据报的目标地址匹配的“最多32位”IPv4地址前缀
//prefix_length：对于适用的路由
//route_prefix将需要与数据报的目的地址的相应位相匹配？
//next_hop：下一个跃点的IP地址。如果网络直接连接到路由器（在
//在这种情况下，下一跳地址应该是数据报的最终目的地）。
//interface_num：发送数据报的接口的索引。 
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  // (void)route_prefix;
  // (void)prefix_length;
  // (void)next_hop;
  // (void)interface_num;
//my
  this->routerTables.emplace_back(route_prefix,prefix_length,next_hop,interface_num);
}

void Router::route() 
{
  // cout<<"start-------------------------------"<<endl;
  for(AsyncNetworkInterface &interface_:interfaces_){
    // cout<<"interface_------------"<<endl;
    while(true){
      auto judge=interface_.maybe_receive();
      if(judge.has_value()==false) break;
      InternetDatagram ipgram=judge.value();
    // cout<<"ipgram------------"<<endl;
      // ipgram=interface_.maybe_receive().value();
      // cout<<"dst-------------------:   "<<ipgram.header.dst<<endl;
      if(ipgram.header.ttl>0) ipgram.header.ttl-=1;
      if(ipgram.header.ttl==0) continue;
      ipgram.header.compute_checksum();//检查校验和
      // cout<<"------------router-------------"<<endl;
      uint32_t ipAddr32=ipgram.header.dst;
      // size_t interfaceIndex=-1;
      size_t len=0;
      size_t routerTableIndex=-1;
      size_t routerDefault=-1;
      for(size_t i=0;i<routerTables.size();++i){
        // cout<<"iiii "<<i<<endl;
        // cout<<"routerTables[i].prefix_length  "<<(uint32_t)routerTables[i].prefix_length<<"       dddddd"<<endl;
        if(routerTables[i].prefix_length==0){
          routerDefault=i;
          // cout<<"routerTableIndex:  "<<routerTableIndex<<endl;
          continue;
        }
        uint32_t mask=-1;
        mask<<=((uint8_t)32-routerTables[i].prefix_length);
        // cout<<"mask   "<<mask<<endl;
        // cout<<routerTables[i].route_prefix<<endl;
        if((ipAddr32&mask)==routerTables[i].route_prefix && 
        routerTables[i].route_prefix>=len){
          // interfaceIndex=routerTables[i].interface_num;
          len=routerTables[i].route_prefix;
          routerTableIndex=i;
        }
      }
      // cout<<"routerTableIndex  "<<routerTableIndex<<endl;
      // cout<<"routerDefault   "<<routerDefault<<endl;
      // cout<<"dddd      "<<size_t(-1)<<endl;
      if(routerTableIndex==(size_t)-1){
        if(routerDefault==(size_t)-1){
          return ;//丢弃
        }
        routerTableIndex=routerDefault;
      }
      // cout<<"res----  "<<routerTableIndex<<endl;
      Router::interface(routerTables[routerTableIndex].interface_num).send_datagram(ipgram,routerTables[routerTableIndex].
      next_hop.value_or(Address::from_ipv4_numeric(ipAddr32)));
      // if(routerTables[routerTableIndex].next_hop.has_value()){
      //   Router::interface(interfaceIndex).send_datagram(ipgram,routerTables[routerTableIndex].next_hop.value());
      // }else{
      //   return ;
      // }
    }
  }

}
