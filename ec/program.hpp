#pragma once

#include "cl.hpp"
#include "global.hpp"
#include "list_view.hpp"
#include <vector>
#include <utility>

namespace ec
{
class Program
{
  friend void swap( Program& , Program& );
protected:
  cl_program data_;
  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseProgram( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainProgram( get() );
    }
  }
public:
  Program()
    : data_( NULL )
  {
  }
  Program( cl_program data )
    : data_( data )
  {
    retain_if();
  }
  Program( cl_program data , no_retain_t )
    : data_( data )
  {
  }
  Program( cl_context context ,
      detail::list_view<const char*> const& sources , detail::list_view<size_t> const& sizes ,
      int* errp=nullptr )
  {
    int err;
    const cl_program ret = clCreateProgramWithSource( context ,
        sources.size() , sources.data() , sizes.data() ,
        &err );
    EC_CHECK_ERROR( err , errp , data_=NULL;return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }
  ~Program()
  {
    release_if();
  }
  Program( Program const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Program( Program&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  Program& operator=( Program const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Program& operator=( Program&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
  cl_program* ptr()
  {
    return &data_;
  }
  cl_program const* ptr() const
  {
    return &data_;
  }
  cl_program get() const
  {
    return data_;
  }
  operator cl_program() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  void build( detail::list_view<cl_device_id> const& devices ,
      const char* options ,
      void (*func)( cl_program , void* )=nullptr , void* userptr=nullptr ,
      int* errp=nullptr ) const
  {
    int err = clBuildProgram( get() ,
        devices.size() , devices.data() ,
        options , func , userptr );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }

  cl_build_status build_status( cl_device_id device , int* errp=nullptr ) const
  {
    cl_build_status ret;
    const int err = clGetProgramBuildInfo( get() , device ,
        CL_PROGRAM_BUILD_STATUS , sizeof(ret) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  std::string build_log( cl_device_id device , int* errp=nullptr ) const
  {
    size_t len;
    int err = clGetProgramBuildInfo( get() , device ,
        CL_PROGRAM_BUILD_LOG , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::string ret;
    ret.resize( len );
    err = clGetProgramBuildInfo( get() , device ,
        CL_PROGRAM_BUILD_LOG , len , &( *ret.begin() ) , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )

    return ret;
  }
  void compile( detail::list_view<cl_device_id> const& devices ,
      const char* options ,
      detail::list_view<cl_program> const& headers ,
      detail::list_view<const char*> const& header_names ,
      void (*func)( cl_program , void* ) ,
      void* userdata=nullptr ,
      int* errp=nullptr ) const
  {
    const int err = clCompileProgram( get() ,
      devices.size() , devices.data() ,
      options ,
      headers.size() , headers.data() ,
      header_names.data() ,
      func , userdata );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }

  Kernel kernel( const char* name , int* errp=nullptr ) const;

protected:
  template < typename T >
  T get_info_( cl_program_info info , int* errp ) const
  {
    T ret;
    const int err = clGetProgramInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  template < typename T >
  std::vector<T> get_info_raw_( cl_program_info info , int* errp ) const
  {
    size_t len;
    int err = clGetProgramInfo( get() , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::vector<T> buf( len/sizeof(T) );
    err = clGetProgramInfo( get() , info , len , buf.data() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return buf;
  }
  std::string get_info_string_( cl_program_info info , int* errp ) const
  {
    size_t len;
    int err = clGetProgramInfo( get() , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::string ret;
    ret.resize( len );
    err = clGetProgramInfo( get() , info , len , &( *ret.begin() ) , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_PROGRAM_REFERENCE_COUNT , errp );
  }
  Context context( int* errp=nullptr ) const;
  cl_uint num_devices( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_PROGRAM_NUM_DEVICES , errp );
  }
  Device device( int* errp=nullptr ) const;
  std::vector<Device> devices( int* errp=nullptr ) const;
  std::string source( int* errp=nullptr ) const
  {
    return get_info_string_( CL_PROGRAM_SOURCE , errp );
  }
  size_t num_kernels( int* errp=nullptr ) const
  {
    return get_info_<size_t>( CL_PROGRAM_NUM_KERNELS , errp );
  }
  std::string kernels( int* errp=nullptr ) const
  {
    return get_info_string_( CL_PROGRAM_KERNEL_NAMES , errp );
  }
};

inline void swap( Program& l , Program& r )
{
  std::swap( l.data_ , r.data_ );
}

}
