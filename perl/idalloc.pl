#!/usr/bin/perl -w

use strict;
use warnings;

my $dir="";
my $ip="";
my $port="";
my %ip_hash;

sub log_debug {
	print "debug: ".$_[0];
}
sub log_debug_n {
	print "debug: ".$_[0]."\n";
}
sub log_warning {
	print "warning: ".$_[0];
}
sub log_warning_n {
	print "warning: ".$_[0]."\n";
}

#check parameters passed by.
if (@ARGV < 3) {
	&log_warning_n("params number passed by incorrect");
	exit(1);
}
$dir=$ARGV[0];
$ip=$ARGV[1];
$port=$ARGV[2];

&log_debug_n("dir=$dir");
&log_debug_n("ip=$ip");
&log_debug_n("port=$port");

chdir($dir);
open(SEC, "<$dir/conf/idsection_list.conf") or die "can not open $dir/conf/idsection_list.conf\n";
my $line;
$line=<SEC>;
&log_debug("get:$line");
chomp($line);
if ($line eq "[idsection_list]") {
	&log_debug("the first line: $line");
}
else {
	&log_warning_n("should be [idsection_list] line, but: $line");
	exit(1);
}
while($line = <SEC>) {
	my $l_ip;
	my $l_port;
	{
		if ($line =~ /^\[\.\@idsection\]/) {
			&log_debug("the @ line : $line");
		}
		else {
			&log_warning_n("should be @ line, but: $line");
			exit(1);
		}
	}
	if ($line=<SEC>) {
		if ($line =~ /^name : idsection/) {
			&log_debug("the name line: $line");
		}
		else {
			&log_warning_n("should be name line, but: $line");
			exit(1);
		}
	}
	if ($line = <SEC>) {
		if ($line =~ /^ip : ([0-9.]+)/) {
			$l_ip = $1;
			&log_debug_n("ip: $l_ip");
		}
		else {
			&log_warning_n("should be ip line, but: $line");
			exit(1);
		}
	}
	if ($line = <SEC>) {
		if ($line =~ /^port : ([0-9]+)/) {
			$l_port = $1;
			&log_debug_n("port: $l_port");
		}
		else {
			&log_warning_n("should be port line, but: $line");
			exit(1);
		}
	}
	$ip_hash{$l_ip}=$l_port;
	&log_debug_n("------------------------------------");
	&log_debug_n("add ip{$l_ip}, port{$l_port}");
}
close(SEC);