#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find;

die "Convert file encoding from gbk to utf8, usage: $0 <path>\n" unless @ARGV == 1;

my $GBK = "ISO-8859";
my $UTF8 = "UTF-8";
my $ASCII = "ASCII";

my @IGNORES = (".svn", ".git");

my $path = $ARGV[0];
my $file_full_path_name = "";
my $old_file_enco = "";
my @gbk_files;
my @utf8_files;
my @ascii_files;
my @unknow_files;

print "Perl script starting...\n";
print "path=$path, \"@IGNORES\" will be ignored\n";

sub route
{
	if (-f $_) {
		$file_full_path_name = $File::Find::name;
		for (my $i = 0; $i < @IGNORES; $i++) {
			if ($file_full_path_name =~ m/$IGNORES[$i]/) {
				return;
			}
		}
		$old_file_enco = `file $_`;

		if ($old_file_enco =~ m/$GBK/) {
			push(@gbk_files, $file_full_path_name);
		}
		elsif ($old_file_enco =~ m/$UTF8/) {
			push(@utf8_files, $file_full_path_name);
		}
		elsif ($old_file_enco =~ m/$ASCII/) {
			push(@ascii_files, $file_full_path_name);
		}
		else {
			push(@unknow_files, $file_full_path_name);
		}
	}
}

find(\&route, $path);

if (@gbk_files > 0) {
	print "--------------gbk files' size=".scalar(@gbk_files)."\n";
	print join("\n", @gbk_files);
	print "\n";
	print "--------------gbk files end\n";
}
else {
	print "No gbk files found!\n";
}

if (@unknow_files > 0) {
	print "--------------unknow file encodings' size=".scalar(@unknow_files)."\n";
	print join("\n", @unknow_files);
	print "\n";
	print "--------------unknow file encodings end\n";
}
else {
	print "all files gbk, utf8, ascii!\n";
}

my $convert_count = 0;

foreach (@gbk_files) {
	`iconv -f gbk -t utf8 $_ > $_.utf8`;
	`mv $_.utf8 $_`;
	$convert_count ++;
	print "Convert finished:$_\n";
}

print "Total convert:$convert_count\n";

