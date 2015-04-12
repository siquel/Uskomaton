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
		if ($imdb->status) {
			my ($rating, $vnum, $awards) = $imdb->rating();
			my $genres = join(',', @{$imdb->genres()});
			my $output = "[ ${\$imdb->title()} || ${\$imdb->year()} ] [ ${\$imdb->kind()} ] [ $genres ] [ Runtime ${\$imdb->duration()} ] [ Rating $rating || $vnum votes ]";
			Uskomaton::sendMessage($ctx, $channel, $output);
		} else {
			Uskomaton::sendMessage($ctx, $channel, "Error: $imdb->error");
		}
	}
}

sub imdbCommand {
	my ($ctx, $channel, $message, $sender) = @_;
	Uskomaton::sendMessage($ctx, $channel, "Not implemented");
}