/*
 * Implement setenv() and unsetenv() using getenv(), putenv(), and, where
 * necessary, code that directly modifies environ. Your version of unsetenv()
 * should check to see whether there are multiple definitions of an environment
 * variable, and remove them all (which is what the glibc version of unsetenv()
 * does).
 */

#include "tlpi_hdr.h"

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fatal("check failed: %s", #cond);                                        \
    }                                                                          \
  } while (0)

extern char **environ;

int setenv_(const char *name, const char *value, int overwrite);
int unsetenv_(const char *name);

void test(void);

int main(void) {
  test();
  exit(EXIT_SUCCESS);
}

int setenv_(const char *name, const char *value, int overwrite) {
  if (name == NULL || strlen(name) == 0 || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }

  char *cur_var = getenv(name);
  if (cur_var != NULL && overwrite == 0) {
    return 0;
  }

  char *buf = malloc(strlen(name) + strlen(value) + 1 + 1);
  sprintf(buf, "%s=%s", name, value);
  if (putenv(buf) != 0) {
    return -1;
  }

  return 0;
}

static void free_env_(char **env) {
  if (env == NULL) {
    return;
  }

  for (char **ptr = env; *ptr != NULL; ptr++) {
    free(*ptr);
  }

  free(env);
}

int unsetenv_(const char *name) {
  static char **envptr = NULL;

  if (name == NULL || strlen(name) == 0 || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }

  if (environ == NULL) {
    return 0;
  }
  if (getenv(name) == NULL) {
    return 0;
  }
  size_t env_size = 0;
  char **new_env = malloc(sizeof(char *));
  if (new_env == NULL) {
    return -1;
  }
  for (char **ptr = environ; *ptr != NULL; ptr++) {
    if (strncmp(*ptr, name, strlen(name)) == 0 && strlen(name) < strlen(*ptr) &&
        (*ptr)[strlen(name)] == '=') {
      continue;
    }

    new_env[env_size] = malloc(strlen(*ptr) + 1);
    if (new_env[env_size] == NULL) {
      free_env_(new_env);
      return -1;
    }
    strcpy(new_env[env_size], *ptr);
    char **new_env_tmp;
    if ((new_env_tmp = realloc(new_env, (++env_size + 1) * sizeof(char *))) ==
        NULL) {
      free(new_env[env_size - 1]);
      new_env[env_size - 1] = NULL;
      free_env_(new_env);
      return -1;
    }
    new_env = new_env_tmp;
  }
  new_env[env_size] = NULL;

  if (envptr != NULL) {
    free_env_(envptr);
    envptr = NULL;
  }
  environ = new_env;
  envptr = new_env;
  return 0;
}

void test(void) {
  char *res;

  // single var set
  if (setenv_("TEST", "test", 0) == -1) {
    errExit("setenv");
  }
  res = getenv("TEST");
  CHECK(strcmp(res, "test") == 0);

  // try to change var without overwrite flag
  if (setenv_("TEST", "test1", 0) == -1) {
    errExit("setenv");
  }
  res = getenv("TEST");
  CHECK(strcmp(res, "test") == 0);

  // change var with overwrite flag
  // try to change var without overwrite flag
  if (setenv_("TEST", "test1", 1) == -1) {
    errExit("setenv");
  }
  res = getenv("TEST");
  CHECK(strcmp(res, "test1") == 0);

  // unset var
  if (unsetenv_("TEST") == -1) {
    errExit("unsetenv");
  }
  res = getenv("TEST");
  CHECK(res == NULL);

  // unset non-existing var
  CHECK(unsetenv_("TEST1") == 0);

  // unset multiple
  char *test_env[] = {"TEST=test", "TEST=test", "TEST=test", NULL};
  environ = test_env;
  if (unsetenv_("TEST") == -1) {
    errExit("unsetenv");
  }
  res = getenv("TEST");
  CHECK(res == NULL);
}
