#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <cmath>

#include "include/corvo/response.h"
#include "include/corvo/request.h"
#include "include/corvo/headers.h"
#include "include/global.h"
#include "include/trace.h"

HttpResponse::HttpResponse(HttpRequest *request) {
  this -> request = request;
  this -> setStatus(RES_501);

  sent          = false;
  bodyLength   = 0;
  bodyCapacity = SZ_LINE_BUFFER;
  body         = new char[SZ_LINE_BUFFER];
}

HttpResponse::~HttpResponse() {

}

void
HttpResponse::setStatus(int code, const char* message) {
  statusCode    = code;
  statusMessage = message;
}

void
HttpResponse::setHeader(const char *name, const char *value) {
  /* Make sure that the value actually has content */
  if (value && strlen(value)) {
    if (headers[name]) { headers[name] = strdup(value); }
    else { headers[strdup(name)] = strdup(value); }
  }
  /* If value is null or empty, remove the header */
  else { headers.erase(name); }
}

void
HttpResponse::write(const unsigned char *buffer, size_t count) {
  /* Ensure that body has sufficient capacity */
  if (bodyCapacity - bodyLength < count) {
    int sz = SZ_LINE_BUFFER;
    while (sz <= bodyLength + count) { sz *= 2; }
    body = (char*) realloc(body, sz);
    bodyCapacity = sz;
  }

  /* Copy over the buffer */
  memcpy(body + bodyLength, buffer, count);
  bodyLength += count;
}

void
HttpResponse::write(const char *buffer, size_t count) {
  /* Ensure that body has sufficient capacity */
  if (bodyCapacity - bodyLength < count) {
    int sz = SZ_LINE_BUFFER;
    while (sz <= bodyLength + count) { sz *= 2; }
    body = (char*) realloc(body, sz);
    bodyCapacity = sz;
  }

  /* Copy over the buffer */
  memcpy(body + bodyLength, buffer, count);
  bodyLength += count;
}

void
HttpResponse::write(const char *buffer) {
  write(buffer, strlen(buffer));
}

void
HttpResponse::send() {
  if (sent) { return; }

  int sock = request -> sock;

  /* Write the metadata */
  dprintf(sock, "HTTP/1.1 %d %s\r\n", statusCode, statusMessage);

  /* Print headers */
  for (StrMap::iterator it = headers.begin(); it != headers.end(); ++it) {
    DBG_VERBOSE("%s: %s\n", it -> first, it -> second);
    dprintf(sock, "%s: %s\r\n", it -> first, it -> second);
    DBG_VERBOSE("%s: %s\n", it -> first, it -> second);
  }
  DBG_VERBOSE("SEND RESPONSE\n");

  /* Add Content-Length if necessary */
  if (bodyLength > 0) {
    dprintf(sock, "Content-Length: %d\r\n", bodyLength);
  }

  /* Add the empty line */
  dprintf(sock, "\r\n");

  /* Add body, if appropriate */
  ::write(sock, body, bodyLength);

  /* Shutdown and close the socket */
  shutdown(sock, SHUT_WR);
  sent = true;
  close(sock);
}
