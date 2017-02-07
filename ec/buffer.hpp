#pragma once

#include "memory.hpp"
#include "buffer_create_type.hpp"
#include <utility>

namespace ec
{

class Buffer
  : public Memory
{
  friend void swap( Buffer& , Buffer& );
public:
  Buffer()
    : Memory()
  {
  }
  Buffer( cl_mem data )
    : Memory( data )
  {
  }
  Buffer( cl_mem data , no_retain_t )
    : Memory( data , no_retain_t() )
  {
  }
  Buffer( Buffer const& rhs )
    : Memory( static_cast< Memory const& >( rhs ) )
  {
  }
  Buffer( Buffer&& rhs )
    : Memory( static_cast< Memory&& >( rhs ) )
  {
  }
  Buffer( cl_context context ,
      cl_mem_flags flags ,
      size_t size ,
      void* host_ptr=nullptr ,
      int* errp=nullptr )
  {
    int err;
    const cl_mem ret = clCreateBuffer( context ,
        flags , size , host_ptr , 
        &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }
  Buffer& operator=( Buffer const& rhs )
  {
    Memory::operator=( static_cast< Memory const& >( rhs ) );
    return *this;
  }
  Buffer& operator=( Buffer&& rhs )
  {
    Memory::operator=( static_cast< Memory&& >( rhs ) );
    return *this;
  }

  Buffer sub_buffer( cl_mem_flags flags , buffer_create_range const& data ,
      int* errp=nullptr ) const;

  Buffer parent( int* errp=nullptr ) const;
  size_t offset( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_MEM_OFFSET , errp );
  }
};
inline void swap( Buffer& l , Buffer& r )
{
  std::swap( l.data_ , r.data_ );
}

inline Buffer Buffer::sub_buffer( cl_mem_flags flags , buffer_create_range const& data , int* errp ) const
{
  int err;
  const cl_mem ret = clCreateSubBuffer( get() , 
      flags , buffer_create_range::value , data.data() , &err );
  EC_CHECK_ERROR( err , errp , return {} )
  EC_SET_ERRP( errp )
  return { ret , no_retain_t() };
}

}
