#!/bin/sh

# Calculate the zero point of an image based on reference images or a
# catalog.
#
# This script will compute some basic parameters that are necessary for the
# computation of the zero point. These parameters are saved into a
# configuration Makefile. Finally, the Makefile is executed at the end of
# this script. The reason of using Make is to parallelize the computation
# of the zero point so it obtained faster than using a script.
#
# Run with '--help' for more information.
#
# Current maintainer:
#     Sepideh Eskandarlou <sepideh.eskandarlou@gmail.com>
# Contributing authors:
#     Mohammad Akhlaghi <mohammad@akhlaghi.org>
#     Raul Infante-Sainz <infantesainz@gmail.com>
# Copyright (C) 2022-2024 Free Software Foundation, Inc.
#
# Gnuastro is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# Gnuastro is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along
# with Gnuastro. If not, see <http://www.gnu.org/licenses/>.


# Exit the script in the case of failure
set -e


# 'LC_NUMERIC' is responsible for formatting numbers printed by the OS.  It
# prevents floating points like '23,45' instead of '23.45'.
export LC_NUMERIC=C





# Default parameter's values
hdu=1
quiet=""
output=""
tmpdir=""
refcat=""
refimgs=""
starcat=""
keeptmp=""
keepzpap=""
refimgszp=""
refcathdu=""
numthreads=0
starcathdu=""
aperarcsec=""
refcatra="RA"
refimgshdu=""
starcatra="RA"
refcatdec="DEC"
matchradius=0.2
starcatdec="DEC"
magnituderange=""
version=@VERSION@
refcatmag="MAGNITUDE"
scriptname=@SCRIPT_NAME@
installdir=@PREFIX@/share/gnuastro




# Output of '--usage'
print_usage() {
     cat <<EOF
$scriptname: run with '--help' to list the options.
EOF
}





# Output of '--help'
print_help() {
   cat <<EOF
Usage: $scriptname [OPTIONS] image.fits

Calculate the Zeropoint of the image based on a reference images or
catalog.

$scriptname options:
 Input(s):
  -h, --hdu=STR/INT            HDU/Extension name or number of input.
  -S, --starcat=STR            Catalog with positions for aperture photomery.
      --starcathdu=STR/INT     HDU of table in '--starcat'.
      --starcatra=STR/INT      Column in '--starcat' that contains RA.
      --starcatdec=STR/INT     Column in '--starcat' that contains DEC.
      --mksrc=STR              Makefile (for developers when debugging).

 Output:
  -o, --output                 Output file name.
  -t, --tmpdir                 Directory to keep temporary files.
  -k, --keeptmp                Keep temporal/auxiliar files.
  -K, --keepzpap               Keep each aperture's zp in a different HDU.

 General settings
  -a, --aperarcsec=FLT[,FLT]   Aperture radius (in arc-seconds) for photometry.
  -M, --magnituderange=FLT,FLT Range of the magnitude to be considered.

 Reference catalog mode
  -c, --refcat=STR             Reference catalog file name.
  -C, --refcathdu=STR/INT      Reference catalog HDU name of number.
  -r, --refcatra=STR           Reference catalog Right Ascension (R.A.) column.
  -d, --refcatdec=STR          Reference catalog Declination (Dec) column.
  -m, --refcatmag=STR          Reference catalog Magnitude column.
  -s, --matchradius=FLT        Radius (arcsec) to match stars and ref. catalog.

 Reference image mode
  -R, --refimgs=STR[,STR]      Reference image name(s) with known zeropoint.
  -H, --refimgshdu=STR/INT     Reference image HDU name(s) or number (s).
  -z, --refimgszp=FLT[,FLT]    Zero point(s) of the reference image(s).

 Operating mode:
  -h, --help                   Print this help.
      --cite                   BibTeX citation for this program.
  -q, --quiet                  Don't print any extra information in stdout.
  -V, --version                Print program version.
  -N, --numthreads=INT         Number of threads; maximum if not given.

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponfing short options.

GNU Astronomy Utilities home page: http://www.gnu.org/software/gnuastro/

Report bugs to bug-gnuastro@gnu.org
EOF
}





