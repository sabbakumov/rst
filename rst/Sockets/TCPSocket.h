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
