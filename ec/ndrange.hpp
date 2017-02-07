#pragma once

namespace ec
{
class NDRange
{
  cl_uint dim_;
  size_t data_[3];

public:
  constexpr NDRange()
    : dim_( 0 ) ,
      data_{ 0 , 0 , 0 }
  {
  }
  constexpr NDRange( size_t x )
    : dim_( 1 ) ,
      data_{ x , 0 , 0 }
  {
    data_[0] = x;
  }
  constexpr NDRange( size_t x , size_t y )
    : dim_( 2 ) ,
      data_{ x , y , 0 }
  {
  }
  constexpr NDRange( size_t x , size_t y , size_t z )
    : dim_( 3 ) ,
      data_{ x , y , z }
  {
  }

  constexpr cl_uint& dim()
  {
    return dim_;
  }
  constexpr cl_uint dim() const
  {
    return dim_;
  }
  constexpr size_t* data()
  {
    return dim_==0 ? nullptr : data_;
  }
  constexpr size_t const* data() const
  {
    return dim_ == 0 ? nullptr : data_;
  }
};
}
