#pragma once

#include "cl.hpp"
#include "global.hpp"
#include "event.hpp"
#include "ndrange.hpp"
#include "image_dimension.hpp"
#include "list_view.hpp"
#include <memory>
#include <utility>

namespace ec
{

class CommandQueue
{
  friend void swap( CommandQueue& , CommandQueue& );
protected:
  cl_command_queue data_;

  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseCommandQueue( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainCommandQueue( get() );
    }
  }

public:
  constexpr static cl_command_queue_properties OUT_OF_ORDER_EXEC = 
    CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
  constexpr static cl_command_queue_properties PROFILING =
    CL_QUEUE_PROFILING_ENABLE;

  CommandQueue()
    : data_( NULL )
  {
  }
  CommandQueue( cl_command_queue data )
    : data_( data )
  {
    retain_if();
  }
  CommandQueue( cl_command_queue data , no_retain_t )
    : data_( data )
  {
  }
  CommandQueue( cl_context context ,
      cl_device_id device ,
      cl_command_queue_properties properties=0 ,
      int* errp=nullptr )
  {
    int err;
    const cl_command_queue ret = clCreateCommandQueue( context , device , properties , &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }
  ~CommandQueue()
  {
    release_if();
  }
  CommandQueue( CommandQueue const& rhs )
    : data_( rhs.data_ )
  {
    retain_if();
  }
  CommandQueue( CommandQueue&& rhs )
    : data_( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  CommandQueue& operator=( CommandQueue const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  CommandQueue& operator=( CommandQueue&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }

  cl_command_queue get() const
  {
    return data_;
  }
  operator cl_command_queue() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  auto marker( 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ret;
    const int err = clEnqueueMarkerWithWaitList( get() , 
        events.size() , events.data() ,
        &ret );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )

    return detail::make_system_event( ret );
  }
  auto barrier(
      detail::list_view<cl_event> const& events ,
      int* errp = nullptr ) const
  {
    cl_event ret;
    const int err = clEnqueueBarrierWithWaitList( get() , 
        events.size() , events.data() ,
        &ret );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ret );
  }

  auto read_buffer( cl_mem buffer , cl_bool block ,
      size_t offset , size_t size , void* ptr , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueReadBuffer( get() , buffer , block , 
        offset , size , ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto write_buffer( cl_mem buffer , cl_bool block ,
      size_t offset , size_t size , void const* ptr , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueWriteBuffer( get() , buffer , block , 
        offset , size , ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto copy_buffer( cl_mem src , cl_mem dst ,
      size_t src_offset , size_t dst_offset ,
      size_t size , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueCopyBuffer( get() , src , dst ,
        src_offset , dst_offset , size ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto fill_buffer( cl_mem buffer ,
      void const* pattern , size_t pattern_size ,
      size_t offset , size_t bytes , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
#ifndef NDEBUG
    if( (bytes % pattern_size) != 0 )
    {
#ifdef EC_THROW_EXCEPTION
      throw std::runtime_error( "bytes must be times of pattern_size" );
#endif
    }
#endif
    cl_event ev;
    const int err = clEnqueueFillBuffer( get() , buffer ,
        pattern , pattern_size , offset , bytes ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  template < typename T >
  auto fill_buffer( cl_mem buffer ,
      T const& pattern ,
      size_t offsetbytes , size_t count ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    return fill_buffer( buffer ,
        &pattern , sizeof(T) ,
        offsetbytes , sizeof(T)*count ,
        events , errp );
  }

  template < typename T = void >
  T*
  map_buffer( cl_mem buffer ,
      cl_bool block , cl_map_flags flags ,
      size_t offset , size_t size , 
      detail::list_view<cl_event> const& events ,
      Event& event ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    int err;
    void* ret = clEnqueueMapBuffer( get() , buffer , block , flags ,
        offset , size , 
        events.size() , events.data() ,
        &ev , &err );
    EC_CHECK_ERROR( err , errp , return nullptr )
    EC_SET_ERRP( errp )

    event = detail::make_system_event( ev );
    return reinterpret_cast< T* >( ret );
  }
  template < typename T = void >
  T*
  map_buffer( cl_mem buffer ,
      cl_bool block , cl_map_flags flags ,
      size_t offset , size_t size , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    Event event;
    return map_buffer< T >( buffer , block , flags , offset , size ,
        events , event ,
        errp );
  }
  auto unmap( cl_mem mem ,
      void* ptr ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueUnmapMemObject( get() , mem , ptr , 
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto read_buffer_rect( cl_mem buffer , cl_bool block ,
      ImageOffset const& buffer_offset , ImageOffset const& host_offset ,
      ImageSize const& size ,
      ImagePitch const& buffer_pitch , ImagePitch const& host_pitch ,
      void* ptr ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueReadBufferRect( get() , buffer , block ,
        buffer_offset.data() , host_offset.data() ,
        size.data() ,
        buffer_pitch.row() , buffer_pitch.slice() ,
        host_pitch.row() , host_pitch.slice() ,
        ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto write_buffer_rect( cl_mem buffer , cl_bool block ,
      ImageOffset const& buffer_offset , ImageOffset const& host_offset ,
      ImageSize const& size ,
      ImagePitch const& buffer_pitch , ImagePitch const& host_pitch ,
      void const* ptr ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueWriteBufferRect( get() , buffer , block ,
        buffer_offset.data() , host_offset.data() ,
        size.data() ,
        buffer_pitch.row() , buffer_pitch.slice() ,
        host_pitch.row() , host_pitch.slice() ,
        ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto copy_buffer_rect( cl_mem src , cl_mem dst ,
      ImageOffset const& src_offset , ImageOffset const& dst_offset ,
      ImageSize const& size ,
      ImagePitch const& src_pitch , ImagePitch const& dst_pitch ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueCopyBufferRect( get() ,
        src , dst ,
        src_offset.data() , dst_offset.data() ,
        size.data() ,
        src_pitch.row() , src_pitch.slice() ,
        dst_pitch.row() , dst_pitch.slice() ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }

  auto read_image( cl_mem image , cl_bool block ,
      ImageOffset const& offset , ImageSize const& size ,
      ImagePitch const& pitch ,
      void* ptr ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueReadImage( get() , image , block ,
        offset.data() , size.data() , pitch.row() , pitch.slice() , 
        ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto write_image( cl_mem image , cl_bool block ,
      ImageOffset const& offset , ImageSize const& size ,
      ImagePitch const& pitch ,
      void const* ptr ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueWriteImage( get() , image , block ,
        offset.data() , size.data() , pitch.row() , pitch.slice() ,
        ptr ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto copy_image( cl_mem src , cl_mem dst ,
      ImageOffset const& src_offset , ImageOffset const& dst_offset ,
      ImageSize const& size ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueCopyImage( get() , src , dst ,
        src_offset.data() , dst_offset.data() ,
        size.data() ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto fill_image( cl_mem image ,
      void const* color ,
      ImageOffset const& offset , ImageSize const& size ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueFillImage( get() , image ,
        color , offset.data() , size.data() ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }

  auto copy_image_to_buffer( cl_mem image , cl_mem buffer ,
      ImageOffset const& image_offset , ImageSize const& image_size ,
      size_t buffer_offset ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueCopyImageToBuffer( get() ,
        image , buffer ,
        image_offset.data() , image_size.data() , buffer_offset ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }
  auto copy_buffer_to_image( cl_mem buffer , cl_mem image ,
      size_t buffer_offset ,
      ImageOffset const& image_offset , ImageSize const& image_size ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueCopyBufferToImage( get() ,
        buffer , image ,
        buffer_offset , image_offset.data() , image_size.data() ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }

  template < typename T = void >
  T*
  map_image( cl_mem image ,
      cl_bool block , cl_map_flags flags ,
      ImageOffset const& offset , ImageSize const& size ,
      ImagePitch& host_pitch ,
      detail::list_view<cl_event> const& events ,
      Event& event ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    int err;
    void* ret = clEnqueueMapImage( get() , image , block , flags ,
        offset.data() , size.data() , 
        host_pitch.data_ , host_pitch.data_ + 1 ,
        events.size() , events.data() ,
        &ev , &err );
    EC_CHECK_ERROR( err , errp , return nullptr )
    EC_SET_ERRP( errp )

    event = detail::make_system_event( ev );
    return reinterpret_cast< T* >( ret );
  }
  template < typename T = void >
  T*
  map_image( cl_mem image ,
      cl_bool block , cl_map_flags flags ,
      ImageOffset const& offset , ImageSize const& size ,
      ImagePitch& host_pitch ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    Event event;
    return map_image< T >( image , block , flags ,
        offset , size , host_pitch , 
        events , event ,
        errp );
  }


  auto ndrange( cl_kernel kernel , 
      NDRange const& global_offsets ,
      NDRange const& global_size ,
      NDRange const& local_size ,
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
#ifndef NDEBUG
    if( global_offsets.dim() != global_size.dim() ||
        global_size.dim() != local_size.dim() )
    {
#ifdef EC_THROW_EXCEPTION
      throw std::runtime_error( "ndrange dimension different" );
#endif
    }
#endif
    cl_event ev;
    const int err = clEnqueueNDRangeKernel( get() , kernel , 
        global_offsets.dim() ,
        global_offsets.data() , global_size.data() , local_size.data() ,
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }

  auto task( cl_kernel kernel , 
      detail::list_view<cl_event> const& events ,
      int* errp=nullptr ) const
  {
    cl_event ev;
    const int err = clEnqueueTask( get() , kernel , 
        events.size() , events.data() ,
        &ev );
    EC_CHECK_ERROR( err , errp , return detail::make_system_event() )
    EC_SET_ERRP( errp )
    return detail::make_system_event( ev );
  }

  void finish() const
  {
    clFinish( get() );
  }
  void flush() const
  {
    clFlush( get() );
  }


protected:
  template < typename T >
  T get_info_( cl_command_queue_info info , int* errp ) const
  {
    T ret;
    const int err = clGetCommandQueueInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  Context context( int* errp=nullptr ) const;
  Device device( int* errp=nullptr ) const;
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_QUEUE_REFERENCE_COUNT , errp );
  }
  cl_command_queue_properties properties( int* errp=nullptr ) const
  {
    return get_info_< cl_command_queue_properties >( CL_QUEUE_PROPERTIES , errp );
  }

};

inline void swap( CommandQueue& l , CommandQueue& r )
{
  std::swap( l.data_ , r.data_ );
}

}
