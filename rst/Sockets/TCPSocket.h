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

#ifndef RST_SOCKETS_TCPSOCKET_H_
#define RST_SOCKETS_TCPSOCKET_H_

#include <netinet/in.h>

#include <cstdint>
#include <string>

#include "rst/Status/Status.h"

namespace rst {

enum {
  kSocketError = -1,
  kInvalidSocket = -1
};

typedef int socket_t;

class TCPSocket {
 public:
  TCPSocket();
  
  explicit TCPSocket(const socket_t& fd);
  
  ~TCPSocket();

  Status Init();
  
  Status Connect(const std::string& address, const uint16_t port);

  Status SetTimeout(const int seconds);

  Status Send(const void* message, const size_t length) const;
  Status Puts(const std::string& string) const;

  Status Receive(const size_t length, void* message) const;
  Status Gets(const size_t max_size, std::string* string) const;

  Status Close();

 private:
  TCPSocket(const TCPSocket&) = delete;
  TCPSocket(TCPSocket&&) = delete;
  
  TCPSocket& operator=(const TCPSocket&) = delete;
  TCPSocket& operator=(TCPSocket&&) = delete;
  
  bool was_connected_;
  bool was_initialized_;
  socket_t fd_;
  sockaddr_in address_;
};

}  // namespace rst

#endif  // RST_SOCKETS_TCPSOCKET_H_
