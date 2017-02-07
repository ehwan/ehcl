#pragma once

#include "cl.hpp"
#include "global.hpp"
#include "list_view.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace ec
{

class Context
{
  friend void swap( Context& , Context& );
  cl_context data_;

  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseContext( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainContext( get() );
    }
  }

public:
  Context()
    : data_( NULL )
  {
  }
  Context( cl_context data )
    : data_( data )
  {
    retain_if();
  }
  Context( cl_context data , no_retain_t )
    : data_( data )
  {
  }
  Context( const cl_context_properties* properties ,
      detail::list_view<cl_device_id> const& devices ,
      void (*func)( const char* errinfo , const void* privateinfo , size_t cb , void* userdata_ ) ,
      void* userdata ,
      int* errp=nullptr )
  {
    int err;
    const cl_context ret = clCreateContext( properties ,
        devices.size() , devices.data() ,
        func , userdata , &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )

    data_ = ret;
  }
      
  ~Context()
  {
    release_if();
  }
  Context( Context const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Context( Context&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }

  Context& operator=( Context const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Context& operator=( Context&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
  
  cl_context get() const
  {
    return data_;
  }
  operator cl_context() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  Program link( detail::list_view<cl_device_id> const& devices ,
      const char* options ,
      detail::list_view<cl_program> const& programs ,
      void (*func)( cl_program , void* ) , void* userdata=nullptr ,
      int* errp=nullptr ) const;

protected:
  template < typename T >
  T get_info_( cl_context_info info , int* errp ) const
  {
    T ret;
    const int err = clGetContextInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  template < typename T >
  std::vector<T> get_info_raw_( cl_context_info info , int* errp ) const
  {
    size_t len;
    int err = clGetContextInfo( get() , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::vector<T> buf( len/sizeof(T) );
    err = clGetContextInfo( get() , info , len , buf.data() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return buf;
  }

public:
  cl_uint reference_count( int *errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_CONTEXT_REFERENCE_COUNT , errp );
  }
  cl_uint num_devices( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_CONTEXT_NUM_DEVICES , errp );
  }
  std::vector<Device> devices( int* errp=nullptr ) const;
  Device device( int* errp=nullptr ) const;
  std::vector<cl_context_properties> properties( int* errp=nullptr ) const
  {
    return get_info_raw_< cl_context_properties >( CL_CONTEXT_PROPERTIES , errp );
  }
};

inline void swap( Context& l , Context& r )
{
  std::swap( l.data_ , r.data_ );
}

}
