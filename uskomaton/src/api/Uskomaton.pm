package Uskomaton;
use strict;
use warnings;
use File::Basename ();
use File::Spec();
our %plugins;

sub register {
	my ($name) = shift @_;
	my ($pkg) = findPackage();
	my $pkgInfo = packageInfo($pkg);
	my $filename = $pkgInfo->{filename};
	

	if (defined $pkgInfo->{loaded}) {
		print "register called more than once $pkgInfo->{filename}";
		return();
	}
	unless($name) {
		$name = "Unknown";
	}
	$pkgInfo->{loaded} = Uskomaton::Internal::register($name, $filename);
}

sub load {
	my $file = shift @_;
	print $file;
	my $pkg = file2Package($file);
	if (exists $plugins{$pkg}) {
		my $info = packageInfo($pkg);
		my $filename = File::Basename::basename($info->{filename});
		print "$filename is already loaded";
		return 2;
	}
	if (open my $handle, $file) {
		my $src = do { local $/; <$handle>};
		close $handle;
		$plugins{$pkg}{filename} = $file;
		my $fullpath = File::Spec->rel2abs($file);
		$src =~ s/^/#line 1 "$fullpath"\n\x7Bpackage $pkg;/;
		# add }
		$src =~ s/\Z/\x7D/;
		_eval($src);
		#if (!exists $plugins{$pkg}{loaded})
		if ($@) {
			#TODO
			die "something went wrong";
			return 1;
		}
	} else {
		print "error opening $file : $!\n";
		return 2;
	}
	return 0;
}

sub hookServer {
	return undef unless @_ >= 2;
	my ($message, $callback) = @_;
	unless (ref $callback) {
		Uskomaton::print("callback isn't reference");
		die "callback isn't reference";
	}
	my $package = findPackage();
	Uskomaton::Internal::hookServer($message, $callback, $package);
}

sub hookOnMessage {
	hookServer('PRIVMSG', shift @_);
}

sub print {
	Uskomaton::Internal::print(shift @_);
}

sub _eval {
	no strict;
	no warnings;
	eval $_[0];
}

sub findPackage {
	my $level = 1;
	while (my ($pkg, $file, $line) = caller($level++)) {
		return $pkg if $pkg =~ /^Uskomaton::Plugin::/;
	}
	die "Unable to determine pkg";
}

sub packageInfo {
	my $pkg = shift @_;
	return $plugins{$pkg};
}

sub file2Package {
	my $str = File::Basename::basename(shift @_);
	$str =~ s/\.pl$//i;
	$str =~ s|([^A-Za-z0-9/])|'_'.unpack("H*",$1)|eg;
	return "Uskomaton::Plugin::$str";
}

1;