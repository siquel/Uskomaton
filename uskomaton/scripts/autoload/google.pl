use Mojo::DOM;
require URI::Encode;
require LWP::UserAgent;
Uskomaton::register("google");
Uskomaton::hookCommand("google", \&onGoogle);

my $uri = URI::Encode->new( { encode_reserved => 0 } );
my $agent = LWP::UserAgent->new(
	agent => "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:37.0) Gecko/20100101 Firefox/37.0"
	);

sub onGoogle {
	my ($ctx, $channel, $message, $sender) = @_;
	my $query = $uri->encode($message);
	my $url = 'http://www.google.com/search?ie=utf-8&oe=utf-8&q='.$query;
	my $response = $agent->get($url);
	return  if (!$response->is_success);
	my $xml = $response->decoded_content;
	my $dom = Mojo::DOM->new($xml);
	my $search = $dom->find('div#search')->[0];
	for my $e ($search->find('div.srg > li.g')->each ) {
		my $url = $e->find("a")->[0]->attr("href");
	    my $title = $e->find("h3.r > a")->[0]->text;
		Uskomaton::sendMessage($ctx, $channel, "$title <-> $url");
		#todo maybe return list?
		return;
	}
}
