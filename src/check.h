#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fatal("check failed: %s", #cond);                                        \
    }                                                                          \
  } while (0)
