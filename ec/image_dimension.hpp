#pragma once

#include "global.hpp"

namespace ec
{
class ImageOffset
{
  size_t data_[3];

public:
  constexpr ImageOffset()
    : data_{ 0 , 0 , 0 }
  {
  }
  constexpr ImageOffset( size_t x )
    : data_{ x , 0 , 0 }
  {
  }
  constexpr ImageOffset( size_t x , size_t y )
    : data_{ x , y , 0 }
  {
  }
  constexpr ImageOffset( size_t x , size_t y , size_t z )
    : data_{ x , y , z }
  {
  }

  constexpr size_t* data()
  {
    return data_;
  }
  constexpr size_t const* data() const
  {
    return data_;
  }
};
class ImageSize
{
  size_t data_[3];

public:
  constexpr ImageSize()
    : data_{ 1 , 1 , 1 }
  {
  }
  constexpr ImageSize( size_t x )
    : data_{ x , 1 , 1 }
  {
  }
  constexpr ImageSize( size_t x , size_t y )
    : data_{ x , y , 1 }
  {
  }
  constexpr ImageSize( size_t x , size_t y , size_t z )
    : data_{ x , y , z }
  {
  }
  constexpr size_t* data()
  {
    return data_;
  }
  constexpr size_t const* data() const
  {
    return data_;
  }
};
class ImagePitch
{
  friend class CommandQueue;
  size_t data_[2];

public:
  constexpr ImagePitch()
    : data_{ 0 , 0 }
  {
  }
  /// 1d
  constexpr ImagePitch( size_t w )
    : data_{ 0 , 0 }
  {
  }
  /// 2d
  constexpr ImagePitch( size_t w , size_t h )
    : data_{ w , 0 }
  {
  }
  /// 3d
  constexpr ImagePitch( size_t w , size_t h , size_t d )
    : data_{ w , w*h }
  {
  }
  constexpr size_t& row()
  {
    return data_[0];
  }
  constexpr size_t row() const
  {
    return data_[0];
  }
  constexpr size_t& slice()
  {
    return data_[1];
  }
  constexpr size_t slice() const
  {
    return data_[1];
  }
};

}
