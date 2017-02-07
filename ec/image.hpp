#pragma once

#include "cl.hpp"
#include "global.hpp"
#include "memory.hpp"
#include <vector>

namespace ec
{
class Image
  : public Memory
{
  friend void swap( Image& , Image& );
public:
  static std::vector<cl_image_format> supported_image_formats(
      cl_context context , cl_mem_flags flags ,
      cl_mem_object_type image_type , int* errp=nullptr )
  {
    int err;
    cl_uint num;
    err = clGetSupportedImageFormats( context , flags , image_type ,
        0 , nullptr , &num );
    EC_CHECK_ERROR( err , errp , return {} )
    std::vector< cl_image_format > ret( num );
    err = clGetSupportedImageFormats( context , flags , image_type ,
        num , ret.data() , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }
  Image()
    : Memory()
  {
  }
  Image( cl_mem data )
    : Memory( data )
  {
  }
  Image( cl_mem data , no_retain_t )
    : Memory( data , no_retain_t() )
  {
  }
  Image( Image const& rhs )
    : Memory( static_cast< Memory const& >( rhs ) )
  {
  }
  Image( Image&& rhs )
    : Memory( static_cast< Memory&& >( rhs ) )
  {
  }
  Image( cl_context context ,
      cl_mem_flags flags ,
      cl_image_format const* format ,
      cl_image_desc const* desc ,
      void* host_ptr=nullptr ,
      int* errp=nullptr )
  {
    int err;
    const cl_mem ret = clCreateImage( context , flags , format ,
        desc , host_ptr , &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }
  Image& operator=( Image const& rhs )
  {
    Memory::operator=( static_cast< Memory const& >( rhs ) );
    return *this;
  }
  Image& operator=( Image&& rhs )
  {
    Memory::operator=( static_cast< Memory&& >( rhs ) );
    return *this;
  }
protected:
  template < typename T >
  T get_info_( cl_image_info info , int* errp ) const
  {
    T ret;
    const int err = clGetImageInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  cl_image_format format( int* errp=nullptr ) const
  {
    return get_info_< cl_image_format >( CL_IMAGE_FORMAT , errp );
  }
  size_t element_size( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_ELEMENT_SIZE , errp );
  }
  size_t row_pitch( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_ROW_PITCH , errp );
  }
  size_t slice_pitch( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_SLICE_PITCH , errp );
  }
  size_t width( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_WIDTH , errp );
  }
  size_t height( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_HEIGHT , errp );
  }
  size_t depth( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_DEPTH , errp );
  }
  size_t array_size( int* errp=nullptr ) const
  {
    return get_info_< size_t >( CL_IMAGE_ARRAY_SIZE , errp );
  }
  cl_uint num_mip_levels( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_IMAGE_NUM_MIP_LEVELS , errp );
  }
  cl_uint num_samples( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_IMAGE_NUM_SAMPLES , errp );
  }
};

inline void swap( Image& l , Image& r )
{
  std::swap( l.data_ , r.data_ );
}

}
