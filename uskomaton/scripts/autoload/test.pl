use Uskomaton;

Uskomaton::register("mita", "helevettia");
Uskomaton::hookOnMessage(\&test2);
Uskomaton::hookCommand("test", \&testCommand);
sub test2 {
	my ($ctx, $channel, $message, $sender) = @_;
	Uskomaton::print("from callback");
	#Uskomaton::sendMessage($ctx, $channel, "$message wii $sender");
}

sub testCommand {
	my ($ctx, $channel, $message, $sender) = @_;
	Uskomaton::sendMessage($ctx, $channel, "from test.pl @ testCommand");
}
