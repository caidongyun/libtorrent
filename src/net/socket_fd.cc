// libTorrent - BitTorrent library
// Copyright (C) 2005, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "torrent/exceptions.h"
#include "socket_address.h"
#include "socket_fd.h"

namespace torrent {

bool
SocketFd::set_nonblock() {
  if (!is_valid())
    throw internal_error("SocketFd::set_nonblock() called on a closed fd");

  return fcntl(m_fd, F_SETFL, O_NONBLOCK) == 0;
}

bool
SocketFd::set_throughput() {
  if (!is_valid())
    throw internal_error("SocketFd::set_throughput() called on a closed fd");

  int opt = IPTOS_THROUGHPUT;

  return setsockopt(m_fd, IPPROTO_IP, IP_TOS, &opt, sizeof(opt)) == 0;
}

int
SocketFd::get_error() const {
  if (!is_valid())
    throw internal_error("SocketFd::get_error() called on a closed fd");

  int err = 0;
  socklen_t length = sizeof(err);

  if (getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &err, &length) == -1)
    throw internal_error("SocketFd::get_error() could not get error");

  return err;
}

bool
SocketFd::open() {
  return (m_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1;
}

bool
SocketFd::bind(const SocketAddress& sa) {
  if (!is_valid())
    throw internal_error("SocketFd::bind(...) called on a closed fd");

  return !::bind(m_fd, &sa.get_addr(), sa.get_sizeof());
}

bool
SocketFd::connect(const SocketAddress& sa) {
  if (!is_valid())
    throw internal_error("SocketFd::connect(...) called on a closed fd");

  return !::connect(m_fd, &sa.get_addr(), sa.get_sizeof()) || errno == EINPROGRESS;
}

bool
SocketFd::listen(int size) {
  if (!is_valid())
    throw internal_error("SocketFd::listen(...) called on a closed fd");

  return !::listen(m_fd, size);
}

SocketFd
SocketFd::accept(SocketAddress& sa) {
  if (!is_valid())
    throw internal_error("SocketFd::accept(...) called on a closed fd");

  socklen_t len = sa.get_sizeof();

  return ::accept(m_fd, &sa.get_addr(), &len);
}

}