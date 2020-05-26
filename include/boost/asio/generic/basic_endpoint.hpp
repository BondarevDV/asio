//
// generic/basic_endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_GENERIC_BASIC_ENDPOINT_HPP
#define BOOST_ASIO_GENERIC_BASIC_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/detail/type_traits.hpp>

#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
# include <boost/asio/detail/apple_nw_ptr.hpp>
# include <Network/Network.h>
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
# include <boost/asio/generic/detail/endpoint.hpp>
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace generic {

/// Describes an endpoint for any socket type.
/**
 * The boost::asio::generic::basic_endpoint class template describes an endpoint
 * that may be associated with any socket type.
 *
 * @note The socket types sockaddr type must be able to fit into a
 * @c sockaddr_storage structure.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 *
 * @par Concepts:
 * Endpoint.
 */
template <typename Protocol>
class basic_endpoint
{
public:
  /// The protocol type associated with the endpoint.
  typedef Protocol protocol_type;

  /// The type of the endpoint structure. This type is dependent on the
  /// underlying implementation of the socket layer.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined data_type;
#elif !defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  typedef boost::asio::detail::socket_addr_type data_type;
#endif

  /// Default constructor.
  basic_endpoint() BOOST_ASIO_NOEXCEPT
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : endpoint_(),
      protocol_(boost::asio::detail::apple_nw_ptr<nw_parameters_t>(), 0)
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  {
  }

#if !defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  /// Construct an endpoint from the specified socket address.
  basic_endpoint(const void* socket_address,
      std::size_t socket_address_size, int socket_protocol = 0)
    : impl_(socket_address, socket_address_size, socket_protocol)
  {
  }
#endif // !defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)

  /// Construct an endpoint from the specific endpoint type.
  template <typename Endpoint>
  basic_endpoint(const Endpoint& endpoint,
      typename enable_if<
        is_convertible<typename Endpoint::protocol_type, protocol_type>::value
      >::type* = 0)
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : endpoint_(endpoint.apple_nw_create_endpoint()),
      protocol_(endpoint.protocol())
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : impl_(endpoint.data(), endpoint.size(), endpoint.protocol().protocol())
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  {
  }

  /// Copy constructor.
  basic_endpoint(const basic_endpoint& other)
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : endpoint_(other.endpoint_),
      protocol_(other.protocol_)
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : impl_(other.impl_)
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  {
  }

#if defined(BOOST_ASIO_HAS_MOVE)
  /// Move constructor.
  basic_endpoint(basic_endpoint&& other)
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : endpoint_(BOOST_ASIO_MOVE_CAST(
          boost::asio::detail::apple_nw_ptr<nw_endpoint_t>)(
            other.endpoint_)),
      protocol_(BOOST_ASIO_MOVE_CAST(protocol_type)(other.protocol_))
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    : impl_(other.impl_)
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  {
  }
#endif // defined(BOOST_ASIO_HAS_MOVE)

  /// Assign from another endpoint.
  basic_endpoint& operator=(const basic_endpoint& other)
  {
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    endpoint_ = other.endpoint_;
    protocol_ = other.protocol_;
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    impl_ = other.impl_;
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return *this;
  }

#if defined(BOOST_ASIO_HAS_MOVE)
  /// Move-assign from another endpoint.
  basic_endpoint& operator=(basic_endpoint&& other)
  {
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    endpoint_ = BOOST_ASIO_MOVE_CAST(
        boost::asio::detail::apple_nw_ptr<nw_endpoint_t>)(
          other.endpoint_);
    protocol_ = BOOST_ASIO_MOVE_CAST(protocol_type)(other.protocol_);
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    impl_ = other.impl_;
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return *this;
  }
#endif // defined(BOOST_ASIO_HAS_MOVE)

  /// The protocol associated with the endpoint.
  protocol_type protocol() const
  {
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return protocol_;
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return protocol_type(impl_.family(), impl_.protocol());
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  }

#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  // The following functions comprise the extensible interface for the Endpoint
  // concept when targeting the Apple Network Framework.

  // Create a new native object corresponding to the endpoint.
  boost::asio::detail::apple_nw_ptr<nw_endpoint_t>
  apple_nw_create_endpoint() const
  {
    return endpoint_;
  }

  // Set the endpoint from the native object.
  void apple_nw_set_endpoint(
      boost::asio::detail::apple_nw_ptr<nw_endpoint_t> new_ep)
  {
    endpoint_ = new_ep;
  }

  // Set the protocol.
  void apple_nw_set_protocol(protocol_type new_protocol)
  {
    protocol_ = new_protocol;
  }
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  /// Get the underlying endpoint in the native type.
  data_type* data()
  {
    return impl_.data();
  }

  /// Get the underlying endpoint in the native type.
  const data_type* data() const
  {
    return impl_.data();
  }

  /// Get the underlying size of the endpoint in the native type.
  std::size_t size() const
  {
    return impl_.size();
  }

  /// Set the underlying size of the endpoint in the native type.
  void resize(std::size_t new_size)
  {
    impl_.resize(new_size);
  }

  /// Get the capacity of the endpoint in the native type.
  std::size_t capacity() const
  {
    return impl_.capacity();
  }
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)

  /// Compare two endpoints for equality.
  friend bool operator==(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return e1.endpoint_ == e2.endpoint_ && e1.protocol_ == e2.protocol_;
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
    return e1.impl_ == e2.impl_;
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  }

  /// Compare two endpoints for inequality.
  friend bool operator!=(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
    return !(e1 == e2);
  }

#if !defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  /// Compare endpoints for ordering.
  friend bool operator<(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
    return e1.impl_ < e2.impl_;
  }

  /// Compare endpoints for ordering.
  friend bool operator>(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
    return e2.impl_ < e1.impl_;
  }

  /// Compare endpoints for ordering.
  friend bool operator<=(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
    return !(e2 < e1);
  }

  /// Compare endpoints for ordering.
  friend bool operator>=(const basic_endpoint<Protocol>& e1,
      const basic_endpoint<Protocol>& e2)
  {
    return !(e1 < e2);
  }
#endif // !defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)

private:
#if defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  // The underlying native endpoint.
  boost::asio::detail::apple_nw_ptr<nw_endpoint_t> endpoint_;

  // The associated protocol object.
  Protocol protocol_;
#else // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
  // The underlying generic endpoint.
  boost::asio::generic::detail::endpoint impl_;
#endif // defined(BOOST_ASIO_HAS_APPLE_NETWORK_FRAMEWORK)
};

} // namespace generic
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_GENERIC_BASIC_ENDPOINT_HPP
