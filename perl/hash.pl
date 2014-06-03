#!/usr/bin/perl -w

use strict;
use warnings;

my %my_hash = ("key", "value");

print "my_hash is ".$my_hash{"key"}."\n";

if (exists $my_hash{"key2"}) {
	print "key exists\n";
}
else  {
	print "key not exists\n";
}
