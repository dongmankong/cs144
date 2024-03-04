#include "reassembler.hh"

using namespace std;
// my
Reassembler::Reassembler():buf(),have(){}
// 流中满了 不能再继续添加； 
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
//my
  if(this->curIndex==(uint64_t)-1){
    this->curIndex=0;
    // cout<<this->curIndex<<endl;
    this->capacity_=output.available_capacity();
    this->buf=vector<char>(capacity_,0);
    this->have=vector<bool>(capacity_,false);
  }
  // cout<<"index:"<<first_index<<"    "<<data<<" shi:" << is_last_substring<<endl;
  int n=data.size();
  // if(curIndex+capacity_<first_index){//超过容量上限
  //   return ;
  // }
  if(is_last_substring){
    isLastIndex=first_index+n;
  }
  if(first_index+n<=curIndex){
    if(curIndex==isLastIndex){
      output.close();
    }
    return ;
  }
  if(first_index<curIndex){
    // cout<<first_index<<"?????????????????????????"<<endl;
    data=data.substr(curIndex-first_index);
    first_index=curIndex;
    n=data.size();
  }
  if(output.available_capacity()<data.size()){
    data=data.substr(0,output.available_capacity());
    n=data.size();
  }
  // int startIndex=0;
  // if(first_index<curIndex){
  //   startIndex=curIndex-first_index;
  //   first_index=curIndex;
  //   n-=curIndex-first_index;
  // }
  // data=data.substr(startIndex,min(output.available_capacity(),(uint64_t)n));
  // n=data.size();

  for(int i=0;i<n;++i){
    if(curIndex+capacity_<=first_index+i){//超过容量上限
      break;
    }
    int nowIndex=(first_index+i)%capacity_;
    if(have[nowIndex]==false){
      have[nowIndex]=true;
      buf[nowIndex]=data[i];
      bufNum++;
      // cout<<"nowIndex:" <<nowIndex<<" data[i]: "<<data[i]<<endl;
    }
  }
  string pushd="";
  while(have[curIndex%capacity_]==true){
    if(curIndex==isLastIndex){
      break;
    }
    have[curIndex%capacity_]=false;
    pushd+=buf[curIndex%capacity_];
    curIndex++;
  }
  // cout<<"curIndex: "<<curIndex<<endl;
  if(pushd!=""){
    // cout<<"输入的值为："<<pushd<<endl;
    bufNum-=pushd.size();
    output.push(pushd);
  }
  if(curIndex==isLastIndex){
    output.close();
  }
  // cout<<"-----------------------------------------------------------------------"<<endl;

  // Your code here.
  // (void)first_index;
  // (void)data;
  // (void)is_last_substring;
  // (void)output;
}

uint64_t Reassembler::bytes_pending() const
{
//my
  // uint64_t sum=0;
  // for(uint64_t i=0;i<capacity_;++i){
  //   if(have[i]==true){
  //     sum++;
  //   }
  // }
  return this->bufNum;
  // for(auto x:m){
  //   sum+=x.second.size();
  // }
  // return m.size();
//
  // return {};
}
