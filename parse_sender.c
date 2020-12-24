#include "/opt/pkg/include/bglibs/sysdeps.h"
#include <stdio.h>

int exitcode = 0;
#define voidexit(n) { exitcode = n; return; }
#define intexit(n) { exitcode = n; return 55; }

#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define QQ_OOM 51
#define QQ_WRITE_ERROR 53
#define QQ_INTERNAL 81
#define QQ_BAD_ENV 91

#define QQ_DROP_MSG 99

/* a replacement for setenv(3) for systems that don't have one */
void mysetenv(const char* key, const char* val, size_t vallen)
{
  char* tmp;
  size_t keylen;
  
  keylen = strlen(key);
  tmp = malloc(keylen + 1 + vallen + 1);
  memcpy(tmp, key, keylen);
  tmp[keylen] = '=';
  memcpy(tmp+keylen+1, val, vallen);
  tmp[keylen+1+vallen] = 0;
  if (putenv(tmp) != 0)
    voidexit(QQ_OOM);
}

void mysetenvu(const char* key, unsigned long val)
{
  char buf[40];
  int i;
  i = sizeof buf;
  buf[--i] = 0;
  do {
    buf[--i] = (val % 10) + '0';
    val /= 10;
  } while (val > 0);
  mysetenv(key, buf + i, sizeof buf - 1 - i);
}

/* Parse the sender address into user and host portions */
size_t parse_sender(const char* env)
{
  const char* ptr = env;
  char* at;
  size_t len;

  if (!env)
    intexit(QQ_BAD_ENV);
  //len = strlen(env);
  
  // sure seems like this is failing
  if(*ptr != 'F')
    intexit(QQ_BAD_ENV);
  ++ptr;
  
  unsetenv("QMAILNAME");
  
  if(!*ptr) {
    if (putenv("QMAILUSER=") != 0
	|| putenv("QMAILHOST=") != 0)
      intexit(QQ_OOM);
    return 2;
  }

  at = strrchr(ptr, '@');
  if(!at) {
    len = strlen(ptr);
    mysetenv("QMAILUSER", ptr, len);
    putenv("QMAILHOST=");
  }
  else {
    len = strlen(at);
    mysetenv("QMAILUSER", ptr, at-ptr);
    mysetenv("QMAILHOST", at+1, len-1);
    ptr = at;
  }
  return ptr + len + 1 - env;
}
