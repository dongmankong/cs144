#include "wrapping_integers.hh"

using namespace std;
Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // // Your code here.
  // (void)n;
  // (void)zero_point;
  // return Wrap32 { 0 };
//my
  uint64_t MOD = (uint64_t)1 << 32;
  n%=MOD;
  return zero_point+(uint32_t)n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  // (void)zero_point;
  // (void)checkpoint;
  // return {};
//my
  uint32_t resLowbit=this->raw_value_-zero_point.raw_value_;
  uint64_t res1=(uint64_t)resLowbit;
  uint64_t checkLowbit=checkpoint&(0x00000000ffffffff);
  uint64_t checkHighbit=checkpoint&(0xffffffff00000000);
  // cout<<"first: "<<res1<<endl;
  if(checkLowbit>=res1){
    res1+=checkHighbit;
  }else{
    if(checkHighbit==0){
      return res1+checkHighbit;
    }
    res1+=checkHighbit-((uint64_t)1 << 32);
  }
  uint64_t res2=res1+((uint64_t)1 << 32);
  // if(checkpoint-res1<res2-checkpoint){
  //   return checkpoint-res1;
  // }else{
  //   return res2-checkpoint;
  // cout<<"first: "<<res1<<"  "<<res2<<endl;

  return checkpoint-res1<res2-checkpoint?res1:res2;
}
