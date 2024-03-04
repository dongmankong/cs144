#pragma once

#include <cstdint>
#include<iostream>
/*
 * The Wrap32 type represents a 32-bit unsigned integer that:
 *    - starts at an arbitrary "zero point" (initial value), and
 *    - wraps back to zero when it reaches 2^32 - 1.
 */
// *Wrap32类型表示一个32位无符号整数，该整数：

// *-从任意“零点”（初始值）开始，并且

// *-当它达到2^32-1时回零。

class Wrap32
{
protected:
  uint32_t raw_value_ {};

public:
  explicit Wrap32( uint32_t raw_value ) : raw_value_( raw_value ) {}

  /* Construct a Wrap32 given an absolute sequence number n and the zero point. */
  // 在给定绝对序列号n和零点的情况下，构造一个Wrap32
  static Wrap32 wrap( uint64_t n, Wrap32 zero_point );

  /*
   * The unwrap method returns an absolute sequence number that wraps to this Wrap32, given the zero point
   * and a "checkpoint": another absolute sequence number near the desired answer.
   *
   * There are many possible absolute sequence numbers that all wrap to the same Wrap32.
   * The unwrap method should return the one that is closest to the checkpoint.
   */
  // *unwrap方法返回一个绝对序列号，该序列号在给定零点的情况下包装到此Wrap32

  // *和一个“检查点”：接近所需答案的另一个绝对序列号。

  // *

  // *有许多可能的绝对序列号都被包裹到同一个Wrap32。

  // *unwrap方法应该返回最接近检查点的方法。
  uint64_t unwrap( Wrap32 zero_point, uint64_t checkpoint ) const;

  Wrap32 operator+( uint32_t n ) const { return Wrap32 { raw_value_ + n }; }
  bool operator==( const Wrap32& other ) const { return raw_value_ == other.raw_value_; }
};
