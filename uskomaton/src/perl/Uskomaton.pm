package Uskomaton;
use strict;
use warnings;
use File::Basename ();
use File::Spec();
use Symbol();
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

sub hookCommand {
	my ($name, $callback) = @_;
	my $pkg = findPackage();
	my $pkgInfo = packageInfo($pkg);
	unless (defined $pkgInfo->{loaded}) {
		Uskomaton::print("Script isn't registerered!");
		die "Script isn't registerered!";
	}
	my $filename = $pkgInfo->{filename};
	unless (ref $callback) {
		Uskomaton::print("callback isn't reference!");
		die "callback isn't reference!";
	}
	my $hook = Uskomaton::Internal::hookCommand($filename, $name, $pkg, $callback);
	push @{$pkgInfo->{hooks}}, $hook if defined $hook;
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
			$@ =~ s/\(eval \d+\)/$file/g;
			Uskomaton::print("Error loading '$file': \n$@\n");
			Uskomaton::print("Unloading...");
			Uskomaton::unload($plugins{$pkg}{filename});
			return 1;
		}
	} else {
		Uskomaton::print( "error opening $file : $!\n");
		return 2;
	}
	return 0;
}

sub unload {
	my $file = shift @_;
	my $pkg = file2Package($file);
	my $info = packageInfo($pkg);

	if ($info) {
		Uskomaton::print("package found \n");
		if (exists $info->{hooks}) {
			Uskomaton::print("Hooks found..\n");
			for my $hook ( @{$info->{hooks}} ) {
				Uskomaton::print("Unhooking hook $hook");
				Uskomaton::unhook($hook, $pkg);
			}
		}
		Symbol::delete_package($pkg);
		delete $plugins{$pkg};
	} else {
		Uskomaton::print("$file is not loaded\n");
	}
}

sub unhook {
	my $hook = shift @_;
	my $pkg = shift @_;
	($pkg) = caller unless $pkg;
	my $info = packageInfo($pkg);
	if (defined($hook)) {
		return Uskomaton::Internal::unhook($hook);
	}
}

sub hookServer {
	return undef unless @_ >= 2;
	my ($message, $callback) = @_;
	unless (ref $callback) {
		Uskomaton::print("callback isn't reference");
		die "callback isn't reference";
	}
	my $package = findPackage();
	my $info = packageInfo($package);
	my $hook = Uskomaton::Internal::hookServer($info->{filename}, $message, $callback, $package);
	push @{$info->{hooks}}, $hook if defined $hook;
}

sub sendMessage {
	return undef unless @_ >= 3;
	Uskomaton::Internal::sendMessage(shift @_, shift @_, shift @_);
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