#pragma once

#include <OpenCL/cl.h>
#include <exception>
#include <iostream>

#define EC_THROW_EXCEPTION

#ifdef EC_THROW_EXCEPTION
#define EC_THROW_IF( Var ) ::ec::throw_exception(Var)
#else
#define EC_TRROW_IF( Var )
#endif

#define EC_CHECK_ERROR( Var , Ptr , Exp ) \
    if( Var != CL_SUCCESS ) \
    { if( Ptr ){ *Ptr = Var; } \
      EC_THROW_IF( Var ); \
      Exp; }

#define EC_SET_ERRP(Ptr) if(Ptr){*Ptr=CL_SUCCESS;}

namespace ec
{
void throw_exception( int err );

class exception
  : public std::exception
{
  int err_;
public:
  
  exception( int err = CL_SUCCESS )
    : err_( err )
  {
  }
  int error_code() const
  {
    return err_;
  }
  const char* what() const noexcept override;
};

struct no_retain_t {};
struct retain_t {};
}
