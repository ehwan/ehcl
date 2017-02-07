#pragma once

#include "cl.hpp"
#include "global.hpp"
#include <string>
#include <memory>

namespace ec { namespace detail
{
class KernelArgument
{
  cl_kernel kernel_;
  cl_uint arg_;

public:
  KernelArgument( cl_kernel kernel , cl_uint arg )
    : kernel_( kernel ) ,
      arg_( arg )
  {
  }
  cl_uint argument() const
  {
    return arg_;
  }
  void set( size_t size , void const* ptr , int* errp=nullptr ) const
  {
    const int err = clSetKernelArg( kernel_ , arg_ , size , ptr );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }
  template < typename T >
  void operator=( T const& rhs ) const
  {
    set( sizeof(T) , &rhs );
  }

protected:
  template < typename T >
  T get_info_( cl_kernel_arg_info info , int* errp ) const
  {
    T ret;
    const int err = clGetKernelArgInfo( kernel_ , arg_ , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  std::string get_info_string_( cl_kernel_arg_info info , int* errp ) const
  {
    size_t len;
    int err = clGetKernelArgInfo( kernel_ , arg_ , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::unique_ptr< char[] > buf( new char[len] );
    err = clGetKernelArgInfo( kernel_ , arg_ , info , len , buf.get() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return { buf.get() , buf.get() + len };
  }

public:
  cl_kernel_arg_address_qualifier 
  address_qualifier( int* errp=nullptr ) const
  {
    return get_info_< cl_kernel_arg_address_qualifier >( CL_KERNEL_ARG_ADDRESS_QUALIFIER , errp );
  }
  cl_kernel_arg_access_qualifier 
  access_qualifier( int* errp=nullptr ) const
  {
    return get_info_< cl_kernel_arg_access_qualifier >( CL_KERNEL_ARG_ACCESS_QUALIFIER , errp );
  }
  cl_kernel_arg_type_qualifier 
  type_qualifier( int* errp=nullptr ) const
  {
    return get_info_< cl_kernel_arg_type_qualifier >( CL_KERNEL_ARG_TYPE_QUALIFIER , errp );
  }
  std::string type_name( int* errp=nullptr ) const
  {
    return get_info_string_( CL_KERNEL_ARG_TYPE_NAME , errp );
  }
  std::string name( int* errp=nullptr ) const
  {
    return get_info_string_( CL_KERNEL_ARG_NAME , errp );
  }
};
}}
