#include <errno.h>
#include <fcntl.h>
#include <janet.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  int fd;
} Flock;

static void _flock_close(Flock *l) {
  if (l->fd != -1) {
    int err;
    do {
      err = close(l->fd);
    } while (err < 0 && errno != EINTR);
    if (err != 0) {
      /* Can't do much... */;
    }
    l->fd = -1;
  }
}

static int flock_gc(void *p, size_t s) {
  (void)s;
  Flock *l = (Flock *)p;
  _flock_close(l);
  return 0;
}

static Janet flock_release(int32_t argc, Janet *argv);

static JanetMethod flock_methods[] = {
    {"close", flock_release}, /* So flocks can be used with 'with' */
    {NULL, NULL}};

static int flock_get(void *ptr, Janet key, Janet *out) {
  (void)ptr;
  return janet_getmethod(janet_unwrap_keyword(key), flock_methods, out);
}

static const JanetAbstractType flock_type = {
    "flock.flock", flock_gc, NULL, flock_get, NULL,
    NULL,          NULL,     NULL, NULL,      NULL};

static Janet flock_new(int32_t argc, Janet *argv) {
  (void)argv;
  janet_fixarity(argc, 0);
  Flock *l;
  l = (Flock *)janet_abstract(&flock_type, sizeof(Flock));
  l->fd = -1;
  return janet_wrap_abstract(l);
}

static Janet flock_acquire(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);

  const char *lpath = janet_getcstring(argv, 0);
  int flock_operation = 0;

  if (janet_keyeq(argv[1], "block")) {
    /* nothing to do. */
  } else if (janet_keyeq(argv[1], "noblock")) {
    flock_operation |= LOCK_NB;
  } else {
    janet_panic("slot #1 must be :block|:noblock");
  }

  if (janet_keyeq(argv[2], "shared")) {
    flock_operation |= LOCK_SH;
  } else if (janet_keyeq(argv[2], "exclusive")) {
    flock_operation |= LOCK_EX;
  } else {
    janet_panic("slot #2 must be :shared|:exclusive");
  }

  Flock *l;
  if (argc == 4) {
    l = (Flock *)janet_getabstract(argv, 3, &flock_type);
  } else {
    l = (Flock *)janet_abstract(&flock_type, sizeof(Flock));
    l->fd = -1;
  }

  do {
    l->fd = open(lpath, /* XXX we want this O_CLOEXEC | */ O_CREAT, S_IRWXU);
  } while (l->fd == -1 && errno == EINTR);
  if (l->fd == -1)
    janet_panicf("unable to open lock file '%s' - %s", lpath, strerror(errno));

  int err;
  do {
    err = flock(l->fd, flock_operation);
  } while (err == -1 && errno == EINTR);
  if (err < 0) {
    _flock_close(l);
    if ((flock_operation & LOCK_NB) && errno == EWOULDBLOCK) {
      return janet_wrap_nil();
    }
    janet_panicf("unable to acquire lock on file '%s' - %s", lpath,
                 strerror(errno));
  }

  return janet_wrap_abstract(l);
}

static Janet flock_release(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);
  Flock *l = (Flock *)janet_getabstract(argv, 0, &flock_type);
  _flock_close(l);
  return janet_wrap_nil();
}

static const JanetReg cfuns[] = {
    {"new", flock_new,
     "(flock/new)\n\n"
     "Create a new flock that is not locked or associated with any file."},
    {"acquire", flock_acquire,
     "(flock/acquire path wait-mode mode &opt flock)\n\n"
     "Open (potentially creating the file) an flock at path,"
     "with waitmode :block|:noblock and mode :shared|:exclusive."
     "returns the lock, if the lock is non blocking, returns nil is the"
     "lock was already held in an incompatible way."},
    {"release", flock_release,
     "(flock/release l)\n\n"
     "release file lock."},
    {NULL, NULL, NULL}};

JANET_MODULE_ENTRY(JanetTable *env) { janet_cfuns(env, "redis", cfuns); }
