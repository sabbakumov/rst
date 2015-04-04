#ifndef RST_SOCKETS_TCPSERVERSOCKET_H_
#define RST_SOCKETS_TCPSERVERSOCKET_H_

#include <netinet/in.h>

#include <cstdint>
#include <memory>

#include "rst/Status/Status.h"
#include "TCPSocket.h"

namespace rst {

class TCPServerSocket {
 public:
  TCPServerSocket();
  
  ~TCPServerSocket();
  
  Status Init(const uint16_t port, const int backlog);

  std::unique_ptr<TCPSocket> Accept(char* address, Status* status) const;
  
  Status Close();

 private:
  TCPServerSocket(const TCPServerSocket&) = delete;
  TCPServerSocket(TCPServerSocket&&) = delete;
  
  TCPServerSocket& operator=(const TCPServerSocket&) = delete;
  TCPServerSocket& operator=(TCPServerSocket&&) = delete;

  Status Bind(const uint16_t port);
  
  Status Listen(const int backlog) const;
  
  bool was_initialized_;
  socket_t fd_;
  sockaddr_in address_;
};

}  // namespace rst

#endif  // RST_SOCKETS_TCPSERVERSOCKET_H_
