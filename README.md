# janet-flock

Simple file locking with flock for janet.

## Quick example
```
(import flock)

(def l1 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :shared))
(def l2 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :shared))
(var l3 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :exclusive))
(unless l1 (error "expected lock success"))
(unless l2 (error "expected lock success"))
(when l3 (error "expected lock failure"))

(with [l (flock/acquire ...)]
  ...)
```

## API

[here](API.md)