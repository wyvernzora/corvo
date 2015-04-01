#include "src/error.h"

#include "include/transpose/buffer.h"
#include "include/transpose/fragment.h"
#include "include/global.h"
#include "include/trace.h"

#include "embed/error.html.h"
#include "embed/styles.css.h"
#include "embed/pure.css.h"

void error(HttpRequest *request, HttpResponse *response) {
  DBG_ERR("%d %s\n", response -> statusCode, response -> statusMessage);

  char code[4];
  snprintf(code, 4, "%d", response -> statusCode);

  Fragment *fragment = new Fragment(error_html, error_html_len);
  fragment -> set("status-code", code);
  fragment -> set("status-message", response -> statusMessage);
  fragment -> set("css-pure", new Fragment(pure_css, pure_css_len));
  fragment -> set("css-style", new Fragment(styles_css, styles_css_len));

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
