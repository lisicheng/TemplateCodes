#!/usr/bin/perl -w
#
use strict;
use warnings;

use File::Find;

die "Usage: $0 <dir> <extention>\n" unless @ARGV == 2;

my $Dir = $ARGV[0];
my $Ext = $ARGV[1];

print "Dir=$Dir, Ext=$Ext\n";

