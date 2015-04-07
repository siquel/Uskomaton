use Uskomaton;

Uskomaton::register("mita", "helevettia");
Uskomaton::print( "testing 123");
Uskomaton::hookServer('PRIVMSG', \&test2);
Uskomaton::hookOnMessage(\&test2);

sub test2 {
	Uskomaton::print("from callback");
	Uskomaton::sendMessage("#test", "aamuja :D");
}