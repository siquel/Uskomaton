use IMDB::Film;
Uskomaton::register("imdb");
Uskomaton::hookOnMessage(\&onMessage);
Uskomaton::hookCommand("imdb", \&imdbCommand);
sub onMessage {
	my ($ctx, $channel, $message, $sender) = @_;
	my @ids = $message =~ m!imdb.com/title/tt(\d+)!ig;
	foreach (@ids) {
		my $imdb = new IMDB::Film(
			crit => $_,
			timeout => 5);
		Uskomaton::sendMessage($ctx, $channel, getOuput($imdb));
	}
}

sub imdbCommand {
	my ($ctx, $channel, $message, $sender) = @_;
	my $imdb = new IMDB::Film(crit => $message, timeout => 5);
	Uskomaton::sendMessage($ctx, $channel, getOuput($imdb));
}

sub getOuput {
	my $imdb = shift @_;
	if ($imdb->status) {
			my ($rating, $vnum, $awards) = $imdb->rating();
			my $genres = join(',', @{$imdb->genres()});
			my $output = "[ ${\$imdb->title()} || ${\$imdb->year()} ] [ ${\$imdb->kind()} ] [ $genres ] [ Runtime ${\$imdb->duration()} ] [ Rating $rating || $vnum votes ]";
			return $output;
	} else {
		return "Error: " . $imdb->error;
	}
}