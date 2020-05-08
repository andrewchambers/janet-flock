# flock

## <function mod>/acquire
type: cfunction
[/flock.c#L57](/flock.c#L57)
```
    (flock/acquire path wait-mode mode &opt flock)
    
    Open (potentially creating the file) an flock at path,with waitmode :block|:noblock and
    mode :shared|:exclusive. Returns the lock, if the lock is non blocking, returns nil is
    thelock was already held in an incompatible way.
```

## <function mod>/locked?
type: cfunction
[/flock.c#L111](/flock.c#L111)
```
    (flock/locked? l)
    
    Check if the lock object has aquired a lock.
```

## <function mod>/new
type: cfunction
[/flock.c#L48](/flock.c#L48)
```
    (flock/new)
    
    Create a new flock that is not locked or associated with any file.
```

## <function mod>/release
type: cfunction
[/flock.c#L117](/flock.c#L117)
```
    (flock/release l)
    
    Release file lock.
```

