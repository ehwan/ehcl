#pragma once

#include "cl.hpp"
#include "global.hpp"
#include "kernel_argument.hpp"
#include <string>
#include <memory>
#include <array>
#include <utility>

namespace ec
{
class Kernel
{
  friend void swap( Kernel& , Kernel& );
protected:
  cl_kernel data_;
  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseKernel( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainKernel( get() );
    }
  }
public:
  Kernel()
    : data_( NULL )
  {
  }
  Kernel( cl_kernel data )
    : data_( data )
  {
    retain_if();
  }
  Kernel( cl_kernel data , no_retain_t )
    : data_( data )
  {
  }
  ~Kernel()
  {
    release_if();
  }
  Kernel( Kernel const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Kernel( Kernel&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  Kernel& operator=( Kernel const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Kernel& operator=( Kernel&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
  cl_kernel get() const
  {
    return data_;
  }
  operator cl_kernel() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  detail::KernelArgument operator[]( cl_uint arg ) const
  {
    return { get() , arg };
  }

protected:
  template < typename T >
  T get_info_( cl_kernel_info info , int* errp=nullptr ) const
  {
    T ret;
    const int err = clGetKernelInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  std::string get_info_string_( cl_kernel_info info , int* errp=nullptr ) const
  {
    size_t len;
    int err;
    err = clGetKernelInfo( get() , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::unique_ptr< char[] > buf( new char[len] );
    err = clGetKernelInfo( get() , info , len , buf.get() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )

    return { buf.get() , buf.get() + len };
  }
  template < typename T >
  T get_workgroup_info_( cl_kernel_work_group_info info , cl_device_id device , int* errp ) const
  {
    T ret;
    const int err = clGetKernelWorkGroupInfo( get() , device ,
        info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  std::string name( int* errp=nullptr ) const
  {
    return get_info_string_( CL_KERNEL_FUNCTION_NAME , errp );
  }
  cl_uint num_args( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_KERNEL_NUM_ARGS , errp );
  }
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_KERNEL_REFERENCE_COUNT , errp );
  }
  Context context( int* errp=nullptr ) const;
  Program program( int* errp=nullptr ) const;
  std::string attributes( int* errp=nullptr ) const
  {
    return get_info_string_( CL_KERNEL_ATTRIBUTES , errp );
  }
  cl_ulong private_mem_size( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< cl_ulong >( CL_KERNEL_PRIVATE_MEM_SIZE , device , errp );
  }
  cl_ulong local_mem_size( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< cl_ulong >( CL_KERNEL_LOCAL_MEM_SIZE , device , errp );
  }
  size_t preferred_work_group_size_multiple( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< size_t >( CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE , device , errp );
  }
  std::array< size_t , 3 > global_work_size( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< std::array<size_t,3> >( CL_KERNEL_GLOBAL_WORK_SIZE , device , errp );
  }
  std::array< size_t , 3 > compile_workgroup_size( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< std::array<size_t,3> >( CL_KERNEL_COMPILE_WORK_GROUP_SIZE , device , errp );
  }

  size_t max_workgroup_size( cl_device_id device , int* errp=nullptr ) const
  {
    return get_workgroup_info_< size_t >( CL_KERNEL_WORK_GROUP_SIZE , device , errp );
  }
};

inline void swap( Kernel& l , Kernel& r )
{
  std::swap( l.data_ , r.data_ );
}

}
