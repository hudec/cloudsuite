/*
    httperf -- a tool for measuring web server performance
    Copyright (C) 2000  Hewlett-Packard Company
    Contributed by Martin Arlitt <arlitt@hpl.hp.com>

    This file is part of httperf, a web server performance measurment
    tool.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

/* This code is based on the generic writev() implementation of GNU
   libc.  It implements writev() simply in terms of write().  No
   atomicity guarantees and performance isn't great either, but its
   good enough for SSL purposes.  */

#include "config.h"

#ifdef HAVE_OPENSSL_SSL_H

#ifdef __FreeBSD__
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#include <string.h>

#include <sys/types.h>
#include <sys/uio.h>

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

ssize_t
SSL_writev (SSL *ssl, const struct iovec *vector, int count)
{
  char *buffer;
  register char *bp;
  size_t bytes, to_copy;
  int i;

  /* Find the total number of bytes to be written.  */
  bytes = 0;
  for (i = 0; i < count; ++i)
    bytes += vector[i].iov_len;

  /* Allocate a temporary buffer to hold the data.  */
  buffer = (char *) alloca (bytes);

  /* Copy the data into BUFFER.  */
  to_copy = bytes;
  bp = buffer;
  for (i = 0; i < count; ++i)
    {
#     define min(a, b)		((a) > (b) ? (b) : (a))
      size_t copy = min (vector[i].iov_len, to_copy);

      memcpy ((void *) bp, (void *) vector[i].iov_base, copy);
      bp += copy;

      to_copy -= copy;
      if (to_copy == 0)
        break;
    }

  return SSL_write (ssl, buffer, bytes);
}

#endif /* HAVE_OPENSSL_SSL_H */
