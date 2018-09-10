#!/usr/bin/perl
#
# run shadower a lot

#my $exe = "/home/mstock/code/shadower/shadower";
my $exe = "shadower";
my @sc = (0.6, 0.8);
my @ss = (0.04, 0.06, 0.08);
my @sw = (1.0, 4.0);
my @nb = (15, 25);

# default file names
my $infile = "in.png";
my $outprefix = "out";

# read input file name from command line
if ($ARGV[0]) {
  $infile = $ARGV[0];
  # assemble output prefix from input file name, if available
  $outprefix = substr($infile, 0, rindex($infile, '.'));
}

# check for the exe
my $exists = `which ${exe}`;
if (chomp($exists) == "0") {
  print "Cannot file ${exe} in the PATH. Quitting.\n";
  exit(1);
}

# check for the input file
if (! -e "${infile}") {
  print "Input file ${infile} does not exist. Quitting.\n";
  exit(1);
}

# Usage: shadower [-sc=<shade coeff>] [-sw=<shadow width>] [-ss=<shadow shift>] [-nb=<bins>] [-o=<outfile>] in.png

my $cnt = 0;
foreach $thisc (@sc) {
foreach $thiss (@ss) {
foreach $thisw (@sw) {
foreach $thisb (@nb) {

  my $command = "${exe}";
  $command .= " ${infile}";
  $command .= " -sc=${thisc}";
  $command .= " -sw=${thisw}";
  $command .= " -ss=${thiss}";
  $command .= " -nb=${thisb}";

  # assemble out file name
  my $outfile = sprintf("%s_%g_%g_%g_%d.png", $outprefix, $thisc, $thiss, $thisw, $thisb);
  $command .= " -o=${outfile}";

  if (! -e "${outfile}") {
    print "\n${command}\n";
    #system($command);
  }

  $cnt = $cnt + 1;
}
}
}
}
