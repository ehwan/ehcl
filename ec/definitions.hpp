#pragma once

#include "platform.hpp"
#include "device.hpp"
#include "context.hpp"
#include "command_queue.hpp"
#include "memory.hpp"
#include "buffer.hpp"
#include "program.hpp"
#include "kernel.hpp"
#include "sampler.hpp"
#include "event.hpp"
#include "list_view.hpp"

namespace ec
{

inline std::vector<Device> Platform::get_devices( cl_device_type type , int* errp ) const
{
  int err;

  cl_uint num;
  err = clGetDeviceIDs( get() , type , 0 , nullptr , &num );
  EC_CHECK_ERROR( err , errp , {} )
  std::unique_ptr< cl_device_id[] > buf( new cl_device_id[num] );
  err = clGetDeviceIDs( get() , type , num , buf.get() , nullptr );
  EC_CHECK_ERROR( err , errp , {} )
  EC_SET_ERRP( errp )
  std::vector<Device> ret( num );
  std::transform( buf.get() , buf.get() + num , ret.begin() ,
      []( cl_device_id id ) -> Device
      {
        return Device( id , no_retain_t() );
      } );
  return ret;
}
inline Device Platform::get_device( cl_device_type type , int* errp ) const
{
  cl_device_id ret;
  const int err = clGetDeviceIDs( get() , type , 1 , &ret , nullptr );
  EC_CHECK_ERROR( err , errp , {} )
  EC_SET_ERRP( errp )
  return { ret , no_retain_t() };
}
inline Device Context::device( int* errp ) const
{
  return { get_info_< cl_device_id >( CL_CONTEXT_DEVICES , errp ) };
}
inline std::vector<Device> Context::devices( int* errp ) const
{
  auto buf = get_info_raw_< cl_device_id >( CL_CONTEXT_DEVICES , errp );
  return { buf.begin() , buf.end() };
}
inline Program Context::link( detail::list_view<cl_device_id> const& devices ,
      const char* options ,
      detail::list_view<cl_program> const& programs ,
      void (*func)( cl_program , void* ) , void* userdata ,
      int* errp ) const
{
  int err;
  const cl_program ret = clLinkProgram( get() ,
      devices.size() , devices.data() ,
      options ,
      programs.size() , programs.data() ,
      func , userdata , &err );
  EC_CHECK_ERROR( err , errp , return {} )
  EC_SET_ERRP( errp )
  return Program( ret , no_retain_t() );
}
inline Context CommandQueue::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_QUEUE_CONTEXT , errp ) };
}
inline Device CommandQueue::device( int* errp ) const
{
  return { get_info_< cl_device_id >( CL_QUEUE_DEVICE , errp ) };
}
inline Context Memory::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_MEM_CONTEXT , errp ) };
}
inline Buffer Buffer::parent( int* errp ) const
{
  return { get_info_< cl_mem >( CL_MEM_ASSOCIATED_MEMOBJECT , errp ) };
}
inline Context Program::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_PROGRAM_CONTEXT , errp ) };
}
inline Device Program::device( int* errp ) const
{
  return { get_info_< cl_device_id >( CL_PROGRAM_DEVICES , errp ) };
}
inline std::vector<Device> Program::devices( int* errp ) const
{
  auto buf = get_info_raw_< cl_device_id >( CL_PROGRAM_DEVICES , errp );
  return { buf.begin() , buf.end() };
}
inline Kernel Program::kernel( const char* name , int* errp ) const
{
  int err;
  cl_kernel ret = clCreateKernel( get() , name , &err );
  EC_CHECK_ERROR( err , errp , {} )
  EC_SET_ERRP( errp )
  return { ret , no_retain_t() };
}
inline Context Kernel::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_KERNEL_CONTEXT , errp ) };
}
inline Program Kernel::program( int* errp ) const
{
  return { get_info_< cl_program >( CL_KERNEL_PROGRAM , errp ) };
}
inline Context Sampler::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_SAMPLER_CONTEXT , errp ) };
}
inline Context detail::EventBase::context( int* errp ) const
{
  return { get_info_< cl_context >( CL_EVENT_CONTEXT , errp ) };
}
inline CommandQueue detail::EventBase::command_queue( int* errp ) const
{
  return { get_info_< cl_command_queue >( CL_EVENT_COMMAND_QUEUE , errp ) };
}

}

namespace ec { namespace detail
{

template <>
struct device_info_t< CL_DEVICE_PLATFORM >
{
  using type = Platform;
  static type apply( cl_device_id id , cl_device_info info , int* errp )
  {
    return { device_info_direct_t< cl_platform_id >::apply( id , info , errp ) };
  }
};

}}
