#pragma once

#include "cl.hpp"
#include "global.hpp"
#include <utility>

namespace ec
{
class Sampler
{
  friend void swap( Sampler& , Sampler& );
protected:
  cl_sampler data_;
  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseSampler( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainSampler( get() );
    }
  }
public:
  Sampler()
    : data_( NULL )
  {
  }
  Sampler( cl_sampler data )
    : data_( data )
  {
    retain_if();
  }
  Sampler( cl_sampler data , no_retain_t )
    : data_( data )
  {
  }
  Sampler( cl_context context ,
      cl_bool normalized ,
      cl_addressing_mode addressing_mode ,
      cl_filter_mode filter_mode ,
      int* errp=nullptr )
  {
    int err;
    const cl_sampler ret = clCreateSampler( context ,
        normalized , addressing_mode , filter_mode , &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }
  ~Sampler()
  {
    release_if();
  }
  Sampler( Sampler const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  Sampler( Sampler&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  Sampler& operator=( Sampler const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  Sampler& operator=( Sampler&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
  cl_sampler get() const
  {
    return data_;
  }
  operator cl_sampler() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

protected:
  template < typename T >
  T get_info_( cl_sampler_info info , int* errp ) const
  {
    T ret;
    const int err = clGetSamplerInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  Context context( int* errp=nullptr ) const;
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_SAMPLER_REFERENCE_COUNT , errp );
  }
  cl_bool normalized_coord( int* errp=nullptr ) const
  {
    return get_info_< cl_bool >( CL_SAMPLER_NORMALIZED_COORDS , errp );
  }
  cl_addressing_mode addressing_mode( int* errp=nullptr ) const
  {
    return get_info_< cl_addressing_mode >( CL_SAMPLER_ADDRESSING_MODE , errp );
  }
  cl_filter_mode filter_mode( int* errp=nullptr ) const
  {
    return get_info_< cl_filter_mode >( CL_SAMPLER_FILTER_MODE , errp );
  }
};
inline void swap( Sampler& l , Sampler& r )
{
  std::swap( l.data_ , r.data_ );
}

}