# Output of '--version':
print_version() {
     cat <<EOF
$scriptname (GNU Astronomy Utilities) $version
Copyright (C) 2022-2024 Free Software Foundation, Inc.
License GPLv3+: GNU General public license version 3 or later.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written/developed by Sepideh Eskandarlou et al.
EOF
}

# Output of `--cite':
print_citation() {
    empty="" # needed for the ascii art!
    cat <<EOF

Thank you for using $scriptname (GNU Astronomy Utilities) $version

Citations and acknowledgement are vital for the continued work on Gnuastro.

Please cite the following record(s) and add the acknowledgement statement below in your work to support us. Please note that different Gnuastro programs may have different corresponding papers. Hence, please check all the programs you used. Don't forget to also include the version as shown above for reproducibility.

Paper introducing this script
-----------------------------
  @ARTICLE{astscrip-zeropoint,
         author = {{Eskandarlou}, Sepideh and {Akhlaghi}, Mohammad and {Infante-Sainz}, Ra{\'u}l and {Saremi}, Elham and {Raji}, Samane and {Sharbaf}, Zahra and {Golini}, Giulia and {Ghaffari}, Zohreh and {Knapen}, Johan H.},
          title = "{Gnuastro: Estimating the Zero-point Magnitude in Astronomical Imaging}",
        journal = {Research Notes of the American Astronomical Society},
       keywords = {Flux calibration, Astronomy software, Open source software, Astronomical techniques, 544, 1855, 1866, 1684, Astrophysics - Instrumentation and Methods for Astrophysics},
           year = 2023,
          month = dec,
         volume = {7},
         number = {12},
            eid = {269},
          pages = {269},
            doi = {10.3847/2515-5172/ad14f4},
  archivePrefix = {arXiv},
         eprint = {2312.04263},
   primaryClass = {astro-ph.IM},
         adsurl = {https://ui.adsabs.harvard.edu/abs/2023RNAAS...7..269E},
        adsnote = {Provided by the SAO/NASA Astrophysics Data System}
  }

Paper introducing Gnuastro (currently main citation)
----------------------------------------------------
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




# Functions to check option values and complain if necessary.
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





# Separate command-line arguments from options and put the option values
# into the respective variables.
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
inputs=""
while [ $# -gt 0 ]
do
    case "$1" in

        # Input parameters.
        -h|--hdu)                hdu="$2";                                     check_v "$1" "$hdu";  shift;shift;;
        -h=*|--hdu=*)            hdu="${1#*=}";                                check_v "$1" "$hdu";  shift;;
        -h*)                     hdu=$(echo "$1" | sed -e's/-h//');            check_v "$1" "$hdu";  shift;;
        -s|--starcat)            starcat="$2";                                 check_v "$1" "$starcat";  shift;shift;;
        -s=*|--starcat=*)        starcat="${1#*=}";                            check_v "$1" "$starcat";  shift;;
        -s*)                     starcat=$(echo "$1" | sed -e's/-c//');        check_v "$1" "$starcat";  shift;;
        --starcathdu)            starcathdu="$2";                              check_v "$1" "$starcathdu";  shift;shift;;
        --starcathdu=*)          starcathdu="${1#*=}";                         check_v "$1" "$starcathdu";  shift;;
        -c|--refcat)             refcat="$2";                                  check_v "$1" "$refcat";  shift;shift;;
        -c=*|--refcat=*)         refcat="${1#*=}";                             check_v "$1" "$refcat";  shift;;
        -c*)                     refcat=$(echo "$1" | sed -e's/-c//');         check_v "$1" "$refcat";  shift;;
        -C|--refcathdu)          refcathdu="$2";                               check_v "$1" "$refcathdu";  shift;shift;;
        -C=*|--refcathdu=*)      refcathdu="${1#*=}";                          check_v "$1" "$refcathdu";  shift;;
        -C*)                     refcathdu=$(echo "$1" | sed -e's/-C//');      check_v "$1" "$refcathdu";  shift;;
        -r|--refcatra)           refcatra="$2";                                check_v "$1" "$refcatra";  shift;shift;;
        -r=*|--refcatra=*)       refcatra="${1#*=}";                           check_v "$1" "$refcatra";  shift;;
        -r*)                     refcatra=$(echo "$1" | sed -e's/-r//');       check_v "$1" "$refcatra";  shift;;
        -d|--refcatdec)          refcatdec="$2";                               check_v "$1" "$refcatdec";  shift;shift;;
        -d=*|--refcatdec=*)      refcatdec="${1#*=}";                          check_v "$1" "$refcatdec";  shift;;
        -d*)                     refcatdec=$(echo "$1" | sed -e's/-d//');      check_v "$1" "$refcatdec";  shift;;
        -m|--refcatmag)          refcatmag="$2";                               check_v "$1" "$refcatmag";  shift;shift;;
        -m=*|--refcatmag=*)      refcatmag="${1#*=}";                          check_v "$1" "$refcatmag";  shift;;
        -m*)                     refcatmag=$(echo "$1" | sed -e's/-m//');      check_v "$1" "$refcatmag";  shift;;
        -R|--refimgs)            refimgs="$2";                                 check_v "$1" "$refimgs";  shift;shift;;
        -R=*|--refimgs=*)        refimgs="${1#*=}";                            check_v "$1" "$refimgs";  shift;;
        -R*)                     refimgs=$(echo "$1" | sed -e's/-R//');        check_v "$1" "$refimgs";  shift;;
        -H|--refimgshdu)         refimgshdu="$2";                              check_v "$1" "$refimgshdu";  shift;shift;;
        -H=*|--refimgshdu=*)     refimgshdu="${1#*=}";                         check_v "$1" "$refimgshdu";  shift;;
        -H*)                     refimgshdu=$(echo "$1" | sed -e's/-H//');     check_v "$1" "$refimgshdu";  shift;;
        --mksrc)                 mksrc="$2";                                   check_v "$1" "$mksrc";  shift;shift;;
        --mksrc=*)               mksrc="${1#*=}";                              check_v "$1" "$mksrc";  shift;;

        # Parameters.
        -M|--magnituderange)     magnituderange="$2";                          check_v "$1" "$magnituderange";  shift;shift;;
        -M=*|--magnituderange=*) magnituderange="${1#*=}";                     check_v "$1" "$magnituderange";  shift;;
        -M*)                     magnituderange=$(echo "$1" | sed -e's/-M//'); check_v "$1" "$magnituderange";  shift;;
        -s|--matchradius)        matchradius="$2";                             check_v "$1" "$matchradius";  shift;shift;;
        -s=*|--matchradius=*)    matchradius="${1#*=}";                        check_v "$1" "$matchradius";  shift;;
        -s*)                     matchradius=$(echo "$1" | sed -e's/-s//');    check_v "$1" "$matchradius";  shift;;
        -z|--refimgszp)          refimgszp="$2";                             check_v "$1" "$refimgszp";  shift;shift;;
        -z=*|--refimgszp=*)      refimgszp="${1#*=}";                        check_v "$1" "$refimgszp";  shift;;
        -z*)                     refimgszp=$(echo "$1" | sed -e's/-z//');    check_v "$1" "$refimgszp";  shift;;
        -a|--aperarcsec)         aperarcsec="$2";                              check_v "$1" "$aperarcsec";  shift;shift;;
        -a=*|--aperarcsec=*)     aperarcsec="${1#*=}";                         check_v "$1" "$aperarcsec";  shift;;
        -a*)                     aperarcsec=$(echo "$1" | sed -e's/-a//');     check_v "$1" "$aperarcsec";  shift;;

        # Output parameters
        -k|--keeptmp)            keeptmp=1; shift;;
        -k*|--keeptmp=*)         on_off_option_error --keeptmp -k;;
        -t|--tmpdir)             tmpdir="$2";                                  check_v "$1" "$tmpdir"; shift;shift;;
        -t=*|--tmpdir=*)         tmpdir="${1#*=}";                             check_v "$1" "$tmpdir"; shift;;
        -t*)                     tmpdir=$(echo "$1" | sed -e's/-t//');         check_v "$1" "$tmpdir"; shift;;
        -o|--output)             output="$2";                                  check_v "$1" "$output"; shift;shift;;
        -o=*|--output=*)         output="${1#*=}";                             check_v "$1" "$output"; shift;;
        -o*)                     output=$(echo "$1" | sed -e's/-o//');         check_v "$1" "$output"; shift;;
        -K|--keepzpap)           keepzpap=1; shift;;
        -K*|--keepzpap=*)        on_off_option_error --keepzpap -K;;

        # Operating mode options.
        -N|--numthreads)        numthreads="$2";                               check_v "$1" "$numthreads";  shift;shift;;
        -N=*|--numthreads=*)    numthreads="${1#*=}";                          check_v "$1" "$numthreads";  shift;;
        -N*)                    numthreads=$(echo "$1" | sed -e's/-N//');      check_v "$1" "$numthreads";  shift;;
        -q|--quiet)             quiet=" -q"; shift;;
        -q*|--quiet=*)          on_off_option_error --quiet -q;;
        -?|--help)              print_help; exit 0;;
        -'?'*|--help=*)         on_off_option_error --help -?;;
        -V|--version)           print_version; exit 0;;
        -V*|--version=*)        on_off_option_error --version -V;;
        --cite)                 print_citation; exit 0;;
        --cite=*)               on_off_option_error --cite;;

        # Unrecognized option:
        -*) echo "$scriptname: unknown option '$1'"; exit 1;;

        # Not an option (not starting with a `-'): assumed to be input FITS
        # file name.
        *) if [ x"$inputs" = x ]; then inputs="$1"; else inputs="$inputs $1"; fi; shift;;
    esac
