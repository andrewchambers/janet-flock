(import build/flock :as flock)

(def l1 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :shared))
(def l2 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :shared))
(var l3 (flock/acquire "/tmp/janet-flock-test.lock" :noblock :exclusive))
(unless l1 (error "expected lock success"))
(unless l2 (error "expected lock success"))
(when l3 (error "expected lock failure"))

(set l3 (flock/new))
(when (flock/locked? l3)
  (error "expected not locked"))

(flock/release l2)

(when (flock/acquire "/tmp/janet-flock-test.lock" :noblock :exclusive l3)
  (error "expected lock failure"))

(:close l1)

(unless (flock/acquire "/tmp/janet-flock-test.lock" :noblock :exclusive l3)
  (error "expected lock success"))

(unless (flock/locked? l3)
  (error "expected locked"))

(:close l3)

(unless (flock/acquire "/tmp/janet-flock-test.lock" :noblock :shared)
  (error "expected lock success"))