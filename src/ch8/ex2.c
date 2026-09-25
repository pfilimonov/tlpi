/* Implement getpwnam() using setpwent(), getpwent(), and endpwent(). */

#include <stdbool.h>
#define _XOPEN_SOURCE 500 /* Get crypt() declaration from <unistd.h> */
#include "tlpi_hdr.h"
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <unistd.h>

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fatal("check failed: %s", #cond);                                        \
    }                                                                          \
  } while (0)

struct passwd *getpwnam_(const char *name);

struct passwd *getpwnam_(const char *name) {
  struct passwd *pwd;

  setpwent();
  while ((pwd = getpwent()) != NULL) {
    if (strcmp(pwd->pw_name, name) == 0) {
      setpwent();
      return pwd;
    }
  }

  endpwent();
  return NULL;
}

int main(void) {
  // check my user
  CHECK(getpwnam_("peter") != NULL);
  struct passwd *real_struct_ptr = getpwnam("peter");
  struct passwd real_struct;
  real_struct.pw_passwd = real_struct_ptr->pw_passwd;
  real_struct.pw_name = real_struct_ptr->pw_name;
  real_struct.pw_uid = real_struct_ptr->pw_uid;
  real_struct.pw_gid = real_struct_ptr->pw_gid;
  real_struct.pw_gecos = real_struct_ptr->pw_gecos;
  real_struct.pw_dir = real_struct_ptr->pw_dir;
  real_struct.pw_shell = real_struct_ptr->pw_shell;

  real_struct_ptr = getpwnam_("peter");
  CHECK(strcmp(real_struct_ptr->pw_passwd, real_struct.pw_passwd) == 0);
  CHECK(strcmp(real_struct_ptr->pw_name, real_struct.pw_name) == 0);
  CHECK(real_struct_ptr->pw_uid == real_struct.pw_uid);
  CHECK(real_struct_ptr->pw_gid == real_struct.pw_gid);
  CHECK(strcmp(real_struct_ptr->pw_gecos, real_struct.pw_gecos) == 0);
  CHECK(strcmp(real_struct_ptr->pw_dir, real_struct.pw_dir) == 0);
  CHECK(strcmp(real_struct_ptr->pw_shell, real_struct.pw_shell) == 0);

  // check non existing user
  CHECK(getpwnam_("non existing user") == NULL);
  return EXIT_SUCCESS;
}