done





# Basic sanity checks
# ===================

# If an input image is not given at all.
if [ x"$inputs" = x ]; then
    cat <<EOF
$scriptname: no input FITS image files. Run with '--help' for more information on how to run
EOF
    exit 1
elif [ ! -f $inputs ]; then
    cat <<EOF
$scriptname: $inputs, no such file or directory
EOF
    exit 1
fi

# If the brighter and fainter range of magnitude are not given at all.
if [ x$magnituderange != x ]; then
    nmagrng=$(echo $magnituderange | awk 'BEGIN{FS=","} \
                                          {for(i=1;i<=NF;++i) c+=$i!=""} \
                                          END{print c}')
    if [ x$nmagrng != x2 ]; then
        cat<<EOF
$scriptname: '--magnituderange' (or '-m') only takes two values, but $nmagrng were given
EOF
        exit 1
    fi
fi

# If an aperture size is not given.
if [ x"$aperarcsec" = x ]; then
    cat <<EOF
$scriptname: '--aperarcsec' (or '-a') is necessary at least with one value. Run with '--help' for more information
EOF
    exit 1
fi

# If atleast one of '--refcat' or '--refimgs' are given, but not together.
if [ x"$refcat$refimgs" = x ]; then
    cat <<EOF
$scriptname: no reference image(s)/catalog provided! Please use '--refcat' or '--refimgs' to specify the reference
EOF
    exit 1
