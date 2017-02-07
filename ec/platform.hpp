#pragma once

#include "cl.hpp"
#include "global.hpp"
#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace ec
{

class Platform
{
  friend void swap( Platform& , Platform& );
  cl_platform_id data_;
public:
  static std::vector<Platform> get_platforms( int* err = nullptr );
  
  Platform( cl_platform_id rhs=NULL )
    : data_( rhs )
  {
  }

  cl_platform_id get() const
  {
    return data_;
  }
  operator cl_platform_id() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }
  std::vector<Device> get_devices( cl_device_type type = CL_DEVICE_TYPE_ALL , int* errp=nullptr ) const;
  Device get_device( cl_device_type type = CL_DEVICE_TYPE_ALL , int* errp=nullptr ) const;

  void unload_compiler( int* errp=nullptr ) const
  {
    const int err = clUnloadPlatformCompiler( get() );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }


protected:
  std::string get_info_string_( cl_platform_info info , int* errp=nullptr ) const
  {
    size_t size;
    int err = clGetPlatformInfo( get() , info , 0 , nullptr , &size );
    EC_CHECK_ERROR( err , errp , return {} )
    std::unique_ptr<char[]> buf( new char[size] );
    err = clGetPlatformInfo( get() , info , size , buf.get() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return std::string( buf.get() , buf.get() + size );
  }

public:
  std::string version( int* errp=nullptr ) const
  {
    return get_info_string_( CL_PLATFORM_VERSION , errp );
  }
  std::string name( int *errp=nullptr ) const
  {
    return get_info_string_( CL_PLATFORM_NAME , errp );
  }
  std::string vendor( int *errp=nullptr ) const
  {
    return get_info_string_( CL_PLATFORM_VENDOR , errp );
  }
  std::string extensions( int *errp=nullptr ) const
  {
    return get_info_string_( CL_PLATFORM_EXTENSIONS , errp );
  }
};

inline void swap( Platform& l , Platform& r )
{
  std::swap( l.data_ , r.data_ );
}

inline std::vector<Platform> Platform::get_platforms( int* errp )
{
  int err;

  cl_uint num;
  err = clGetPlatformIDs( 0 , nullptr , &num );
  EC_CHECK_ERROR( err , errp , return {} )
  std::unique_ptr< cl_platform_id[] > buf( new cl_platform_id[num] );
  err = clGetPlatformIDs( num , buf.get() , nullptr );
  EC_CHECK_ERROR( err , errp , return {} )
  EC_SET_ERRP( errp )
  return std::vector<Platform>( buf.get() , buf.get() + num );
}

}
