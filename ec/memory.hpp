#pragma once

#include "cl.hpp"
#include "global.hpp"
#include <utility>

namespace ec
{

class Memory
{
  friend void swap( Memory& , Memory& );
protected:
  cl_mem data_;
  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseMemObject( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainMemObject( get() );
    }
  }
public:
  Memory()
    : data_( NULL )
  {
  }
  Memory( cl_mem data )
    : data_( data )
  {
    retain_if();
  }
  Memory( cl_mem data , no_retain_t )
    : data_( data )
  {
  }
  ~Memory()
  {
    release_if();
  }
  Memory( Memory const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Memory( Memory&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  Memory& operator=( Memory const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Memory& operator=( Memory&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
  cl_mem get() const
  {
    return data_;
  }
  operator cl_mem() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }
  void setDestructor( void (*func)( cl_mem , void* ) , void* userdata=nullptr , int* errp=nullptr ) const
  {
    const int err = clSetMemObjectDestructorCallback( get() , func , userdata );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }

protected:
  template < typename T >
  T get_info_( cl_mem_info info , int* errp ) const
  {
    T ret;
    const int err = clGetMemObjectInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  cl_mem_object_type type( int* errp=nullptr ) const
  {
    return get_info_< cl_mem_object_type >( CL_MEM_TYPE , errp );
  }
  cl_mem_flags flags( int* errp=nullptr ) const
  {
    return get_info_< cl_mem_flags >( CL_MEM_FLAGS , errp );
  }
  size_t size( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_MEM_SIZE , errp );
  }
  void* host_ptr( int* errp=nullptr ) const
  {
    return get_info_< void* >( CL_MEM_HOST_PTR , errp );
  }
  cl_uint map_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_MEM_MAP_COUNT , errp );
  }
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_MEM_REFERENCE_COUNT , errp );
  }
  Context context( int* errp=nullptr ) const;
};
inline void swap( Memory& l , Memory& r )
{
  std::swap( l.data_ , r.data_ );
}

}
