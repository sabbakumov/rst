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

#include "TCPServerSocket.h"

#include <unistd.h>

#include <string>

namespace rst {

TCPServerSocket::TCPServerSocket() : was_initialized_(false), fd_(0) {}

TCPServerSocket::~TCPServerSocket() {
  Close().Ignore();
}

Status TCPServerSocket::Init(const uint16_t port, const int backlog) {
  if (was_initialized_) {
    return StatusFailedPrecondition("A socket has already been initialized");
  }

  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == kInvalidSocket) {
    return StatusInternalError("Error creating a socket");
  }
  was_initialized_ = true;

  int reuse_address = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse_address,
                 sizeof reuse_address) == kSocketError) {
    return StatusInternalError("Error setting a socket for reuse");
  }

  Status status = Bind(port);
  if (!status.ok()) return status;

  return Listen(backlog);
}

Status TCPServerSocket::Bind(const uint16_t port) {
  if (!was_initialized_) {
    return StatusFailedPrecondition("Socket has not been initialized");
  }

  address_.sin_family = AF_INET;
  address_.sin_port = htons(port);
  address_.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(fd_, reinterpret_cast<sockaddr*>(&address_),
           sizeof address_) == kSocketError) {
    return StatusInternalError("Error binding a socket");
  }

  return StatusOk();
}

Status TCPServerSocket::Listen(const int backlog) const {
  if (!was_initialized_) {
    return StatusFailedPrecondition("Socket has not been initialized");
  }

  if (listen(fd_, backlog) == kSocketError) {
    return StatusInternalError("Error changing backlog for a socket");
  }

  return StatusOk();
}

std::unique_ptr<TCPSocket> TCPServerSocket::Accept(
    char* /*address*/, Status* status) const {
  if (!was_initialized_) {
    *status = StatusFailedPrecondition("Socket has not been initialized");
    return nullptr;
  }

  sockaddr_in incoming;
  socklen_t len = sizeof incoming;
  socket_t fd = accept(fd_, reinterpret_cast<sockaddr*>(&incoming), &len);
  if (fd == kInvalidSocket) {
    *status = StatusInternalError("Error accepting a socket");
    return nullptr;
  }

  return std::unique_ptr<TCPSocket>(new TCPSocket(fd));
}

Status TCPServerSocket::Close() {
  if (!was_initialized_) {
    return StatusFailedPrecondition("Socket has not been initialized");
  }
  
  if (close(fd_) == kSocketError) {
    return StatusInternalError("Error closing a socket");
  }
  was_initialized_ = false;
  
  return StatusOk();
}

}  // namespace rst

