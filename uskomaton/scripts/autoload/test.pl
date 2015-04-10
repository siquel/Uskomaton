use Uskomaton;

Uskomaton::register("mita", "helevettia");
Uskomaton::hookOnMessage(\&test2);
sub test2 {
	my ($ctx, $channel, $message, $sender) = @_;
	Uskomaton::print("from callback");
	Uskomaton::sendMessage($ctx, $channel, "$message wii $sender");
}
