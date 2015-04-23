#ifndef BACKTRACE_H
#define BACKTRACE_H

#ifdef USE_BACKTRACE
#ifdef __APPLE_

class BackTrace {
 public:
  explicit BackTrace();
  virtual ~BackTrace();
};

static BackTrace landmine;

#else

#endif
#endif
#endif