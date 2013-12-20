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

#mkdir env directory
chdir($SE_PATH);
if (-d $pid) {
	print("directory $pid already exists!\n");
	exit();
}
mkdir($pid);
if (-d $pid) {
	print("directory $pid created!\n");
}
else {
	print("can't create directory $pid!\n");
	exit();
}

#copy objects
`cp -r $TEMPLATE_PATH/bin $SE_PATH/$pid`;
print "created $pid/bin\n";
`cp -r $TEMPLATE_PATH/conf $SE_PATH/$pid`;
print "created $pid/conf\n";
`cp -r $TEMPLATE_PATH/data $SE_PATH/$pid`;
print "created $pid/data\n";
`cp -r $TEMPLATE_PATH/lib $SE_PATH/$pid`;
print "created $pid/lib\n";
`cp -r $TEMPLATE_PATH/limits $SE_PATH/$pid`;
print "created $pid/limits\n";
`cp -r $TEMPLATE_PATH/log $SE_PATH/$pid`;
print "created $pid/log\n";
`cp -r $TEMPLATE_PATH/mk_env.sh $SE_PATH/$pid`;
print "created $pid/mk_env.sh\n";
`cp -r $TEMPLATE_PATH/status  $SE_PATH/$pid`;
print "created $pid/status\n";
`cp -r $TEMPLATE_PATH/supervise.se $SE_PATH/$pid`;
print "created $pid/supervise.se\n";

#replace flexse.conf
`cp $TEMPLATE_PATH/applicant/flexse.conf $SE_PATH/$pid/conf/`;
print "flexse.conf replaced!\n";

chdir("$SE_PATH/$pid");
#substitute ports
`sed -i "s/__substitute_commit__/$port_commit/g" conf/se.conf`;
print "se.conf commit port substituted with $port_commit\n";
`sed -i "s/__substitute_query__/$port_query/g" conf/se.conf`;
print "se.conf query port substituted with $port_query\n";

#run env
`sh mk_env.sh`;
sleep(3);
`sh mk_env.sh`;
sleep(3);

print "SE module $pid done......\n";
