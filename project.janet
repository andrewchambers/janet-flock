(declare-project
  :name "flock"
  :author "Andrew Chambers"
  :license "MIT"
  :url "https://github.com/andrewchambers/janet-flock"
  :repo "git+https://github.com/andrewchambers/janet-flock.git")

(declare-native
    :name "flock"
    :source ["flock.c"])
