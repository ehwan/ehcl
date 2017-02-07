#pragma once

#include <OpenCL/cl.h>
#include <cstddef>
#include <type_traits>
#include <iterator>


namespace ec { namespace detail {

using size_type = cl_uint;

template < typename T >
class list_view
{
  using this_type = list_view<T>;
protected:
  T value_{0};
  T const* data_;
  size_type size_;

public:
  list_view( this_type const& ) = delete;
  list_view( this_type&& ) = delete;
  list_view()
    : data_( nullptr ) ,
      size_( 0 )
  {
  }
  list_view( std::nullptr_t )
    : list_view()
  {
  }
  list_view( T const* data , size_type size )
    : data_( size==0 ? nullptr : data ) ,
      size_( size )
  {
  }
  list_view( T value )
    : value_( value ) ,
      data_( &value_ ) ,
      size_( 1 )
  {
  }
  template < typename T_ >
  list_view( T_ const& value ,
      std::enable_if_t< std::is_convertible<T_,T>::value >* = nullptr ,
      decltype(value.get())* = nullptr )
    : list_view( value.get() )
  {
  }
  template < typename T_ , size_type N >
  list_view( T_ const (&arr)[N] ,
      std::enable_if_t< std::is_convertible<T_,T>::value >* = nullptr )
  {
    T_ const* temp = arr;
    data_ = reinterpret_cast<T const*>(temp);
    size_ = N;
  }
  template < typename Container >
  list_view( Container const& container ,
      std::enable_if_t< std::is_convertible<Container,T>::value == false >* = nullptr ,
      decltype(container.data())* = nullptr ,
      decltype(container.size())* = nullptr )
    : list_view(
        reinterpret_cast<T const*>(container.data()) ,
        static_cast<size_type>(container.size()) )
  {
  }

  T const* data() const
  {
    return data_;
  }
  size_type size() const
  {
    return size_;
  }
};
template <>
class list_view<const char*>
{
  using this_type = list_view<const char*>;
protected:
  const char* value_{nullptr};
  const char** data_;
  size_type size_;

public:
  list_view( this_type const& ) = delete;
  list_view( this_type&& ) = delete;
  list_view()
    : data_( nullptr ) ,
      size_( 0 )
  {
  }
  list_view( std::nullptr_t )
    : list_view()
  {
  }
  list_view( const char** data , size_type size )
    : data_( size==0 ? nullptr : data ) ,
      size_( size )
  {
  }
  list_view( const char* value )
    : value_( value ) ,
      data_( &value_ ) ,
      size_( 1 )
  {
  }
  template < typename T_ , size_type N >
  list_view( T_ (&arr)[N] ,
      std::enable_if_t< std::is_convertible<T_,const char*>::value >* = nullptr )
  {
    T_* temp = arr;
    data_ = reinterpret_cast<const char**>(temp);
    size_ = N;
  }
  template < typename Container >
  list_view( Container& container , 
      std::enable_if_t< std::is_convertible<Container,const char*>::value == false >* = nullptr ,
      decltype(container.data())* = nullptr , 
      decltype(container.size())* = nullptr )
    : list_view( 
        reinterpret_cast<const char**>(container.data()) ,
        static_cast<size_type>(container.size()) )
  {
  }

  const char** data() const
  {
    return data_;
  }
  size_type size() const
  {
    return size_;
  }
};

}}
