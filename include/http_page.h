/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HTTP_PAGE_H
#define __HTTP_PAGE_H

#include <stdio.h>

uint8_t name[] = {"index.html"};

uint8_t content[] = {
"<html>"
  "<head>"
    "<title>Hello World</title>"
  "</head>"
  "<body>"
    "<h1>Hello, World!</h1>"
  "</body>"
"</html>"
};

#endif
