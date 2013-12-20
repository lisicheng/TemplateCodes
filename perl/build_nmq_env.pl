#!/usr/bin/perl

my $TEMPLATE_PATH = '/home/arch/se/se-tmplate';
my $SE_PATH = '/home/arch/se';
my $NMQ_PATH = '/home/arch/nmq_for_se/nmq/pusher';

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

chdir($NMQ_PATH);
if (-f "conf/module_$pid_short.conf") {
	print "file conf/module_$pid_short.conf already exists!\n";
	exit();
}
if (-f "conf/machine_$pid_short.conf") {
	print "file conf/machine_$pid_short.conf already exists!\n";
	exit();
}
`cp conf/module_template_lisicheng.conf conf/module_$pid_short.conf`;
`sed -i "s/__substitute_name__/$pid_short/g" conf/module_$pid_short.conf`;
print "module file module_$pid_short.conf created!\n";
`cp conf/machine_template_lisicheng.conf conf/machine_$pid_short.conf`;
`sed -i "s/__substitute_name__/$pid_short/g" conf/machine_$pid_short.conf`;
`sed -i "s/__substitute_port__/$port_commit/g" conf/machine_$pid_short.conf`;
print "machine file machine_$pid_short.conf created!\n";

#backup pusher_talk, pusher_client
`cp conf/pusher_talk.conf conf/pusher_talk.conf.bak`;
`cp conf/pusher_clients.conf conf/pusher_clients.conf.bak`;
open(TALK, ">>conf/pusher_talk.conf") or die "can't open conf/pusher_talk.conf!\n";
print TALK "\$include : module_$pid_short.conf\n";
close(TALK);
open(TALK, ">>conf/pusher_clients.conf") or die "can't open conf/pusher_clients.conf!\n";
print TALK "\$include : machine_$pid_short.conf\n";
close(TALK);

sleep(2);
`./bin/pusher_control restart`;
sleep(3);

print "SE nmq configurations $pid_short done ......\n";
