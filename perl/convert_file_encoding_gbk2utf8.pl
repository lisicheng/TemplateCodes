#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find;

die "Convert file encoding from gbk to utf8, usage: $0 <path> Ignorefile1, Ignorefile2, ...\n" unless @ARGV >= 1;

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

print "path=$path, \"@IGNORES\" will be ignored\n";

sub print_info {
	print "****************************************************\n";
	print "*****   gbk files size = ".scalar(@gbk_files)." \n";
	print "*****  utf8 files size = ".scalar(@utf8_files)." \n";
	print "***** ascii files size = ".scalar(@ascii_files)." \n";
	print "****************************************************\n";
}

sub route
{
	if (-f $_) {
		$file_full_path_name = $File::Find::name;

		#Remove ignored dirs and files!
		for (my $i = 0; $i < @IGNORES; $i++) {
			if ($file_full_path_name =~ m/$IGNORES[$i]/) {
				return;
			}
			for (my $j = 1; $j < @ARGV; $j++) {
				if ($_ eq $ARGV[$j]) {
					print "**********************Ignore file $_\n";
					return;
				}
			}
		}

		$old_file_enco = lc(`file --mime-encoding $_`);

		#Find files need to be converted!
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

$GBK = lc($GBK);
$UTF8 = lc($UTF8);
$ASCII = lc($ASCII);
find(\&route, $path);

if (@gbk_files > 0) {
	print "--------------gbk files' size=".scalar(@gbk_files)."\n";
	print join("\n", @gbk_files);
	print "\n";
	print "--------------gbk files end\n";
}

if (@unknow_files > 0) {
	print "--------------unknow file encodings' size=".scalar(@unknow_files)."\n";
	print join("\n", @unknow_files);
	print "\n";
	print "--------------unknow file encodings end\n";
}

print_info();

my $convert_count = 0;

foreach (@gbk_files) {
	`iconv -f gbk -t utf8 $_ > $_.utf8`;
	`mv $_.utf8 $_`;
	$convert_count ++;
	print "Convert finished:$_\n";
}

print "Total converted:$convert_count\n";

