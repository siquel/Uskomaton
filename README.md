# Uskomaton
### IRC bot with scripting support
Currently supports perl

## C++ API
```cpp
// TODO
```

## Perl API
```perl
# helloworld.pl
# call onMessage when PRIVMSG arrives from server
Uskomaton::hookOnMessage(\&onMessage); # same as Uskomaton::hookServer('PRIVMSG', \&onMessage);
sub onMessage {
  my ($context, $channel, $message, $sender) = @_;
  # do something
}
```
