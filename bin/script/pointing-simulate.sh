#!/bin/sh
# Script to simuate a dither pattern based on an existing image
# (accounting for its distortion).
#
# Original author:
#     Mohammad Akhlaghi <mohammad@akhlaghi.org>
# Contributing author(s):
# Copyright (C) 2023-2023 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Exit the script in the case of failure
set -e

# 'LC_NUMERIC' is responsible for formatting numbers printed by the OS.  It
# prevents floating points like '23,45' instead of '23.45'.
export LC_NUMERIC=C

# Default values.
img=""
hdu=1
cat=""
width=""
quiet=""
imghdu=1
center=""
keeptmp=0
racol="RA"
deccol="DEC"
widthinpix=0
numthreads=0
version=@VERSION@
hook_warp_after=""
hook_warp_before=""
stack_operator="sum"
scriptname=@SCRIPT_NAME@
ctype="RA---TAN,DEC--TAN"
output=pointing-simulate.fits
installdir=@PREFIX@/share/gnuastro




# Output of '--help'
print_help() {
   cat <<EOF
Usage: $scriptname positions-cat.fits --center=1.23,4.56 \
                   --width=2,2 --img=image.fits [OPTIONS]

Given a set of dithering positions ('positions-cat.fits' in the example
above), and an image ('image.fits' which can be from any part of the sky,
only its distortion and orientation are important), build the exposure map
of the output stack after applying that dither (where each pixel contains
the number of exposures that were used in it). The field of the final stack
can be set with the '--center' and '--width' options.

$scriptname options:
 Input:
  -h, --hdu=STR/INT       HDU name or number of the positions catalog.
  -i, --img=STR           Name of reference image.
  -H, --imghdu=STR/INT    HDU name or number of the input image.
  -r, --racol=STR/INT     Name/number of column containing RA.
  -d, --deccol=STR/INT    Name/number of column containing Declination.
      --mksrc=STR         Makefile (for developers when debugging).

 Hooks:
      --hook-warp-before='COMMAND' Before warping each exposure;
                          Input: '$EXPOSURE'. Output: '$TOWARP'.
      --hook-warp-after='COMMAND' After warping each exposure;
                          Input: '$WARPED'. Output: '$TARGET'.

 Output:
  -o, --output=STR        Name of finally stacked image.
  -C, --center=FLT,FLT    Center of output stack (in RA,Dec).
  -w, --width=FLT,FLT     Width of output stack (in WCS).
      --ctype=STR,STR     Projection of output (CTYPEi in WCS).
      --widthinpix        Interpret '--width' values in pixels.
      --stack-operator="STR" Arithmetic operator to use for stacking.
  -t, --tmpdir=STR        Directory to keep temporary files.
  -k, --keeptmp           Keep temporal/auxiliar files.

 Operating mode:
  -?, --help              Print this help.
  -N, --numthreads=INT    Number of threads; maximum if not given.
      --cite              BibTeX citation for this program.
  -q, --quiet             Don't print any extra information in stdout.
  -V, --version           Print program version.

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponfing short options.

GNU Astronomy Utilities home page: http://www.gnu.org/software/gnuastro/

Report bugs to bug-gnuastro@gnu.org
EOF
}

print_version() {
     cat <<EOF
$scriptname (GNU Astronomy Utilities) $version
Copyright (C) 2023-2023 Free Software Foundation, Inc.
License GPLv3+: GNU General public license version 3 or later.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written/developed by Mohammad Akhlaghi, Raul Infante-Sainz and Sepideh Eskandarlou.
EOF
}

