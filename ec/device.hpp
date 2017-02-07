#pragma once

#include "cl.hpp"
#include <utility>
#include <string>
#include <memory>

namespace ec { namespace detail
{

template < cl_device_info Info >
struct device_info_t;
  
}}

namespace ec
{

class Device
{
  friend void swap( Device& , Device& );
  cl_device_id data_;

  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseDevice( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainDevice( get() );
    }
  }
  
public:
  Device()
    : data_( NULL )
  {
  }
  Device( cl_device_id data )
    : data_( data )
  {
    retain_if();
  }
  Device( cl_device_id data , no_retain_t )
    : data_( data )
  {
  }

  ~Device()
  {
    release_if();
  }
  Device( Device const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Device( Device&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  Device& operator = ( Device const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Device& operator = ( Device&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }

  cl_device_id get() const
  {
    return data_;
  }
  operator cl_device_id() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  template < cl_device_info Info >
  typename detail::device_info_t< Info >::type
  get_info( int* errp=nullptr ) const
  {
    return detail::device_info_t< Info >::apply( get() , Info , errp );
  }
};
inline void swap( Device& l , Device& r )
{
  std::swap( l.data_ , r.data_ );
}

}

namespace ec { namespace detail
{
template < typename RetType >
struct device_info_direct_t
{
  using type = RetType;
  static type apply( cl_device_id id , cl_device_info info , int* errp )
  {
    type ret;
    const int err = clGetDeviceInfo( id , info , sizeof(type) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )

    return ret;
  }
};
struct device_info_string_t
{
  using type = std::string;
  static type apply( cl_device_id id , cl_device_info info , int* errp )
  {
    size_t len;
    int err = clGetDeviceInfo( id , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::unique_ptr<char[]> buf( new char[len] );
    err = clGetDeviceInfo( id , info , len , buf.get() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )

    return std::string( buf.get() , buf.get() + len );
  }
};
template < typename T >
struct device_info_raw_t
{
  using type = std::vector< T >;
  static type apply( cl_device_id id , cl_device_info info , int* errp )
  {
    size_t len;
    int err = clGetDeviceInfo( id , info , 0 , nullptr , &len );
    EC_CHECK_ERROR( err , errp , return {} )
    std::vector< T > buf( len/sizeof(T) );
    err = clGetDeviceInfo( id , info , len , buf.data() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )

    return buf;
  }
};

#define EC_DEVICE_INFO_DIRECT(Name,Type) \
  template <> \
  struct device_info_t< Name > \
    : device_info_direct_t< Type > \
  {}
#define EC_DEVICE_INFO_RAW(Name,Type) \
  template <> \
  struct device_info_t< Name > \
    : device_info_raw_t< Type > \
  {}
#define EC_DEVICE_INFO_STRING(Name) \
  template <> \
  struct device_info_t< Name > \
    : device_info_string_t \
  {}
EC_DEVICE_INFO_DIRECT( CL_DEVICE_TYPE , cl_device_type );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_VENDOR_ID , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_COMPUTE_UNITS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS , cl_uint );
EC_DEVICE_INFO_RAW(    CL_DEVICE_MAX_WORK_ITEM_SIZES , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_WORK_GROUP_SIZE , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_CLOCK_FREQUENCY , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_ADDRESS_BITS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_MEM_ALLOC_SIZE , cl_ulong );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE_SUPPORT , cl_bool );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_READ_IMAGE_ARGS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_WRITE_IMAGE_ARGS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE2D_MAX_WIDTH , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE2D_MAX_HEIGHT , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE3D_MAX_WIDTH , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE3D_MAX_HEIGHT , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE3D_MAX_DEPTH , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE_MAX_BUFFER_SIZE , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_IMAGE_MAX_ARRAY_SIZE , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_SAMPLERS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_PARAMETER_SIZE , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MEM_BASE_ADDR_ALIGN , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_SINGLE_FP_CONFIG , cl_device_fp_config );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_DOUBLE_FP_CONFIG , cl_device_fp_config );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_GLOBAL_MEM_CACHE_TYPE , cl_device_mem_cache_type );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_GLOBAL_MEM_CACHE_SIZE , cl_ulong );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_GLOBAL_MEM_SIZE , cl_ulong );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE , cl_ulong );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_MAX_CONSTANT_ARGS , cl_uint );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_ENDIAN_LITTLE , cl_bool );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_AVAILABLE , cl_bool );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_COMPILER_AVAILABLE , cl_bool );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_LINKER_AVAILABLE , cl_bool );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_EXECUTION_CAPABILITIES , cl_device_exec_capabilities );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_QUEUE_PROPERTIES , cl_command_queue_properties );
EC_DEVICE_INFO_STRING( CL_DEVICE_BUILT_IN_KERNELS );
// platform
EC_DEVICE_INFO_STRING( CL_DEVICE_NAME );
EC_DEVICE_INFO_STRING( CL_DEVICE_VENDOR );
EC_DEVICE_INFO_STRING( CL_DRIVER_VERSION );
EC_DEVICE_INFO_STRING( CL_DEVICE_PROFILE );
EC_DEVICE_INFO_STRING( CL_DEVICE_VERSION );
EC_DEVICE_INFO_STRING( CL_DEVICE_OPENCL_C_VERSION );
EC_DEVICE_INFO_STRING( CL_DEVICE_EXTENSIONS );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_PRINTF_BUFFER_SIZE , size_t );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_PREFERRED_INTEROP_USER_SYNC , cl_bool );
// parent device
EC_DEVICE_INFO_DIRECT( CL_DEVICE_PARTITION_MAX_SUB_DEVICES , cl_uint );
EC_DEVICE_INFO_RAW( CL_DEVICE_PARTITION_PROPERTIES , cl_device_partition_property );
EC_DEVICE_INFO_DIRECT( CL_DEVICE_REFERENCE_COUNT , cl_uint );

template <>
struct device_info_t< CL_DEVICE_PARENT_DEVICE >
{
  using type = Device;
  static type apply( cl_device_id id , cl_device_info info , int* errp )
  {
    return { device_info_direct_t< cl_device_id >::apply( id , info , errp ) };
  }
};


#undef EC_DEVICE_INFO_DIRECT
#undef EC_DEVICE_INFO_RAW
#undef EC_DEVICE_INFO_STRING

}}
