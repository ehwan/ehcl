#pragma once

#include "cl.hpp"

namespace ec
{

class buffer_create_range
{
  struct data_t
  {
    size_t offset;
    size_t size;
  } data_;

public:
  constexpr static cl_buffer_create_type value = CL_BUFFER_CREATE_TYPE_REGION;
  buffer_create_range( size_t offset , size_t size )
  {
    data_.offset = offset;
    data_.size = size;
  }
  void const* data() const
  {
    return &data_;
  }
};

}
