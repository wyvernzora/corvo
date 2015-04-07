#include "error.h"

#include "transpose/buffer.h"
#include "transpose/fragment.h"
#include "global.h"
#include "trace.h"
#include "corvo/handler.h"

#include "embed/error.html.h"
#include "embed/styles.css.h"
#include "embed/bootstrap.css.h"

ErrorHandler::ErrorHandler() {
  statusCode = -1;
}

ErrorHandler::ErrorHandler(int code, const char *message) {
  statusCode    = code;
  statusMessage = message;
}

void
ErrorHandler::handle(HttpRequest *request, HttpResponse *response) {
  DBG_ERR("%d %s\n", response -> statusCode, response -> statusMessage);

  if (statusCode > 0) { response -> setStatus(statusCode, statusMessage); }

  char code[4];
  snprintf(code, 4, "%d", response -> statusCode);

  Fragment *fragment = new Fragment(error_html);
  fragment -> set("status-code", code);
  fragment -> set("status-message", response -> statusMessage);
  fragment -> set("css-bootstrap", bootstrap_css);
  fragment -> set("css-style", styles_css);

  switch (response -> statusCode) {
    case 404:
      fragment -> set("status-description", ERR_404);
      break;
    case 405:
      fragment -> set("status-description", ERR_405);
      break;
    default:
      fragment -> set("status-description", ERR_DEFAULT);
      break;
  }

  Buffer *buffer = fragment -> render();

  response -> write(buffer -> data(), buffer -> length());
  delete buffer;
  delete fragment;
}
