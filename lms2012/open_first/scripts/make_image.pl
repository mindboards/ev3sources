#!/usr/bin/perl

use strict;
use warnings;

use Time::Local;
use File::Basename;
use File::stat;
use File::Spec::Functions qw'catfile';
use File::Copy            qw'move';
use autodie               qw'move';
use Cwd	                  qw'abs_path';


my ($offset_addr1, $file1, $offset_addr2, $file2, $offset_addr3, $file3, $offset_addr4, $file4, $offset_addr5, $output_filename) = @ARGV;
#die "File Combining Tool v1.0\n\nUsage: $0 <Uboot Start Addr> <uBoot File> <uImage Start Addr> <uImage File> <Filesystem Start Addr> <Filesystem File> <UserData Start Addr> <UserData File> <Output Filename>\nDefault Output Filename : EV3-image.bin\n\n"
#    unless $offset_addr1 && $file1 && $offset_addr2 && $file2 && $offset_addr3 && $file3 && $offset_addr4 && $file4;

if(!$output_filename)
{
   $offset_addr1 = "0x0";
   $file1 = "./uBoot";
   $offset_addr2 = "0x50000";
   $file2 = "./uImage";
   $offset_addr3 = "0x250000";
   $file3 = "./EV3.cramfs.bin";
   $offset_addr4 = "0xCB0000";
   $offset_addr5 = "0xFA0000";
#   $file4 = "./userdata.tgz";
   $output_filename = "./EV3-image." . time() . ".bin";
}

if(-e $output_filename)
{
   my ($filename, $origin) = fileparse(abs_path($output_filename));
   my $dest = $origin;
   my $path = catfile $origin, $filename;
   {
      my $counter;
      while( -e $path )
      {
         $counter++;
         $path = catfile $dest, "$filename.$counter";
      }
   }
   move( catfile( $origin, $filename ), $path );
}

open FIN1, '<:raw', $file1 or die "Couldn't open $file1: $!";
open FIN2, '<:raw', $file2 or die "Couldn't open $file2: $!";
open FIN3, '<:raw', $file3 or die "Couldn't open $file3: $!";
#open FIN4, '<:raw', $file4 or die "Couldn't open $file4: $!";
open OUT, '>:raw', $output_filename or die "Couldn't open $output_filename: $!";

my ($len, $data, $dataFill, $byteCount, $offset, $blksize);

$offset   = hex($offset_addr1);
$blksize  = 4096;
#$dataFill = 0;
$dataFill = "\x{FF}";

while ($len = sysread FIN1, my $data, $blksize) {
    $offset += syswrite OUT, $data, $len;
    die "System write error: $!\n"
       unless defined $offset;
}
defined $len or die "Failed reading FIN1: $!";
printf("%s,0x%x\n",$offset,$offset);

while($offset != hex($offset_addr2))
{
    $offset += syswrite OUT, 0xFF, 1;
}
printf("%s,0x%x\n",$offset,$offset);

while ($len = sysread FIN2, my $data, $blksize) {
    $offset += syswrite OUT, $data, $len;
    die "System write error: $!\n"
	unless defined $offset;
}
defined $len or die "Failed reading FIN2: $!";
printf("%s,0x%x\n",$offset,$offset);

while($offset != hex($offset_addr3))
{
    $offset += syswrite OUT, $dataFill, 1;
}
printf("%s,0x%x\n",$offset,$offset);

while ($len = sysread FIN3, my $data, $blksize) {
    $offset += syswrite OUT, $data, $len;
    die "System write error: $!\n"
       unless defined $offset;
}
defined $len or die "Failed reading FIN3: $!";
printf("%s,0x%x\n",$offset,$offset);

while($offset != hex($offset_addr4))
{
    $offset += syswrite OUT, $dataFill, 1;
}
printf("%s,0x%x\n",$offset,$offset);

#while ($len = sysread FIN4, my $data, $blksize) {
#    $offset += syswrite OUT, $data, $len;
#    die "System write error: $!\n"
#       unless defined $offset;
#}
#defined $len or die "Failed reading FIN4: $!";
#printf("%s,0x%x\n",$offset,$offset);

while($offset != hex($offset_addr5))
{
    $offset += syswrite OUT, $dataFill, 1;
}
printf("%s,0x%x\n",$offset,$offset);

printf("File created    : %s\n",$output_filename);
printf("Total File Size : %s Bytes\n",stat($output_filename)->size);
#printf("Checksum : %s\n",stat($output_filename)->checksum);

if(stat($output_filename)->size > 16384000)
{
  printf("File too large to fit into Flash. Max size 16.384.000\n");
}
# CLOSE FILES !!!!!
#close FIN1;
#close BIN;

# End Script
