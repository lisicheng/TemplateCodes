#!/usr/bin/perl

my $TEMPLATE_PATH = '/home/arch/se/se-tmplate';
my $SE_PATH = '/home/arch/se';

my $pid = 'se-lisicheng';
my $pid_short = 'lisicheng';
my $port_commit = 2654;
my $port_query = 2655;

#read pid form applicant/pid
chdir($TEMPLATE_PATH);
open(PID, "<applicant/pid") or die "can't open applicant/pid file!\n";
@pid_lines = <PID>;
$pid = $pid_lines[0];
chomp($pid);
close(PID);
if ($pid =~ /se-/) {
	$pid_short = substr($pid, 3);
	print "pid provided already start with 'se-'\n";
}
else {
	$pid_short = $pid;
	$pid = "se-$pid";
}
print "get pid[$pid] from applicant/pid file!\n";

#read port
open(PORT, "<applicant/port.commit") or die "can't open applicant/port.commit!\n";
@port_lines = <PORT>;
$port_commit = $port_lines[0];
chomp($port_commit);
close(PORT);
$port_query = int($port_commit) + 1;
print "get port.commit[$port_commit], port_query[$port_query] from applicant/port.commit!\n";

#download se.conf.php
if (-f "applicant/se.conf.php") {
	`rm applicant/se.conf.php`;
}
if (-f "applicant/se.conf.php.new") {
	`rm applicant/se.conf.php.new`;
}
`scp arch\@cq01-ksarch-rdtest01.vm.baidu.com:/home/arch/archproxy/config/se.conf.php applicant/`;

print "continue, make sure se.conf.php downloaded correctly? y(n)";
my $op = <STDIN>;
chomp($op);
if ($op eq 'y' or $op eq 'yes') { 
}
else {
	print "user stoped executing!\n";
	exit();
}

open(SE2, ">applicant/se.conf.php.new") or die "can't create file se.conf.php.new!\n";
open(SE1, "<applicant/se.conf.php") or die "can't open applicant/se.conf.php!\n";
my $line;
while ($line = <SE1>) {
	if ($line =~ /__substitute_users__/) {
		print SE2 <<USERS;
		'$pid_short'=>array(
			'tk'=>'*',
			'ip'=>array('0.0.0.0/0'),
		),
USERS
	}
	if ($line =~ /__substitute_se_resources__/) {
		print SE2 <<SE_RESOURCES;
		'$pid_short'=>array(
			array(  
			'ip'=>MASTER_NMQ1,
			'port'=>9900,
			'op'=>array('insert','delete','restore','modify','modbasic'),
			),
			array(  
			'ip'=>'10.46.135.29',
			'port'=>$port_query,
			'op'=>array('select'),
			),
		 ),
SE_RESOURCES
	}
	if ($line =~ /__substitute_basic_fields__/) {
		print SE2 <<BASIC_FIELDS;
		'$pid_short'=>array(
			'insert'=>array(/*'localId','exname','nickname'*/),
			'delete'=>array(),
			'modbasic'=>array(),
			'select'=>array(),
		),
BASIC_FIELDS
	}
	if ($line =~ /__substitute_default_values__/) {
		print SE2 <<DEFAULT_VALUES;
		'$pid_short'=>array(
			'insert'=>array('cmd_no'=>20100,'command_no'=>20100),
			'delete'=>array('cmd_no'=>20101,'command_no'=>20101),
			'modbasic'=>array('cmd_no'=>20104,'command_no'=>20104),
			'select'=>array('cmd_no'=>10102,'command_no'=>10102,'page_no'=>0, 'list_num'=>20, 'sortmode'=>0),
		),  
DEFAULT_VALUES
	}
	if ($line =~ /__substitute_pid_alias__/) {
		print SE2 <<PID_ALIAS;
		'$pid_short' => '$pid_short',
PID_ALIAS
	}
	if ($line =~ /__substitute_input_encoding__/) {
		print SE2 <<INPUT_ENCODING;
		'$pid_short' => 'UTF-8',
INPUT_ENCODING
	}
	print SE2 $line;
}
close(SE1);
close(SE2);

print "rebuilding se.conf.php.new succeed!\n";

print "continue, send se.conf.php.new to archproxy(cq01-ksarch-rdtest01.vm)? y(n)";
$op = <STDIN>;
chomp($op);
if ($op eq 'y' or $op eq 'yes') { 
	`scp applicant/se.conf.php.new arch\@cq01-ksarch-rdtest01.vm.baidu.com:/home/arch/archproxy/config/se.conf.php`;
}
else {
	print "user stoped executing!\n";
	exit();
}

print "SE archproxy configurations $pid_short done ......\n";
