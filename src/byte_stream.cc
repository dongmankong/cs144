#include <stdexcept>

#include "byte_stream.hh"
//my
#include <iostream>
// 
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ),que() {}

void Writer::push( string data )
{
  // (void)data;
// my
  // cout<<" shuru: "<<endl;
  for(char c:data){
    if(available_capacity()>0){
      this->que.push(c);
      this->total_write++;
      // cout<<c<<"  ";
    }
    // cout<<endl;
    // this->total_write+=data.size();
    // else{
    //   break;
    // }
  }
  // int n=data.size();
  // for(int i=0;i<n;++i){
  //   if(available_capacity()==0){
  //     break;
  //   }else{
  //     if(data[i]==EOF){
  //       this->close();
  //     }
  //     this->que.push(data[i]);
  //     this->total_write++;
  //     this->cur_num++;
  //   }
  // }
}

void Writer::close()
{
  // my
  this->isClosed=true;
}

void Writer::set_error()
{
  // my
  // cout<<"write error"<<endl;
  this->isError=true;
}

bool Writer::is_closed() const
{
  // my
  // return {};
  return isClosed;
}

uint64_t Writer::available_capacity() const
{
  // my
  // return {};
  return capacity_-que.size();
}

uint64_t Writer::bytes_pushed() const
{
  // my
  // return {};
  return total_write;
}

string_view Reader::peek() const
{
  // return {};
  // my
  // string s="";
  // s+=this->que.front();
  // return s;
  if(que.empty()) return {};
  // return string(1,this->que.front());
  return string_view(&que.front(),1);
}

bool Reader::is_finished() const
{
  // return {};
  // my
  // cout<<"is_finished:"<<this->bytes_buffered()<<endl;
  if(this->bytes_buffered()==0 && this->isClosed==true){
    return true;
  }
  return false;
}

bool Reader::has_error() const
{
  // return {};
  //my
  return isError;
}

void Reader::pop( uint64_t len )
{
  // (void)len;
  // my
  // cout<<"len: "<<len<<endl;
  while(len>0){
    if(que.empty()) break;
    que.pop();
    this->total_read++;
    len--;
  }
  // cout<<"pop:"<<(int)len<<endl;

}

uint64_t Reader::bytes_buffered() const
{
  // return {};
  // my
  return que.size();
}

uint64_t Reader::bytes_popped() const
{
  // return {};
  // my
  return total_read;
}
