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

