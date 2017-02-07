#pragma once

#include "cl.hpp"
#include "global.hpp"
#include <utility>
#include <vector>

namespace ec { namespace detail
{

class EventBase
{
  friend void swap( EventBase& , EventBase& );
protected:
  cl_event data_;

public:
  EventBase( cl_event data=NULL )
    : data_( data )
  {
  }
  EventBase( cl_context context , int* errp=nullptr )
  {
    int err;
    const cl_event ret = clCreateUserEvent( context , &err );
    EC_CHECK_ERROR( err , errp , data_=NULL; return )
    EC_SET_ERRP( errp )
    data_ = ret;
  }

  void release_if() const
  {
    if( data_ != NULL )
    {
      clReleaseEvent( get() );
    }
  }
  void retain_if() const
  {
    if( data_ != NULL )
    {
      clRetainEvent( get() );
    }
  }

  cl_event get() const
  {
    return data_;
  }
  operator cl_event() const
  {
    return data_;
  }
  operator bool() const
  {
    return data_ != NULL;
  }

  void set_status( cl_int execution_status , int* errp=nullptr ) const
  {
    const int err = clSetUserEventStatus( get() , execution_status );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }
  void complete( int *errp=nullptr ) const
  {
    set_status( CL_COMPLETE , errp );
  }
  // callback event , status , userdata
  void set_callback( void (*func)( cl_event , cl_int , void* ) , cl_int status ,
      void* userdata=nullptr , int* errp=nullptr ) const
  {
    const int err = clSetEventCallback( get() , status , func , userdata );
    EC_CHECK_ERROR( err , errp , return )
    EC_SET_ERRP( errp )
  }
  // callback event , status , userdata
  void set_callback( void (*func)( cl_event , cl_int , void* ) , 
      void* userdata=nullptr , int* errp=nullptr ) const
  {
    set_callback( func , CL_COMPLETE , userdata , errp );
  }

protected:
  template < typename T >
  T get_info_( cl_event_info info , int* errp ) const
  {
    T ret;
    const int err = clGetEventInfo( get() , info , sizeof(T) , &ret , nullptr );
    EC_CHECK_ERROR( err , errp , return {} )
    EC_SET_ERRP( errp )
    return ret;
  }

public:
  cl_uint reference_count( int* errp=nullptr ) const
  {
    return get_info_< cl_uint >( CL_EVENT_REFERENCE_COUNT , errp );
  }
  cl_int execution_status( int* errp=nullptr ) const
  {
    return get_info_< cl_int >( CL_EVENT_COMMAND_EXECUTION_STATUS , errp );
  }
  cl_command_type command_type( int* errp=nullptr ) const
  {
    return get_info_< cl_command_type >( CL_EVENT_COMMAND_TYPE , errp );
  }
  Context context( int* errp=nullptr ) const;
  CommandQueue command_queue( int* errp=nullptr ) const;
};
inline void swap( EventBase& l , EventBase& r )
{
  std::swap( l.data_ , r.data_ );
}

class SharedEvent
  : public EventBase
{
public:
  SharedEvent()
    : EventBase()
  {
  }
  SharedEvent( cl_event data )
    : EventBase( data )
  {
    retain_if();
  }
  SharedEvent( cl_event data , no_retain_t )
    : EventBase( data )
  {
  }
  SharedEvent( cl_context context , int* errp=nullptr )
    : EventBase( context , errp )
  {
  }

  ~SharedEvent()
  {
    release_if();
  }
  SharedEvent( SharedEvent const& rhs )
    : EventBase( rhs.data_ )
  {
    retain_if();
  }
  SharedEvent( SharedEvent&& rhs )
    : EventBase( rhs.data_ )
  {
    rhs.data_ = NULL;
  }
  SharedEvent& operator=( SharedEvent const& rhs )
  {
    release_if();
    data_ = rhs.data_;
    retain_if();
    return *this;
  }
  SharedEvent& operator=( SharedEvent&& rhs )
  {
    release_if();
    data_ = rhs.data_;
    rhs.data_ = NULL;
    return *this;
  }
};
class ShellEvent
  : public EventBase
{
public:
  ShellEvent()
    : EventBase()
  {
  }
  ShellEvent( cl_event event )
    : EventBase( event )
  {
  }
};

inline EventBase make_system_event( cl_event event )
{
  return { event };
}
inline EventBase make_system_event()
{
  return {};
}

}}

namespace ec
{

using Event = detail::EventBase;

}
