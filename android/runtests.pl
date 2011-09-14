#!/usr/bin/perl -w

use Cwd;
use Cwd 'abs_path';
use File::Basename;
use File::Temp 'tempdir';
use File::Path 'remove_tree';
use Getopt::Long;
use Pod::Usage;

system("adb devices")==0 or die "No device found, please plug/start at least one device/emulator\n";

my @stack = cwd;
my $device=""; # "-s device_serial";
my $packageName="org.kde.necessitas.quadruplor";
my $intentName="$packageName/eu.licentia.necessitas.industrius.QtActivity";
my $jobs = 4;
my $testsubset = "";
my $man = 0;
my $help = 0;

GetOptions('h|help' => \$help
            , man => \$man
            , 'd|device=s' => \$device
            , 't|test=s' => \$testsubset
            , 'j|jobs=i' => \$jobs
            ) or pod2usage(2);
pod2usage(1) if $help;
pod2usage(-verbose => 2) if $man;

$device = "-s $device" if ($device);
$testsubset="/$testsubset" if ($testsubset);

sub dir
{
#    print "@stack\n";
}

sub pushd ($)
{
    unless ( chdir $_[0] )
    {
        warn "Error: $!\n";
        return;
    }
    unshift @stack, cwd;
    dir;
}

sub popd ()
{
    @stack > 1 and shift @stack;
    chdir $stack[0];
    dir;
}

sub waitForProcess
{
    my $process=shift;
    my $action=shift;
    my $timeout=shift;
    my $sleepPeriod=shift;
    $sleepPeriod=1 if !defined($sleepPeriod);
    print "Waiting for $process ".$timeout*$sleepPeriod." seconds to";
    print $action?" start...\n":" die...\n";
    while($timeout--)
    {
        my $output = `adb $device shell ps 2>&1`; # get current processes
        #FIXME check why $output is not matching m/.*S $process\n/ or m/.*S $process$/ (eol)
        my $res=($output =~ m/.*S $process/)?1:0; # check the procress
        if ($action == $res)
        {
            print "... succeed\n";
            return 1;
        }
        sleep($sleepPeriod);
        print "timeount in ".$timeout*$sleepPeriod." seconds\n"
    }
    print "... failed\n";
    return 0;
}

my $src_dir_qt=abs_path(dirname($0)."/..");
my $quadruplor_dir="$src_dir_qt/android/quadruplor";
my $qmake_path="$src_dir_qt/bin/qmake";
my $tests_dir="$src_dir_qt/tests$testsubset";
my $temp_dir=tempdir(CLEANUP => 1);
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
my $output_dir=$stack[0]."/".(1900+$year)."-$mon-$mday-$hour:$min";
mkdir($output_dir);
my $sdk_api=0;
my $output = `adb $device shell getprop`; # get device properties
if ($output =~ m/.*\[ro.build.version.sdk\]: \[(\d+)\]/)
{
    $sdk_api=int($1);
    $sdk_api=5 if ($sdk_api>5 && $sdk_api<8);
    $sdk_api=9 if ($sdk_api>9);
}


sub startTest
{
    my $platform_plugin = shift;
    my $application = shift;
    system("adb $device shell am start -n $intentName -e platform_plugin $platform_plugin -e application $application"); # start intent
    #wait to start
    return 0 unless(waitForProcess($packageName,1,10));
    #wait to stop
    return 0 unless(waitForProcess($packageName,0,200,5));
    my $output_file = shift;
    system("adb $device pull /data/data/$packageName/files/output.xml $output_dir/$output_file");
    return 1;
}

########### build & install quadruplor ###########
pushd($quadruplor_dir);
system("android update project -p . -t android-10")==0 or die "Can't update project ...\n";
system("ant uninstall install")==0 or die "Can't install Quadruplor\n";
system("adb $device shell am start -n $intentName"); # create application folders
popd();

########### build qt tests and benchmarks ###########
pushd($tests_dir);
system("$qmake_path -r") == 0 or die "Can't run qmake\n"; #exec qmake
system("make -j$jobs") == 0 or warn "Can't build all tests\n"; #exec make
my $testsFiles=`find -name lib*.so`;
foreach(split("\n",$testsFiles))
{
    chomp; #remove white spaces
    pushd(abs_path(dirname($_))); # cd to application dir
    system("make INSTALL_ROOT=$temp_dir install"); # install the application to temp dir
    system("adb $device shell rm -r /data/data/$packageName/files/*"); # remove old data
    system("adb $device push $temp_dir /data/data/$packageName/files"); # copy
    my $application=basename(cwd);
    my $output_name=dirname($_);
    $output_name =~ s/\.//;   # remove first "." character
    $output_name =~ s/\///;   # remove first "/" character
    $output_name =~ s/\//_/g; # replace all "/" with "_"
    print "$output_name\n";
    if (-e "$temp_dir/libtst_$application.so")
    {
        startTest("/data/local/qt/plugins/platforms/android/libandroidGL-$sdk_api.so"
                ,"/data/data/$packageName/files/libtst_$application.so"
                , "$output_name.xml") or die "Can't run $application stopping tests ...\n";
    }
    else
    {   #ups this test application doesn't respect name conversion
        warn "$application test application doesn't respect name conversion please fix it !\n";
    }
    popd();
    remove_tree( $temp_dir, {keep_root => 1} );
}
popd();

__END__

=head1 NAME

Script to run all qt tests/benchmarks to an android device/emulator

=head1 SYNOPSIS

runtests.pl [options]

=head1 OPTIONS

=over 8

=item B<-d --device>

Device serial number. Can be empty if only one device is attached.

=item B<-t --test>

Tests subset (e.g. benchmarks, auto, auto/qbuffer, etc.).


=item B<-j --jobs>

Make jobs when building tests.

=item B<-h  --help>

Print a brief help message and exits.

=item B<--man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<This program> will run all qt tests/benchmarks to an android device/emulator.

=cut
