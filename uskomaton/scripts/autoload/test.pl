use Uskomaton;

Uskomaton::register("mita", "helevettia");
Uskomaton::hookOnMessage(\&test2);
sub test2 {
	my ($ctx, $channel) = @_;
	Uskomaton::print("from callback");
	Uskomaton::sendMessage($ctx, $channel, "aamuja :D");
}