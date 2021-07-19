#include "stdint.h"

#include "stdlib.h"

typedef struct _7 _7;
typedef struct _8 _8;
typedef struct _2 _2;
int main(_7 __1);
_8 _9(int __3);
void _A(_8 * this, float __4);
int _1;
typedef struct _7 {
  uint8_t * buf;
  int len;
  int cap;
}
_7;
typedef struct _8 {
  float * buf;
  int len;
  int cap;
}
_8;
typedef struct _2 {
  int t;
  union {
    struct {
      int a;
    }
    Opt2;
    struct {
      int a;
      int b;
    }
    Opt3;
  }
  u;
}
_2;
_8 _9(int __3) {
  return (_8) {
    .buf = malloc(sizeof(float) * __3), .len = 0, .cap = __3,
  };
};
void _A(_8 * this, float __4) {
  ((( * this).buf)[(( * this).len)]) = __4;
  (( * this).len) += 1;
  if ((( * this).len) == (( * this).cap)) {
    (( * this).cap) *= 2;
    (( * this).buf) = realloc(( * this).buf, sizeof(float) * (( * this).cap));
  }
};
int main(_7 __1) {
  _8 __2 = _9(2);
  _A(( & __2), '1');
  _A(( & __2), '2');
  _A(( & __2), '3');
  _2 __5 = (_2) {
    .t = 0
  };
  _2 __6 = (_2) {
    .t = 1, .u = {
      .Opt2 = {
        .a = 1,
      }
    }
  };
  _2 __7 = (_2) {
    .t = 2, .u = {
      .Opt3 = {
        .a = 1,
        .b = 1,
      }
    }
  };
};