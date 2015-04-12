# Uskomaton
### IRC bot with scripting support
Currently supports perl

## C++ API
```cpp
// TODO
```

## Perl API
Every function has 4 arguments
```perl
sub onMessage {
   my $context = shift @_; # == just a string, used to keep track origin of server
   my $channel = shift @_; # == channel where this event/etc was generated
   my $message = shift @_; # == message, differs from use case
   my $sender = shift @_; # == user/nick who sent this event
}
```
### Command hooking
Every script can have "infinite" amount of command hooks. The hooks are treated as commands, for example !test.
```perl
#Uskomaton::hookCommand($name, $callback);
Uskomaton::hookCommand("hello", \&hello);
sub hello {
   my ($context, $channel, $message, $sender) = @_;
   Uskomaton::sendMessage($context, $channel, "hello $sender!");
}
```
### Server hooks
They are used to get messages from server, eg. PRIVMSG. 
```perl
# call onMessage when PRIVMSG arrives from server
Uskomaton::hookOnMessage(\&onMessage); 
#same as Uskomaton::hookServer('PRIVMSG', \&onMessage);
sub onMessage {
  my ($context, $channel, $message, $sender) = @_;
  # do something
}
```

### Message sending
```perl
Uskomaton::sendMessage($context, $channel, $message);
```