print_citation() {
    empty="" # needed for the ascii art!
    cat <<EOF

Thank you for using $scriptname (GNU Astronomy Utilities) $version

Citations and acknowledgement are vital for the continued work on Gnuastro.

Please cite the following record(s) and add the acknowledgement statement below in your work to support us. Please note that different Gnuastro programs may have different corresponding papers. Hence, please check all the programs you used. Don't forget to also include the version as shown above for reproducibility.

Paper describing this script
----------------------------
  @ARTICLE{astscript-pointing-simulate,
         author = {{Akhlaghi}, Mohammad},
          title = "{Gnuastro: Simulating the Exposure Map of a Pointing Pattern}",
        journal = {Research Notes of the American Astronomical Society},
           year = 2023,
          month = oct,
         volume = {7},
         number = {10},
            eid = {211},
          pages = {211},
            doi = {10.3847/2515-5172/acfc44},
archivePrefix = {arXiv},
       eprint = {2310.15006},
 primaryClass = {astro-ph.IM},
         adsurl = {https://ui.adsabs.harvard.edu/abs/2023RNAAS...7..211A},
        adsnote = {Provided by the SAO/NASA Astrophysics Data System}
  }

First paper introducing Gnuastro (currently main citation)
----------------------------------------------------------
  @ARTICLE{gnuastro,
     author = {{Akhlaghi}, M. and {Ichikawa}, T.},
      title = "{Noise-based Detection and Segmentation of Nebulous Objects}",
    journal = {ApJS},
  archivePrefix = "arXiv",
     eprint = {1505.01664},
   primaryClass = "astro-ph.IM",
       year = 2015,
      month = sep,
     volume = 220,
        eid = {1},
      pages = {1},
        doi = {10.1088/0067-0049/220/1/1},
     adsurl = {https://ui.adsabs.harvard.edu/abs/2015ApJS..220....1A},
    adsnote = {Provided by the SAO/NASA Astrophysics Data System}
  }

Acknowledgement
---------------
This work was partly done using GNU Astronomy Utilities (Gnuastro, ascl.net/1801.009) version $version. Work on Gnuastro has been funded by the Japanese Ministry of Education, Culture, Sports, Science, and Technology (MEXT) scholarship and its Grant-in-Aid for Scientific Research (21244012, 24253003), the European Research Council (ERC) advanced grant 339659-MUSICOS, the Spanish Ministry of Economy and Competitiveness (MINECO, grant number AYA2016-76219-P) and the NextGenerationEU grant through the Recovery and Resilience Facility project ICTS-MRR-2021-03-CEFCA.
                                               ,
                                              {|'--.
                                             {{\    \ $empty
      Many thanks from all                   |/\`'--./=.
      Gnuastro developers!                   \`\.---' \`\\
                                                  |\  ||
                                                  | |//
                                                   \//_/|
                                                   //\__/
                                                  //
                   (http://www.chris.com/ascii/) |/

EOF
}

on_off_option_error() {
    if [ x"$2" = x ]; then
        echo "$scriptname: '$1' doesn't take any values"
    else
        echo "$scriptname: '$1' (or '$2') doesn't take any values"
    fi
    exit 1
}

check_v() {
    if [ x"$2" = x ]; then
        cat <<EOF
$scriptname: option '$1' requires an argument. Try '$scriptname --help' for more information
EOF
        exit 1;
    fi
}





# Read the command-line options
# -----------------------------
#
# OPTIONS WITH A VALUE:
#
#   Each option has three lines because we take into account the three common
#   formats:
#   For long option names, '--longname value' and '--longname=value'.
#   For short option names, '-l value', '-l=value' and '-lvalue'
#   (where '-l' is the short version of the hypothetical '--longname option').
#
#   The first case (with a space between the name and value) is two
#   command-line arguments. So, we'll need to shift it twice. The
#   latter two cases are a single command-line argument, so we just need to
#   "shift" the counter by one.
#
#   IMPORTANT NOTE: the ORDER OF THE LATTER TWO cases matters: '-h*' should be
#   checked only when we are sure that its not '-h=*').
#
# OPTIONS WITH NO VALUE (ON-OFF OPTIONS)
#
#   For these, we just want the forms of '--longname' or '-l'. Nothing
#   else. So if an equal sign is given we should definitely crash and also,
#   if a value is appended to the short format it should crash. So in the
#   second test for these ('-l*') will account for both the case where we
#   have an equal sign and where we don't.
while [ $# -gt 0 ]
do
    case "$1" in

    # Input parameters.
    -h|--hdu)        hdu="$2";                                    check_v "$1" "$hdu";    shift;shift;;
    -h=*|--hdu=*)    hdu="${1#*=}";                               check_v "$1" "$hdu";    shift;;
    -h*)             hdu=$(echo "$1" | sed -e's/-h//');           check_v "$1" "$hdu";    shift;;
    -i|--img)        img="$2";                                    check_v "$1" "$img";    shift;shift;;
    -i=*|--img=*)    img="${1#*=}";                               check_v "$1" "$img";    shift;;
    -i*)             img=$(echo "$1" | sed -e's/-i//');           check_v "$1" "$img";    shift;;
    -H|--imghdu)     imghdu="$2";                                 check_v "$1" "$imghdu"; shift;shift;;
    -H=*|--imghdu=*) imghdu="${1#*=}";                            check_v "$1" "$imghdu"; shift;;
    -H*)             imghdu=$(echo "$1" | sed -e's/-H//');        check_v "$1" "$imghdu"; shift;;
    -r|--racol)      racol="$2";                                  check_v "$1" "$racol";  shift;shift;;
    -r=*|--racol=*)  racol="${1#*=}";                             check_v "$1" "$racol";  shift;;
    -r*)             racol=$(echo "$1" | sed -e's/-r//');         check_v "$1" "$racol";  shift;;
    -d|--deccol)     deccol="$2";                                 check_v "$1" "$deccol"; shift;shift;;
    -d=*|--deccol=*) deccol="${1#*=}";                            check_v "$1" "$deccol"; shift;;
    -d*)             deccol=$(echo "$1" | sed -e's/-d//');        check_v "$1" "$deccol"; shift;;
    --mksrc)         mksrc="$2";                                  check_v "$1" "$mksrc";  shift;shift;;
    --mksrc=*)       mksrc="${1#*=}";                             check_v "$1" "$mksrc";  shift;;

    # Hooks
    --hook-warp-after) hook_warp_after=$(echo "$2" | sed -e's@\$@\$\$@g'); check_v "$1" "$hook_warp_after"; shift;;
    --hook-warp-after=*) v="${1#*=}"; hook_warp_after=$(echo "$v" | sed -e's@\$@\$\$@g'); check_v "$1" "$hook_warp_after"; shift;;
    --hook-warp-before) hook_warp_before=$(echo "$2" | sed -e's@\$@\$\$@g'); check_v "$1" "$hook_warp_before"; shift;;
    --hook-warp-before=*) v="${1#*=}"; hook_warp_before=$(echo "$v" | sed -e's@\$@\$\$@g'); check_v "$1" "$hook_warp_before"; shift;;

    # Output:
    -o|--output)     output="$2";                                 check_v "$1" "$output"; shift;shift;;
    -o=*|--output=*) output="${1#*=}";                            check_v "$1" "$output"; shift;;
    -o*)             output=$(echo "$1" | sed -e's/-o//');        check_v "$1" "$output"; shift;;
    -C|--center)     center="$2";                                 check_v "$1" "$center"; shift;shift;;
    -C=*|--center=*) center="${1#*=}";                            check_v "$1" "$center"; shift;;
    -C*)             center=$(echo "$1" | sed -e's/-C//');        check_v "$1" "$center"; shift;;
    -w|--width)      width="$2";                                  check_v "$1" "$width";  shift;shift;;
    -w=*|--width=*)  width="${1#*=}";                             check_v "$1" "$width";  shift;;
    -w*)             width=$(echo "$1" | sed -e's/-w//');         check_v "$1" "$width";  shift;;
    --ctype)         ctype="$2";                                  check_v "$1" "$ctype";  shift;shift;;
    --ctype=*)       ctype="${1#*=}";                             check_v "$1" "$ctype";  shift;;
    --widthinpix)    widthinpix=1;                                                        shift;;
    --widthinpix=*)  on_off_option_error --quiet -q;;
    --stack-operator) stack_operator="$2";                        check_v "$1" "$stack_operator";  shift;shift;;
    --stack-operator=*) stack_operator="${1#*=}";                 check_v "$1" "$stack_operator";  shift;;
    -k|--keeptmp)           keeptmp=1; shift;;
    -k*|--keeptmp=*)        on_off_option_error --keeptmp -k;;
    -t|--tmpdir)            tmpdir="$2";                                  check_v "$1" "$tmpdir";  shift;shift;;
    -t=*|--tmpdir=*)        tmpdir="${1#*=}";                             check_v "$1" "$tmpdir";  shift;;
    -t*)                    tmpdir=$(echo "$1" | sed -e's/-t//');         check_v "$1" "$tmpdir";  shift;;

    # Non-operating options.
    -q|--quiet)             quiet="--quiet"; shift;;
    -q*|--quiet=*)          on_off_option_error --quiet -q;;
    -?|--help)              print_help; exit 0;;
    -'?'*|--help=*)         on_off_option_error --help -?;;
    -V|--version)           print_version; exit 0;;
    -V*|--version=*)        on_off_option_error --version -V;;
    --cite)                 print_citation; exit 0;;
    --cite=*)               on_off_option_error --cite;;
    -N|--numthreads)        numthreads="$2";                          check_v "$1" "$numthreads";  shift;shift;;
    -N=*|--numthreads=*)    numthreads="${1#*=}";                     check_v "$1" "$numthreads";  shift;;
    -N*)                    numthreads=$(echo "$1" | sed -e's/-N//'); check_v "$1" "$numthreads";  shift;;

    # Unrecognized option:
    -*) echo "$scriptname: unknown option '$1'"; exit 1;;

    # Not an option (not starting with a `-'): assumed to be the name
    # of the positions catalog.
    *)
      if [ x"$cat" = x ]; then cat="$1";
      else
	  echo "$scriptname: only one input (position catalog) is expected";
      fi
      shift;;
  esac
done





# Sanity checks
# -------------
#
# The following options are necessary (the script will not work without
# them).
if [ x"$cat" = x ]; then
    echo "$scriptname: no argument (position catalog) given"; exit 1
fi
if [ x"$img" = x ]; then
    echo "$scriptname: no reference image given to '--img'"; exit 1
fi
if [ x"$width" = x ]; then
    echo "$scriptname: no stack width given to '--width'"; exit 1
fi
if [ x"$ctype" = x ]; then
    echo "$scriptname: no projection given to '--ctype'"; exit 1
fi
if [ x"$center" = x ]; then
    echo "$scriptname: no stack center given to '--center'"; exit 1
else
    ncenter=$(echo $center | awk 'BEGIN{FS=","}\
                                  {for(i=1;i<=NF;++i) c+=$i!=""}\
                                  END{print c}')
    if [ x$ncenter != x2 ]; then
        cat <<EOF
$scriptname: '--center' (or '-c') only takes two values, but $ncenter value(s) where given in '$center'
EOF
        exit 1
    fi
fi
ndither=$(asttable $cat --info-num-rows)
if [ x$ndither = x0 ]; then
        cat <<EOF
$scriptname: $cat: input dither pointing table is empty! It should contain at least one row, containing two columns for the RA and Dec of each pointing of the dither pattern. Please see the documentation with this command: 'info astscript-pointing-simulate'
EOF
        exit 1
fi





# Temporary directory
# -------------------
#
# The directory to keep temporary files.
if [ x"$tmpdir" = x ]; then
    namecnosuffix=${cat%.*}
    namecnodir=$(echo ${namecnosuffix##*/})
    nameinosuffix=${img%.*}
    nameinodir=$(echo ${nameinosuffix##*/})
    namewidth=$(echo $width | sed 's|,|-|')
    namecenter=$(echo $center | sed 's|,|-|')
    tmpdir=dither-tmp-$namecnodir-$nameinodir-$namecenter-$namewidth
fi
if [ -d $tmpdir ]; then
    rm -f $tmpdir/*             # For debuging, you can comment this line
    #echo NOT CLEANING TMPDIR   # and uncomment the 'echo' line under it.
else
    mkdir $tmpdir
fi





# Configuration file
# ------------------
#
# All the settings given by the user and data are passed to Make through
# this configuration file (and the variables within it).
counter=1;
config=$tmpdir/pointing-simulate.conf
echo "img = $img" > $config
echo "ctype = $ctype" >> $config
echo "width = $width" >> $config
echo "quiet = $quiet" >> $config
echo "center = $center" >> $config
echo "output = $output" >> $config
echo "imghdu = $imghdu" >> $config
echo "scriptname = $scriptname" >> $config
echo "stack-operator = $stack_operator" >> $config
echo "dithers = $(seq $ndither | tr '\n' ' ')" >> $config
echo "hook-warp-before=$hook_warp_before" >> $config
echo "hook-warp-after=$hook_warp_after" >> $config
if [ $widthinpix = 1 ]; then
    echo "widthinpix = --widthinpix" >> $config
else
    echo "widthinpix = " >> $config
fi
asttable $cat -c$racol,$deccol | while read r d; do

    # Add the RA and Dec of the center of each pointing.
    echo "$counter-ra  = $r" >> $config
    echo "$counter-dec = $d" >> $config

    # Increment the counter.
    counter=$((counter+1))
done





# If the user hasn't set the number threads, find it
# --------------------------------------------------
#
# Note that on different operating systems, the command to read the number
# of threads differs: for example in GNU/Linux based systems, it is
# 'nproc', but in macOS is is a special case of 'sysctl'.
if [ x"$numthreads" = x0 ]; then
    if type nproc > /dev/null 2> /dev/null; then
        numthreads=$(nproc --all);
    else
        numthreads=$(sysctl -a | awk '/^hw\.ncpu/{print $2}')
        if [ x"$numthreads" = x ]; then numthreads=1; fi
    fi
fi





# Call the Makefile
# -----------------
#
# Make is invoked with the requested Makefile. We cannot put 'tmpdir' into
# the configuration file because the configuration file is within the
# temporary directory. Also, the number of threads should be given when
# calling Make. Otherwise, all other settings should be taken inside the
# configuration file.
if [ x"$mksrc" = x ]; then mksrc=$installdir/pointing-simulate.mk; fi
make -f $mksrc tmpdir=$tmpdir --jobs=$numthreads





# Remove temporary directory
# --------------------------
#
# If user does not specify to keep the build file with the option of
# --keeptmp', then the directory will be removed.
if [ x"$keeptmp" = x ]; then
   rm -r $tmpdir
fi





# Delete the temporary directory if necessary
if [ "$keeptmp" = 0 ]; then
   rm -rf $tmpdir
fi