fi
if [ x"$refcat" != x ] && [ x"$refimgs" != x ]; then
    cat <<EOF
$scriptname: only one of '--refimgs' or '--refcat' should be given
EOF
    exit 1
fi





# Define a temporary directory and the final output file
# ------------------------------------------------------
#
# Construct the temporary directory. If the user does not specify any
# directory, then a default one with the base name of the input image will
# be constructed.  If the user set the directory, then make it. This
# directory will be deleted at the end of the script if the user does not
# want to keep it (with the `--keeptmp' option).

# The final catalog is also defined here if the user does not provide an
# explicit name. If the user has defined a specific path/name for the
# output, it will be used for saving the output file. If the user does not
# specify an output name, then a default value containing the field, min,
# and max magnitudes will will be generated.
basename=$(basename $inputs | sed 's/\.fits/ /' | awk '{print $1}')
if [ x"$tmpdir" = x ]; then \
  tmpdir=$(pwd)/"$basename"_zeropoint
fi

# The main job of this script is managed in a Makefile. In Make, if the all
# the prerequiesits exist, Make will not build the prerequisite in a second
# run. To ensure that each run will repeat the full analysis (in case a
# user changes a parameter for example), we need to make sure that the
# temporary directory is empty.
if [ -d "$tmpdir" ]; then
    rm -f "$tmpdir"/*
else
    mkdir "$tmpdir"
fi

# Default output catalog file
if [ x"$output" = x ]; then
  output="$basename"_zeropoint.fits
fi





# Overlap check between input and reference images
# ------------------------------------------------
#
# In this step, we have to check and be sure if the input image overlaps
# with the reference catalog or reference images. If they overlap,
# everything is fine and script can contiue. But if they do not overlap
# together the script will stop and let the user know that the input image
# does not overlap with reference catalog or image.

# Find minimum and maximum RA/Dec values of the input image.
allradecinput=$(astfits $inputs --hdu=$hdu --skycoverage -q | awk 'NR==2')
minrainput=$(echo  $allradecinput | awk '{print $1}')
maxrainput=$(echo  $allradecinput | awk '{print $2}')
mindecinput=$(echo $allradecinput | awk '{print $3}')
maxdecinput=$(echo $allradecinput | awk '{print $4}')

# Reference is a catalog.
if [ x"$refcat" != x ]; then

    # Find the overlaping area between catalog and image.
    overlap=$tmpdir/overlap.fits
    asttable $refcat --inpolygon=$refcatra,$refcatdec \
             --polygon="  $minrainput,$mindecinput \
                        : $minrainput,$maxdecinput \
                        : $maxrainput,$mindecinput \
                        : $maxrainput,$maxdecinput" \
             --output=$overlap

    # Find the number of stars in the overlaping area.
    number=$(astfits $overlap -h1 --keyvalue=NAXIS2 -q)

    # Stop the execution if the number of sources is zero (common sources
    # not found).
    if [ "$number" = 0 ]; then

        # Stop if the catalog doesn't overlap with the image.
        cat <<EOF
$scriptname: input image and reference catalog do not overlap on the sky
EOF
        exit 1
    fi

# Reference is an image.
else

    # Find how many references images are used and go over all of them.
    number=0
    nums=$(echo $refimgs \
                | awk 'BEGIN{FS=","}{for(i=1;i<=NF;++i) printf "%s ", i}')
    for n in $nums; do

        # Extract this reference image name and its HDU.
        img=$(echo $refimgs       | awk 'BEGIN{FS=","}{printf "%s", $'$n'}')
        hduimg=$(echo $refimgshdu | awk 'BEGIN{FS=","}{printf "%s", $'$n'}')

        # See if any of the edges of this reference image are within the
        # input image.
        skycoverage=$tmpdir/skycoverage-$n.fits
        astfits $img --hdu=$hduimg --skycoverage -q \
            | awk 'NR==2 {printf "%f %f\n%f %f\n%f %f\n%f %f\n", \
                                 $1,$3,$1,$4,$2,$3,$2,$4}' \
            | asttable --polygon="  $minrainput,$mindecinput \
                                  : $minrainput,$maxdecinput \
                                  : $maxrainput,$mindecinput \
                                  : $maxrainput,$maxdecinput" \
                       --output=$skycoverage

        # The number of edges in the overlaping area.
        num=$(asttable $skycoverage | wc -l)
        number=$((number+num))
    done

    # If reference image(s) overlap(s) with input image, the script will
    # continue and obtain the zero point. While, if they do not overlap the
    # script will stopped.
    if [ "$number" = 0 ]; then
        cat <<EOF
$scriptname: input and reference image(s) does not overlap on the sky
EOF
        exit 1
    fi
fi





# Prepare a configuration file for running the Makefile
# -----------------------------------------------------
#
# Here, necessary parameters to run the Makefile are set as Make parameters
# that will be included in the 'zeropoint.mk' in it execution.
#
# Basic parameters.
config=$tmpdir/zeropoint.conf
echo "zpinput = 0" > $config
echo "input = $inputs" >> $config
echo "hduinput = $hdu" >> $config
echo "output = $output" >> $config
echo "tmpdir = $tmpdir" >> $config
echo "starcat = $starcat" >> $config
echo "starcatra = $starcatra" >> $config
echo "starcatdec = $starcatdec" >> $config
echo "starcathdu = $starcathdu" >> $config
echo "matchradius = $matchradius" >> $config

# Magnitude range (empty string if not provided).
if [ x$magnituderange = x ]; then
    echo "magrange = " >> $config
else
    echo "magrange = $magnituderange" >> $config
fi

# Size of the apertures. In Make, the comma should be changed with a SPACE.
aper=$(echo $aperarcsec | sed 's|,| |g')
echo "aper-arcsec = $aper" >> $config

# If a catalog is provided as the reference.
if [ x"$refcat" != x ]; then

    # Basic parameters.
    echo "reftype = cat" >> $config
    echo "ref1 = $refcat" >> $config
    echo "hduref1 = $refcathdu" >> $config
    echo "refnumber = 1" >> $config
    echo "ra  = $refcatra" >> $config
    echo "dec = $refcatdec" >> $config
    echo "mag = $refcatmag" >> $config

else

    # Type of reference file: img.
    echo "reftype = img" >> $config

    # Compute how many references images are used.
    num=$(echo $refimgs \
               | awk 'BEGIN{FS=","}{for(i=1;i<=NF;++i) printf "%s ", i}')
    echo "refnumber = $num" >> $config

    # For each image, write its variables.
    for n in $num; do
        img=$(echo $refimgs | awk 'BEGIN{FS=","}{printf "%s", $'$n'}')
        hdu=$(echo $refimgshdu | awk 'BEGIN{FS=","}{printf "%s", $'$n'}')
        zp=$(echo $refimgszp | awk 'BEGIN{FS=","}{printf "%s", $'$n'}')
        echo "ref$n = $img" >> $config
        echo "hduref$n = $hdu" >> $config
        echo "zpref$n = $zp" >> $config
    done
fi





# Keep the zeropoint from the different apertures in different extensions
# -----------------------------------------------------------------------
#
# If the user specifies to keep the zeropoint of the different apertures
# they will be saved in different extensions of the output.
if [ x"$keepzpap" = x ]; then
    echo "keepzpap =    " >> $config
else
    echo "keepzpap = keep" >> $config
fi





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
# Here, Makefile is invoked.
if [ x$mksrc = x ]; then
    mksrc=$installdir/zeropoint.mk
fi
make -f $mksrc tmpdir=$tmpdir --jobs=$numthreads





# Remove temporary directory
# --------------------------
#
# If user does not specify to keep the build file with the option of
# --keeptmp', then the directory will be removed.
if [ x"$keeptmp" = x ]; then
   rm -r $tmpdir
fi





# Inform the user
# ---------------
#
# The outputs are complex and it may not be clear to a new user that the
# job is complete without any errors. So we will print the following
# message:
if [ x"$quiet" = x ]; then
    cat <<EOF
$scriptname: SUCCESS! Output: '$output'
EOF
fi
