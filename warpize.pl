#!/usr/bin/perl

use strict;
use warnings;

use constant LIST_RES_ID => 4200;
use constant BASE_RES_ID => 9000;

open(my $fhGlobList, '<:encoding(UTF-8)', "$ARGV[0]/globlist");
open(my $fhWarpList, '>:encoding(UTF-8)', "$ARGV[0]/build/warplist");
open(my $fhWarpRc, '>:encoding(UTF-8)', "$ARGV[0]/build/warp.rc");

my $absList;
my $absLine;
my $relLine;
my $n = BASE_RES_ID;

print $fhWarpList "$ARGV[1]\n";
print $fhWarpRc LIST_RES_ID . " RCDATA \"$ARGV[0]/build/warplist\"\n";
print $fhWarpRc BASE_RES_ID . " RCDATA \"$ARGV[0]/build/$ARGV[1]\"\n";

while (my $globLine = <$fhGlobList>) {
  chomp $globLine;
  $absList = `find $ARGV[0] -type f -wholename '$ARGV[0]/$globLine'`;
  
  foreach $absLine (split /\n/, $absList) {
    $relLine = $absLine;
    $relLine =~ s:^\Q$ARGV[0]\E/::;
    
    if($relLine ne $ARGV[1]) {
      print $fhWarpList "$relLine\n";
      print $fhWarpRc (++$n) . " RCDATA \"$absLine\"\n";
    }
  }
}
