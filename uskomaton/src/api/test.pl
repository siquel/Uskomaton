use Uskomaton;

Uskomaton::register("mita", "helevettia");
Uskomaton::hookOnMessage(\&test2);

sub test2 {
	Uskomaton::print("from callback");
	Uskomaton::sendMessage("#dev", "aamuja :D");
}