// Copyright (c) 2015, Sergey Abbakumov
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TCPSocket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <string>

namespace rst {

TCPSocket::TCPSocket()
    : was_connected_(false), was_initialized_(false), fd_(0) {}

TCPSocket::TCPSocket(const socket_t& fd)
    : was_connected_(true), was_initialized_(true), fd_(fd) {}

TCPSocket::~TCPSocket() {
  Close().Ignore();
}

Status TCPSocket::Init() {
  if (was_initialized_) {
    return StatusFailedPrecondition("Socket has already been initialized");
  }
  
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == kInvalidSocket) {
    return StatusInternalError("Error creating a socket");
  }
  was_initialized_ = true;

  return StatusOk();
}

Status TCPSocket::Connect(const std::string& address, const uint16_t port) {
  if (!was_initialized_) {
    return StatusFailedPrecondition("The socket has not been initialized");
  }
  if (was_connected_) {
    return StatusFailedPrecondition("The socket has been already connected");
  }
  
  address_.sin_family = AF_INET;
  addrinfo* result = nullptr;
  if (getaddrinfo(address.c_str(), std::to_string(port).c_str(), nullptr,
                  &result) != 0) {
    return StatusInvalidArgument("Bad address");
  }
  sockaddr* addr = reinterpret_cast<sockaddr*>(&address_);
  *addr = *result->ai_addr;

  if (connect(fd_, reinterpret_cast<sockaddr*>(&address_),
              sizeof address_) == kSocketError) {
    freeaddrinfo(result);
    return StatusInternalError("Error connecting a socket");
  }
  freeaddrinfo(result);
  was_connected_ = true;

  return StatusOk();
}

Status TCPSocket::SetTimeout(const int seconds) {
  if (!was_connected_) {
    return StatusFailedPrecondition("Socket has not been connected");
  }

  timeval time_out;
  time_out.tv_sec = seconds;
  time_out.tv_usec = 0;

  int res = 0;
  res = setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof time_out);
  if (res == kSocketError) {
    return StatusInternalError("Error setting receive timeout");
  }

  res = setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &time_out, sizeof time_out);
  if (res == kSocketError) {
    return StatusInternalError("Error setting send timeout");
  }

  return StatusOk();
}

Status TCPSocket::Send(const void* message, const size_t length) const {
  if (!was_connected_) {
    return StatusFailedPrecondition("Socket has not been conected");
  }
  
  size_t total = 0;
  for (ssize_t n = 0; total < length; total += n) {
    n = send(fd_, static_cast<const char*>(message) + total, length - total, 0);
    if (n == 0) break;
    if (n == kSocketError) return StatusInternalError("Can not send a message");
  }

  return StatusOk();
}

Status TCPSocket::Puts(const std::string& string) const {
  return Send(string.c_str(), string.size());
}

Status TCPSocket::Receive(const size_t length, void* message) const {
  if (!was_connected_) {
    return StatusFailedPrecondition("Socket has not been conected");
  }
  
  size_t total = 0;
  for (ssize_t n = 0; total < length; total += n) {
    n = recv(fd_, static_cast<char*>(message) + total, length - total, 0);
    
    if (n == 0) break;
    if (n == kSocketError) return StatusInternalError("Can not send a message");
  }
  
  return StatusOk();
}

Status TCPSocket::Gets(const size_t max_size, std::string* message) const {
  if (!was_connected_) {
    return StatusFailedPrecondition("Socket has not been conected");
  }
  
  char buffer[1024];
  size_t to_read = std::min(sizeof buffer, max_size);
  message->clear();
  for (ssize_t n = to_read; static_cast<size_t>(n) == to_read; to_read -= n) {
    n = recv(fd_, buffer, to_read, 0);
    
    if (n == 0) break;
    if (n == kSocketError) return StatusInternalError("Can not send a message");

    message->append(buffer, n);
  }
  
  return StatusOk();
}

Status TCPSocket::Close() {
  if (!was_initialized_) {
    return StatusFailedPrecondition("Socket has not been initialized");
  }

  if (close(fd_) == kSocketError) {
    return StatusInternalError("Error closing a socket");
  }
  was_initialized_ = false;
  was_connected_ = false;

  return StatusOk();
}

}  // namespace rst;

