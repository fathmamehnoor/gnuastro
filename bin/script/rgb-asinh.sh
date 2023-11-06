#!/bin/sh
#
# Build a color image using asinh function to manipulate the pixel values
# with the goal of showing the low and high pixel values at the same time.
# Script based in Lupton et al. (2004) http://doi.org/10.1086/382245
#
# Run with '--help' for more information.
#
# Current maintainer:
#     2021-2023 Raul Infante-Sainz <infantesainz@gmail.com>
# Contributing authors:
#     2021-2023 Mohammad Akhlaghi <mohammad@akhlaghi.org>
#     2021-2023 Samane Raji <samaneraji@gmail.com>
# Copyright (C) 2021-2023 Free Software Foundation, Inc.
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


# Save the current system language, and then change it to English to avoid
# problems in some parts of the code (AWK with `,' instead of `.' for
# decimal separator).
system_lang=$LANG
export LANG=C





# Default option values (can be changed with options on the command-line).
hdu=""
hdus=""

# Minimum, zeropoint, and weight values
minimum=""
minimums=""
zeropoint=""
zeropoints=""
weights="1.0,1.0,1.0"

# To control the asinh transformation
stretch=""
qbright=""

# For color and gray background
grayval=""
colorval=""
graykernelfwhm=0
colorkernelfwhm=0

# Linear range values. When normalizing the images to a new range, use the
# range 'newmin-newmax'. It is obtained by the following operations:
# OldRange = (OldMax - OldMin)
# NewRange = (NewMax - NewMin)
# NewValue = (((OldValue - OldMin) * NewRange) / OldRange) + NewMin
minvalrange=0.000
maxvalrange=100.0

# Transformation parameters to improve the contrast and brightness
gamma=1.0
contrast=1.0
brightness=0.0

quiet=""
tmpdir=""
keeptmp=0
checkparams=0
output="rgb-asinh.jpg"

grayback=0

version=@VERSION@
scriptname=@SCRIPT_NAME@





# Output of `--usage' and `--help':
print_usage() {
    cat <<EOF
$scriptname: run with '--help' for list of options
EOF
}

print_help() {
    cat <<EOF
Usage: $scriptname [OPTION] R.fits G.fits B.fits K.fits

This script is part of GNU Astronomy Utilities $version.

This script generates a color image from three different images (RGB). In order
to show the entire range of pixel values, an asinh transformation is used. For
more on the definiton of this algorithm see Lupton et al. 2004:

  https://doi.org/10.1086/382245

For more information, please run any of the following commands. In
particular the first contains a very comprehensive explanation of this
script's invocation: expected input(s), output(s), and a full description
of all the options.

     Inputs/Outputs and options:           $ info $scriptname
     Full Gnuastro manual/book:            $ info gnuastro

If you couldn't find your answer in the manual, you can get direct help from
experienced Gnuastro users and developers. For more information, please run:

     $ info help-gnuastro

$scriptname options:
 Input:
  -h, --hdus=STR          HDU/extensions (comma separated) for the R,G,B,K FITS images.
  -H, --hdu=STR           Common HDU/extension for the (R,G,B,K) channel FITS images
                          (this overrides -h or --hdus).

  -m, --minimums=FLT      Minimum values (comma separated) to be mapped to black (zero).
  -M, --minimum=FLT       Common minimum value to be mapped to black (zero).
                          (this overrides -m or --minimums)
  -z, --zeropoints=FLT    Zeropoints for the images (comma separated values).
  -Z, --zeropoint=FLT     Common zeropoint value for all (R,G,B) images.
                          (this overrides -z or --zeropoints argument)
  -w, --weights=FLT       Relative weights for the images (comma separated values).

 Asinh scaling parameters
  -s, --stretch=FLT       Linear stretching parameter for faint features.
  -Q, --qbright=FLT       Parameter for bringing out brighter features.

 Contrast and brightness
  -b, --brightness        Change the brightness of the final image (linear).
  -c, --contrast          Change the contrast of the final image (linear).
  -g, --gamma             Gamma parameter for gamma transformation (non linear,
                          this overrides --brightness or --contrast)

 Color and gray parameters
      --grayback            Generate the gray-background color image.
      --grayval=FLT         Value that defines the black and white (for gray regions).
      --colorval=FLT        Value that defines the separation between color and black.
      --graykernelfwhm=FLT  Kernel FWHM for convolving the background image.
      --colorkernelfwhm=FLT Kernel FWHM for convolving the reference image that is used
                            for defining the separation between the color and black parts.
 Output:
      --checkparams       Print the distribution of values for obtaining the parameters.
  -k, --keeptmp           Keep temporal/auxiliar files.
  -o, --output            Output color image name.

 Operating mode:
  -h, --help              Print this help list.
      --cite              BibTeX citation for this program.
  -q, --quiet             Don't print the list.
  -V, --version           Print program version.

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

GNU Astronomy Utilities home page: http://www.gnu.org/software/gnuastro/

Report bugs to bug-gnuastro@gnu.org.
EOF
}





# Output of `--version':
print_version() {
    cat <<EOF
$scriptname (GNU Astronomy Utilities) $version
Copyright (C) 2021-2023, Free Software Foundation, Inc.
License GPLv3+: GNU General public license version 3 or later.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written/developed by Raul Infante-Sainz
EOF
}





# Functions to check option values and complain if necessary.
on_off_option_error() {
    if [ "x$2" = x ]; then
        echo "$scriptname: '$1' doesn't take any values."
    else
        echo "$scriptname: '$1' (or '$2') doesn't take any values."
    fi
    exit 1
}

check_v() {
    if [ x"$2" = x ]; then
        echo "$scriptname: option '$1' requires an argument."
        echo "Try '$scriptname --help' for more information."
        exit 1;
    fi
}





# Separate command-line arguments from options. Then put the option
# value into the respective variable.
#
# OPTIONS WITH A VALUE:
#
#   Each option has three lines because we want to all common formats: for
#   long option names: `--longname value' and `--longname=value'. For short
#   option names we want `-l value', `-l=value' and `-lvalue' (where `-l'
#   is the short version of the hypothetical `--longname' option).
#
#   The first case (with a space between the name and value) is two
#   command-line arguments. So, we'll need to shift it two times. The
#   latter two cases are a single command-line argument, so we just need to
#   "shift" the counter by one. IMPORTANT NOTE: the ORDER OF THE LATTER TWO
#   cases matters: `-h*' should be checked only when we are sure that its
#   not `-h=*').
#
# OPTIONS WITH NO VALUE (ON-OFF OPTIONS)
#
#   For these, we just want the two forms of `--longname' or `-l'. Nothing
#   else. So if an equal sign is given we should definitely crash and also,
#   if a value is appended to the short format it should crash. So in the
#   second test for these (`-l*') will account for both the case where we
#   have an equal sign and where we don't.
while [ $# -gt 0 ]
do
    case "$1" in
        # Input parameters.
        -h|--hdus)          hdus="$2";                             check_v "$1" "$hdus";  shift;shift;;
        -h=*|--hdus=*)      hdus="${1#*=}";                        check_v "$1" "$hdus";  shift;;
        -h*)                hdus=$(echo "$1"  | sed -e's/-h//');   check_v "$1" "$hdus";  shift;;
        -H|--hdu)           hdu="$2";                              check_v "$1" "$hdu";  shift;shift;;
        -H=*|--hdu=*)       hdu="${1#*=}";                         check_v "$1" "$hdu";  shift;;
        -H*)                hdu=$(echo "$1"  | sed -e's/-H//');    check_v "$1" "$hdu";  shift;;

        -m|--minimums)      minimums="$2";                             check_v "$1" "$minimums";  shift;shift;;
        -m=*|--minimums=*)  minimums="${1#*=}";                        check_v "$1" "$minimums";  shift;;
        -m*)                minimums=$(echo "$1"  | sed -e's/-m//');   check_v "$1" "$minimums";  shift;;
        -M|--minimum)       minimum="$2";                              check_v "$1" "$minimum";  shift;shift;;
        -M=*|--minimum=*)   minimum="${1#*=}";                         check_v "$1" "$minimum";  shift;;
        -M*)                minimum=$(echo "$1"  | sed -e's/-M//');    check_v "$1" "$minimum";  shift;;
        -Z|--zeropoint)      zeropoint="$2";                            check_v "$1" "$zeropoint";  shift;shift;;
        -Z=*|--zeropoint=*)  zeropoint="${1#*=}";                       check_v "$1" "$zeropoint";  shift;;
        -Z*)                 zeropoint=$(echo "$1"  | sed -e's/-Z//');  check_v "$1" "$zeropoint";  shift;;
        -z|--zeropoints)     zeropoints="$2";                           check_v "$1" "$zeropoints";  shift;shift;;
        -z=*|--zeropoints=*) zeropoints="${1#*=}";                      check_v "$1" "$zeropoints";  shift;;
        -z*)                 zeropoints=$(echo "$1"  | sed -e's/-z//'); check_v "$1" "$zeropoints";  shift;;
        -w|--weights)       weights="$2";                              check_v "$1" "$weights";  shift;shift;;
        -w=*|--weights=*)   weights="${1#*=}";                         check_v "$1" "$weights";  shift;;
        -w*)                weights=$(echo "$1"  | sed -e's/-w//');    check_v "$1" "$weights";  shift;;

        -s|--stretch)       stretch="$2";                              check_v "$1" "$stretch";  shift;shift;;
        -s=*|--stretch=*)   stretch="${1#*=}";                         check_v "$1" "$stretch";  shift;;
        -s*)                stretch=$(echo "$1"  | sed -e's/-s//');    check_v "$1" "$stretch";  shift;;
        -Q|--qbright)       qbright="$2";                              check_v "$1" "$qbright";  shift;shift;;
        -Q=*|--qbright=*)   qbright="${1#*=}";                         check_v "$1" "$qbright";  shift;;
        -Q*)                qbright=$(echo "$1"  | sed -e's/-Q//');    check_v "$1" "$qbright";  shift;;

        -g|--gamma)          gamma="$2";                                 check_v "$1" "$gamma";  shift;shift;;
        -g=*|--gamma=*)      gamma="${1#*=}";                            check_v "$1" "$gamma";  shift;;
        -g*)                 gamma=$(echo "$1"  | sed -e's/-g//');       check_v "$1" "$gamma";  shift;;
        -c|--contrast)       contrast="$2";                              check_v "$1" "$contrast";  shift;shift;;
        -c=*|--contrast=*)   contrast="${1#*=}";                         check_v "$1" "$contrast";  shift;;
        -c*)                 contrast=$(echo "$1"  | sed -e's/-c//');    check_v "$1" "$contrast";  shift;;
        -b|--brightness)     brightness="$2";                            check_v "$1" "$brightness";  shift;shift;;
        -b=*|--brightness=*) brightness="${1#*=}";                       check_v "$1" "$brightness";  shift;;
        -b*)                 brightness=$(echo "$1"  | sed -e's/-b//');  check_v "$1" "$brightness";  shift;;

        --grayback)         grayback=1; shift;;
        --grayval)          grayval="$2";                              check_v "$1" "$grayval";  shift;shift;;
        --grayval=*)        grayval="${1#*=}";                         check_v "$1" "$grayval";  shift;;
        --colorval)         colorval="$2";                             check_v "$1" "$colorval";  shift;shift;;
        --colorval=*)       colorval="${1#*=}";                        check_v "$1" "$colorval";  shift;;
        --graykernelfwhm)   graykernelfwhm="$2";                       check_v "$1" "$graykernelfwhm";  shift;shift;;
        --graykernelfwhm=*) graykernelfwhm="${1#*=}";                  check_v "$1" "$graykernelfwhm";  shift;;
        --colorkernelfwhm)   colorkernelfwhm="$2";                     check_v "$1" "$colorkernelfwhm";  shift;shift;;
        --colorkernelfwhm=*) colorkernelfwhm="${1#*=}";                check_v "$1" "$colorkernelfwhm";  shift;;

        # Output parameters
        --checkparams)    checkparams=1; shift;;
        -k|--keeptmp)     keeptmp=1; shift;;
        -k*|--keeptmp=*)  on_off_option_error --keeptmp -k;;
        -t|--tmpdir)      tmpdir="$2";                          check_v "$1" "$tmpdir";  shift;shift;;
        -t=*|--tmpdir=*)  tmpdir="${1#*=}";                     check_v "$1" "$tmpdir";  shift;;
        -t*)              tmpdir=$(echo "$1" | sed -e's/-t//'); check_v "$1" "$tmpdir";  shift;;
        -o|--output)      output="$2";                          check_v "$1" "$output"; shift;shift;;
        -o=*|--output=*)  output="${1#*=}";                     check_v "$1" "$output"; shift;;
        -o*)              output=$(echo "$1" | sed -e's/-o//'); check_v "$1" "$output"; shift;;

        # Non-operating options.
        -q|--quiet)       quiet="--quiet"; shift;;
        -q*|--quiet=*)    on_off_option_error --quiet -q;;
        -?|--help)        print_help; exit 0;;
        -'?'*|--help=*)   on_off_option_error --help -?;;
        -V|--version)     print_version; exit 0;;
        -V*|--version=*)  on_off_option_error --version -V;;
        --cite)           astfits --cite; exit 0;;
        --cite=*)         on_off_option_error --cite;;

        # Unrecognized option:
        -*) echo "$scriptname: unknown option '$1'"; exit 1;;

        # Not an option (not starting with a `-'): assumed to be input FITS
        # file name.
        *) inputs="$inputs $1"; shift;;
    esac
done





# Basic sanity checks
# -------------------
#
# If no input is given, let the user know.
if [ x"$inputs" = x ]; then
    echo "$scriptname: no input FITS files."
    echo "Run with '--help' for more information on how to run."
    exit 1
fi


# Make sure three or four (for the gray background) inputs have been given.
ninputs=$(echo "$inputs" | awk '{print NF}')
if [ $ninputs != 3 ] && [ $ninputs != 4 ]; then
    echo "$scriptname: $ninputs inputs given, but 3 (or 4) inputs are necessary: one for each of the R,G,B,(K) channels respectively."
    exit 1
fi


# If no HDU extension is given, let the user know.
if [ x"$hdu" = x ] && [ x"$hdus" = x ]; then
    echo "$scriptname: no input HDU extension(s) given."
    echo "Run with '--help' for more information on how to run."
    exit 1
fi





# Asign basic input variables
# ---------------------------
#
# Images from the inputs from redder to bluer: R,G,B,K.
rimage=$(echo $inputs | awk '{print $1}')
gimage=$(echo $inputs | awk '{print $2}')
bimage=$(echo $inputs | awk '{print $3}')
kimage=$(echo $inputs | awk '{print $4}')


# Make sure different HDUs have been provided properly
if [ x$hdus != x ]; then
    nhdus=$(echo "$hdus" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nhdus != $ninputs ]; then
        echo "$scriptname: $ninputs HDUs '-h' or '--hdus' should be given (comma separated)."
        exit 1
    fi
    # Set the different zeropoints in case they are three
    rhdu=$(echo "$hdus" | awk 'BEGIN{FS=","}{print $1}')
    ghdu=$(echo "$hdus" | awk 'BEGIN{FS=","}{print $2}')
    bhdu=$(echo "$hdus" | awk 'BEGIN{FS=","}{print $3}')
    khdu=$(echo "$hdus" | awk 'BEGIN{FS=","}{print $4}')
fi

# Use a common HDU if it is provided
if [ x$hdu != x ]; then
    nhdu=$(echo "$hdu" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nhdu != 1 ]; then
        echo "$scriptname: a single value for '-H' or '--hdu' should be given."
        exit 1
    fi
    rhdu=$hdu
    ghdu=$hdu
    bhdu=$hdu
    khdu=$hdu
fi


# Make sure different minimums have been provided properly
if [ x$minimums != x ]; then
    nminimums=$(echo "$minimums" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nminimums != $ninputs ]; then
        echo "$scriptname: $ninputs minimums '-m' or '--minimums' shoud be given (comma separated)."
        exit 1
    fi
    # Set the different zeropoints in case they are three
    rmin=$(echo "$minimums" | awk 'BEGIN{FS=","}{print $1}')
    gmin=$(echo "$minimums" | awk 'BEGIN{FS=","}{print $2}')
    bmin=$(echo "$minimums" | awk 'BEGIN{FS=","}{print $3}')
    kmin=$(echo "$minimums" | awk 'BEGIN{FS=","}{print $4}')
fi

# Use a common minimum value if it is provided
if [ x$minimum != x ]; then
    nminimum=$(echo "$minimum" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nminimum != 1 ]; then
        echo "$scriptname: a single value for '-M' or '--minimum' should be given."
        exit 1
    fi
    rmin=$minimum
    gmin=$minimum
    bmin=$minimum
    kmin=$minimum
fi


# Make sure different zeropoints have been provided properly
if [ x$zeropoints != x ]; then
    nzeropoints=$(echo "$zeropoints" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nzeropoints != $ninputs ]; then
        echo "$scriptname: $ninputs zeropoints '-z' or '--zeropoints' should be given (comma separated)."
        exit 1
    fi
    # Set the different zeropoints in case they are three
    rzero=$(echo "$zeropoints" | awk 'BEGIN{FS=","}{print $1}')
    gzero=$(echo "$zeropoints" | awk 'BEGIN{FS=","}{print $2}')
    bzero=$(echo "$zeropoints" | awk 'BEGIN{FS=","}{print $3}')
    kzero=$(echo "$zeropoints" | awk 'BEGIN{FS=","}{print $4}')
fi

# Use a common zeropoint value if it is provided
if [ x$zeropoint != x ]; then
    nzeropoint=$(echo "$zeropoint" | awk 'BEGIN{FS=","}{print NF}')
    if [ $nzeropoint != 1 ]; then
        echo "$scriptname: a single value for '-Z' or '--zeropoint' should be given."
        exit 1
    fi
    rzero=$zeropoint
    gzero=$zeropoint
    bzero=$zeropoint
    kzero=$zeropoint
fi


# Make sure three weights have been given.
nweights=$(echo "$weights" | awk 'BEGIN{FS=","}{print NF}')
if [ $nweights != 3 ]; then
    echo "$scriptname: 3 weights '-w' or '--weights' shoud be given (comma separated)."
    exit 1
fi


# If the user provides --stretch, make sure it is not equalt o zero (with 8
# decimals), that would crash the asinh transformation.
if [ x$stretch != x ]; then
    stretch_check=$(echo "$stretch" | awk 'BEGIN{FS=","} {printf "%.8f", $1}')
    if [ x$stretch_check = x0.00000000 ]; then
        echo "$scriptname: --stretch value ($stretch) cannot be zero (8 decimals)."
        exit 1
    fi
fi


# If the user provides --qbright, make sure it is not equalt o zero (with 8
# decimals), that would crash the asinh transformation.
if [ x$qbright != x ]; then
    qbright_check=$(echo "$qbright" | awk 'BEGIN{FS=","} {printf "%.8f", $1}')
    if [ x$qbright_check = x0.00000000 ]; then
        echo "$scriptname: --qbright value ($qbright) cannot be zero (8 decimals)."
        exit 1
    fi
fi





# Define the temporal directory
# -----------------------------
#
# Construct the temporal directory. If the user does not specify any
# directory, then a default one with will be constructed.  If the user
# set the directory, then make it. This directory will be deleted at
# the end of the script if the user does not want to keep it (with the
# `--keeptmp' option).
defaulttmpdir="rgb-asinh-tmp"
if [ x$tmpdir = x ]; then tmpdir=$defaulttmpdir; fi
if [ -d $tmpdir ]; then junk=1; else mkdir $tmpdir; fi





# Clip pixel values below the specified limit
# -------------------------------------------
#
# If the user specify a given value below which the pixel values are
# wanted to be zero, then put all of those pixels to zero. By default,
# no clipping will be done (a symbolic link will be put to the input).
rclipped="$tmpdir/r_clipped.fits"
gclipped="$tmpdir/g_clipped.fits"
bclipped="$tmpdir/b_clipped.fits"
kclipped="$tmpdir/k_clipped.fits"
if [ x$rmin = x ]; then
    ln -sf $(realpath $rimage) $rclipped
else
    astarithmetic $rimage --hdu=$rhdu set-i $quiet \
                  i i $rmin lt 0 where --output=$rclipped
    rhdu=1;
fi

if [ x$gmin = x ]; then
    ln -sf $(realpath $gimage) $gclipped
else
    astarithmetic $gimage --hdu=$ghdu set-i $quiet \
                  i i $gmin lt 0 where --output=$gclipped
    ghdu=1;
fi

if [ x$bmin = x ]; then
    ln -sf $(realpath $bimage) $bclipped
else
    astarithmetic $bimage --hdu=$bhdu set-i $quiet \
                  i i $bmin lt 0 where --output=$bclipped
    bhdu=1;
fi

# kclipped is constructed only if a fourth image has been given.
if [ x$kmin = x ] && [ x$kimage != x ]; then
    ln -sf $(realpath $kimage) $kclipped
elif [ x$kimage != x ]; then
    astarithmetic $kimage --hdu=$khdu set-i $quiet \
                  i i $kmin lt 0 where --output=$kclipped
    khdu=1;
fi





# Scale the images: weighting and converting to micro Janskys
# -----------------------------------------------------------
#
# All input images are scaled if different weights of 1 are provided. This
# step allow to easily weight the different bands differently. The
# weighting is done considering the specified weight values and setting the
# maximum value to 1. That means that the image with the highest weighting
# value will be not modified, while the other bands will be scaled
# appropiately. In addition to this, if zero points are provided, the
# images will be transformed to have micro Jansky values, this helps makes
# a more physically meaningful comparison between the filters.
rscaled="$tmpdir/r_scaled.fits"
gscaled="$tmpdir/g_scaled.fits"
bscaled="$tmpdir/b_scaled.fits"

# Initialize all weights to be used to 1.0.
rweight_norm=1.0; gweight_norm=1.0; bweight_norm=1.0; kweight_norm=1.0

# If the user specified different values, then compute them appropiately
if [ x$weights != x"1.0,1.0,1.0" ]; then
    rweight=$(echo "$weights" | awk 'BEGIN{FS=","} {printf "%.8f", $1}')
    gweight=$(echo "$weights" | awk 'BEGIN{FS=","} {printf "%.8f", $2}')
    bweight=$(echo "$weights" | awk 'BEGIN{FS=","} {printf "%.8f", $3}')

    weight_sum=$(astarithmetic $rweight $gweight $bweight 3 sum --type=f32 -q)
    rweight_sum=$(astarithmetic $rweight $weight_sum / -q | awk '{printf "%.8f", $1}')
    gweight_sum=$(astarithmetic $gweight $weight_sum / -q | awk '{printf "%.8f", $1}')
    bweight_sum=$(astarithmetic $bweight $weight_sum / -q | awk '{printf "%.8f", $1}')

    wmax=$(astarithmetic $rweight_sum $gweight_sum $bweight_sum 3 max --type=f32 -q)

    rweight_norm=$(astarithmetic $rweight_sum $wmax / -q | awk '{printf "%.8f", $1}')
    gweight_norm=$(astarithmetic $gweight_sum $wmax / -q | awk '{printf "%.8f", $1}')
    bweight_norm=$(astarithmetic $bweight_sum $wmax / -q | awk '{printf "%.8f", $1}')
fi


# If all weights are 1.0 and no zeropoints are provided, then symbolic link.
if [ x$weights = x"1.0,1.0,1.0" ] && [ x"$rzero" = x ]; then
    ln -sf $(realpath $rclipped) $rscaled
    ln -sf $(realpath $gclipped) $gscaled
    ln -sf $(realpath $bclipped) $bscaled

# If diferent weights but same zeropoints, then scale the images.
elif [ x$weights != x"1.0,1.0,1.0" ] && [ x"$rzero" = x ]; then
    # Just weight the images appropiately
    astarithmetic $rclipped -h$rhdu $rweight_norm x -o$rscaled $quiet
    astarithmetic $gclipped -h$ghdu $gweight_norm x -o$gscaled $quiet
    astarithmetic $bclipped -h$bhdu $bweight_norm x -o$bscaled $quiet
    rhdu=1; ghdu=1; bhdu=1

# Else, use weights and zeropoints to scale and transform to micro Jansky.
else
    # Consider zeropoints as float (8 decimals) numbers to homogeneize
    rzerof=$(echo $rzero | awk '{printf "%.8f", $1}')
    gzerof=$(echo $gzero | awk '{printf "%.8f", $1}')
    bzerof=$(echo $bzero | awk '{printf "%.8f", $1}')

    # Transform to pixel unit to micro Jansky
    astarithmetic $rclipped -h$rhdu $rzerof counts-to-jy 1e6 x \
                  $rweight_norm x --output=$rscaled $quiet
    astarithmetic $gclipped -h$ghdu $gzerof counts-to-jy 1e6 x \
                  $gweight_norm x --output=$gscaled $quiet
    astarithmetic $bclipped -h$bhdu $bzerof counts-to-jy 1e6 x \
                  $bweight_norm x --output=$bscaled $quiet
    rhdu=1; ghdu=1; bhdu=1
fi





# Stacked image: I_RGB = (R+G+B)/3
# --------------------------------
#
# The asinh transformation is done on the mean of RGB images. Here, this
# image is obtained by stacking them using the 'mean' operator. If the
# stacked image has exact zero pixel values, change them to nan values.
I_RGB_stack="$tmpdir/RGB-mean.fits"
astarithmetic $rscaled --hdu=$rhdu \
              $gscaled --hdu=$ghdu \
              $bscaled --hdu=$bhdu 3 mean set-i \
              i i 0 eq nan where \
               --output=$I_RGB_stack $quiet






# Setting 'stretch' and 'qbright' values: guessing or specified values
# --------------------------------------------------------------------
#
# The asinh transformation is done over the stacked RGB image. To do so,
# there are two necessary parameters that are computed here. If the user
# specify the values, then they will be used. If not, they are guessed from
# the median value of the stacked image. If the guessed values are equal to
# zero, then set them to 1.0 because zero value will cause an error in the
# asinh transformation (all pixels become blank). In the case of 'qbright',
# the final guessed value is 10 times the computed median.

# Setting: -s, --stretch
stretch_guessed=$(aststatistics $I_RGB_stack -h1 --median -q)
if [ x$stretch = x"" ]; then
  if [ x$stretch_guessed = x0 ]; then stretch_guessed=1.0; fi
  stretch_value=$stretch_guessed
else
  stretch_value=$stretch
fi

# Setting: -q, --qbright.
qbright_guessed=$(astarithmetic $stretch_guessed 10.0 x -q)
if [ x$qbright = x"" ]; then
  if [ x$qbright_guessed = x0 ]; then qbright_guessed=1.0; fi
  qbright_value=$qbright_guessed
else
  qbright_value=$qbright
fi





# ASINH transformation
# --------------------
#
# Once the necessary parameters have been obtained, here the asinh
# transformation is done over the mean of RGB images. After the
# transformation is done, it is normalized by I_RGB. Finally, the range of
# pixel values is linear transformed to [minvalrange - maxvalrange].
I_RGB_asinh_norm="$tmpdir/RGB-mean-asinh-norm.fits"
astarithmetic $I_RGB_stack -h1 set-I_RGB \
              I_RGB $qbright_value x $stretch_value / set-i \
              i asinh \
              $qbright_value / \
              I_RGB / set-asinhed \
              asinhed --output=$I_RGB_asinh_norm $quiet





# Compute the R, G, B images
# --------------------------
#
# Each input image is multiplied by the transformed and normalized averaged
# image (I_RGB_asinh_norm).
I_R="$tmpdir/R.fits"
I_G="$tmpdir/G.fits"
I_B="$tmpdir/B.fits"
astarithmetic $rscaled --hdu=$rhdu $I_RGB_asinh_norm x $quiet --output=$I_R
astarithmetic $gscaled --hdu=$ghdu $I_RGB_asinh_norm x $quiet --output=$I_G
astarithmetic $bscaled --hdu=$bhdu $I_RGB_asinh_norm x $quiet --output=$I_B





# Normalize by the maximum value of R, G, B
# -----------------------------------------
#
# Compute the maximum of the R, G, B images and divide each image by that
# value. This step is done in order to represent the true color.
I_R_norm="$tmpdir/R_norm.fits"
I_G_norm="$tmpdir/G_norm.fits"
I_B_norm="$tmpdir/B_norm.fits"
maxR=$(aststatistics $I_R --maximum -q)
maxG=$(aststatistics $I_G --maximum -q)
maxB=$(aststatistics $I_B --maximum -q)
maxRGB=$(astarithmetic $maxR float64 $maxG float64 $maxB float64 3 max -q)

astarithmetic $I_R $maxRGB / $maxvalrange x --output=$I_R_norm $quiet
astarithmetic $I_G $maxRGB / $maxvalrange x --output=$I_G_norm $quiet
astarithmetic $I_B $maxRGB / $maxvalrange x --output=$I_B_norm $quiet





# Transformation to change the contrast and brightness
# ----------------------------------------------------
#
# Modify the images that are going to be used for obtaining the COLOR
# image. There are two possible options: linear and NON-linear
# transformation. For the linear transformation, two parameters are used,
# contrast and brightness:
#
#  OUTPUT = contrast * INPUT + brightness
#
# In the NON-linear transformation only one parameter is used, gamma:
#
#  OUTPUT = INPUT**gamma
#
# After this transformation is done, all values are clipped by the
# maxvalrange value. So, all pixels above maxrangevalue become equal to
# maxrangevalue.

I_R_transformed="$tmpdir/R_transformed.fits"
I_G_transformed="$tmpdir/G_transformed.fits"
I_B_transformed="$tmpdir/B_transformed.fits"
if [ x"$gamma" != x"1.0" ]; then
  astarithmetic $I_R_norm $maxvalrange / $gamma pow $maxvalrange x set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_R_transformed $quiet
  astarithmetic $I_G_norm $maxvalrange / $gamma pow $maxvalrange x set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_G_transformed $quiet
  astarithmetic $I_B_norm $maxvalrange / $gamma pow $maxvalrange x set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_B_transformed $quiet

elif [ x"$contrast" != x"1.0" ] || [ x$brightness != x"0.0" ]; then
  astarithmetic $I_R_norm $contrast x $brightness + set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_R_transformed $quiet
  astarithmetic $I_G_norm $contrast x $brightness + set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_G_transformed $quiet
  astarithmetic $I_B_norm $contrast x $brightness + set-t \
                t t $maxvalrange gt $maxvalrange where --output=$I_B_transformed $quiet

else
    ln -sf $(realpath $I_R_norm) $I_R_transformed
    ln -sf $(realpath $I_G_norm) $I_G_transformed
    ln -sf $(realpath $I_B_norm) $I_B_transformed
fi





# If the user wants the gray background image
if [ x$grayback = x1 ]; then

    # Until now, all necessary threshold and parameters have been computed (if
    # the user did not specify any value) from the R,G,B images. The following
    # steps are necessary for constructing the gray background color image. So,
    # low brightness will be showed in gray while high brightness wil be showed
    # in color.

    # Convolve the gray threshold image and set it to 0-100 range values
    # ------------------------------------------------------------------
    #
    # Convolve the image that is going to be used for defining the threshold
    # for splitting the COLOR and GRAY parts. By doing this, instead of having
    # a noised frontier, the border would be more clear. If the user don't want
    # to convolve, just make a symbolic link.
    I_COLORGRAY_threshold="$tmpdir/COLORGRAY_threshold.fits"
    if [ $colorkernelfwhm = 0 ]; then
      # Change pixel values to the wanted range
      astarithmetic $I_RGB_asinh_norm -h1 set-image \
                    image minvalue set-oldmin \
                    image maxvalue set-oldmax \
                    $minvalrange set-newmin \
                    $maxvalrange set-newmax \
                    oldmax oldmin - set-oldrange \
                    newmax newmin - set-newrange \
                    image oldmin - newrange x oldrange / newmin + set-transformed \
                    transformed --output=$I_COLORGRAY_threshold $quiet
    else
      I_COLORGRAY_kernel="$tmpdir/COLORGRAY_kernel.fits"
      astmkprof --kernel=gaussian,$colorkernelfwhm,3 \
                --output=$I_COLORGRAY_kernel $quiet

      I_COLORGRAY_convolved="$tmpdir/COLORGRAY_kernel.fits"
      astconvolve $I_RGB_asinh_norm --hdu=1 --kernel=$I_COLORGRAY_kernel \
                  --domain=spatial --output=$I_COLORGRAY_convolved $quiet

      # Change pixel values to the wanted range
      astarithmetic $I_COLORGRAY_convolved -h1 set-image \
                    image minvalue set-oldmin \
                    image maxvalue set-oldmax \
                    $minvalrange set-newmin \
                    $maxvalrange set-newmax \
                    oldmax oldmin - set-oldrange \
                    newmax newmin - set-newrange \
                    image oldmin - newrange x oldrange / newmin + set-transformed \
                    transformed --output=$I_COLORGRAY_threshold $quiet
    fi





    # Find the COLOR threshold
    # ------------------------
    #
    # The color image with the gray background is constructed by separating the
    # original image into two regimes: COLOR and GRAY. Here, the pixel value
    # used for separate these two regions is computed. It is obtained from the
    # asinh-transformed image. If the user does not provide a value then use
    # ghe computed one (guessed). If the user provide a value, then use it
    # directly. Note that the guessed value is computed in any case.
    colorval_guessed=$(aststatistics $I_COLORGRAY_threshold --median -q)
    if [ x$colorval = x"" ]; then
      colorval=$colorval_guessed
    fi





    # Binary mask for COLOR (1) and GRAY (0) parts
    # --------------------------------------------
    #
    # Here the image is separated into two parts: COLOR (pixels=1) and GRAY
    # (pixels = 0). To obtain this mask, the colorval is considered as the
    # threshold on the asinhed image. So:
    #   asinhed < colorval -->  COLOR = 1
    #   asinhed > colorval -->  GRAY  = 0
    I_COLORGRAY_mask="$tmpdir/COLORGRAY_mask-1color-0gray.fits"
    astarithmetic $I_COLORGRAY_threshold set-ref \
                  ref $colorval lt -o $I_COLORGRAY_mask $quiet





    # Convolve the GRAY background reference image
    # --------------------------------------------
    #
    # Convolve the image that is going to be used for the gray parts. This is
    # done for smoothing the image and increase the signal to noise ratio of
    # the black-white regions.
    if [ x$ninputs = x3 ]; then
      I_GRAY=$I_RGB_stack
      khdu=1
    else
      I_GRAY=$kclipped
    fi

    I_GRAY_convolved="$tmpdir/GRAY_convolved.fits"
    if [ $graykernelfwhm = 0 ]; then
      ln -sf $(realpath $I_GRAY) $I_GRAY_convolved
    else
      I_GRAY_kernel="$tmpdir/GRAY_kernel.fits"
      astmkprof --kernel=gaussian,$graykernelfwhm,3 \
                --oversample=1 --output=$I_GRAY_kernel $quiet

      astconvolve $I_GRAY --hdu=$khdu --kernel=$I_GRAY_kernel \
                  --domain=spatial --output=$I_GRAY_convolved $quiet
    fi





    # Gray background image
    # ---------------------
    #
    # We use the specified gray background image after some modifications to
    # obtain the GRAY part. For futher highlight the interesting diffuse flux,
    # we need to invert it the brigher pixels are darker.
    #
    # Here is the logic of the steps we take:
    #   1. We call the convolved image 'values'.
    #   2. We call the mask image 'mask'.
    #   3. Pixels equal to 1 in 'mask' are set to nan in 'values': 'masked'.
    #   4. The min value of this image is 'oldmin'.
    #   5. The max value of this image is 'oldmax'.
    #   6. The new min value of the final image is 'newmin'.
    #   7. The new max value of the final image is 'newmax'.
    #   (Note that they are equal but inverted of the general range values).
    #   7. The old range (oldmax - oldmin) is 'oldrange'.
    #   8. The new range (newmax - newmin) is 'newrange'.
    #   9. The 'masked' image is transformed to have the new range of values.
    #   (Note that 'grayscale' will transform the pixel value in case of any
    #   function is specified. E.g., log, sqrt, asinh, etc.)
    grayscale=""
    I_GRAY_colormasked="$tmpdir/GRAY_colormasked.fits"
    astarithmetic $I_GRAY_convolved -h1 set-values \
                  $I_COLORGRAY_mask -h1 set-mask \
                  values mask nan where $grayscale set-masked \
                  masked minvalue set-oldmin \
                  masked maxvalue set-oldmax \
                  $maxvalrange set-newmin \
                  $minvalrange set-newmax \
                  oldmax oldmin - set-oldrange \
                  newmax newmin - set-newrange \
                  masked oldmin - newrange x oldrange / newmin + set-transformed \
                  transformed --output=$I_GRAY_colormasked $quiet





    # Find the GRAY thresholds
    # ------------------------
    #
    # Once the COLOR and GRAY parts have been separated, the gray part can be
    # also separated into BLACK and WHITE. To separate these two parts, here a
    # threshold is estimated as the median of the GRAY values.
    I_GRAY_colormasked_clipped="$tmpdir/GRAY_colormasked_clipped.fits"
    grayval_guessed=$(aststatistics $I_GRAY_colormasked --median -q)

    if [ x$grayval = x"" ]; then
      grayval=$grayval_guessed
      ln -sf $(realpath $I_GRAY_colormasked) $I_GRAY_colormasked_clipped
    else
      astarithmetic $I_GRAY_colormasked -h1 set-i \
                    i i $grayval lt $minvalrange where \
                    --output=$I_GRAY_colormasked_clipped
    fi





    # Use color for high S/N and gray for low S/N
    # -------------------------------------------
    #
    # Once the gray-mask has been obtained, use it for replace all pixels less
    # than the gray-threshold with the pixels of the reference image. This is
    # done for each input image (band).  Output images end with -gray.fits,
    # generated into the for loop.
    I_R_transformed_gray="$tmpdir/R_transformed_gray.fits"
    I_G_transformed_gray="$tmpdir/G_transformed_gray.fits"
    I_B_transformed_gray="$tmpdir/B_transformed_gray.fits"
    for f in $I_R_transformed $I_G_transformed $I_B_transformed; do
      outputname=$(echo "$f" | sed -e's/.fits/_gray.fits/')
      astarithmetic $f \
                    $I_GRAY_colormasked_clipped isblank not \
                    $I_GRAY_colormasked_clipped \
                    where -g1 --output=$outputname $quiet;
    done





    # Make the color-gray image
    # -------------------------
    #
    # Once all the previous treatment has been done for each image, then
    # combine all with Convert program to obtain the colored image (with the
    # low S/N regions in gray).
    astconvertt $I_R_transformed_gray -h1 \
                $I_G_transformed_gray -h1 \
                $I_B_transformed_gray -h1 \
                --output=$output $quiet





# If user does not want the gray background image
else

    # Make the color figure
    # ---------------------
    #
    # Once all the previous treatment has been done for each image, then combine
    # all with Convert program to obtain the colored image. Limit fluxes used
    # here correspond to the gray-computed ones in order to obtain exactly the
    # same color as the gray-background image (for those pixels that are not
    # background).
    astconvertt $I_R_transformed -h1 \
                $I_G_transformed -h1 \
                $I_B_transformed -h1 \
                --output=$output $quiet





    # Remove images
    if [ $keeptmp = 0 ]; then
      rm $rscaled $gscaled $bscaled \
         $rclipped $gclipped $bclipped
    fi

fi





# Information to help and warn the user
# -------------------------------------
#
# If the quiet mode is not activated, then print valuable information to
# help the user in guessing appropiate parameters for making the color
# image. In the same way, if the images come with astrometric information
# (WCS) the relative variations in pixel scales and alignment of them are
# provided. This is useful to know if color-gradients on the color image
# could be due to a bad alignment of the images or different pixel scales.

if [ ! x$quiet = x"--quiet" ]; then

  # Print on the command line the statistics of the image and the parameters
  # that are going to be used. This will helps to guess appropiate parameters
  if [ x$checkparams = x1 ]; then
  echo "                   "
  echo "FOR ASINH-TRANSFORMATION ('--strech' and '--qbright' parameters)."
  aststatistics $I_RGB_stack

  echo "                   "
  echo "FOR COLOR-THRESHOLD (separation between color and black, '--colorval' parameter)"
  aststatistics $I_COLORGRAY_threshold

  echo "                   "
  echo "FOR GRAY-THRESHOLD (separation between black and white, '--grayval' parameter)"
  aststatistics $I_GRAY_colormasked
  fi

  cat <<EOF

TIPS:
  # First, use the default options to estimate the parameters.
  # Select a good background value of the images:
      A minimum value of zero could be a good option: '--minimum=0.0'
  # Focus on the bright regions and tweak '--qbright' and '--stretch':
      First, try low values of '--qbright' to show the bright parts.
      Then, adjust '--stretch' to show the fainter regions around bright parts.
      Overall, play with these two parameters to show the color regions appropriately.
  # [next tips only for gray background image: --grayback]
  # Change '--colorval' to separate the color and black regions:
      Increase/decrease it to increase/decrease the color area (brightest pixels).
  # Change '--grayval' to separate the black and gray regions:
      Decrease it to increase the regions that are shown in black.
  # Use '--checkparams'to check the pixel value distributions.

PARAMETERS:
  Estimated: --qbright=$qbright_guessed --stretch=$stretch_guessed --colorval=$colorval_guessed --grayval=$grayval_guessed
  Used     : --qbright=$qbright_value --stretch=$stretch_value --colorval=$colorval --grayval=$grayval

Output written to '$output'.
EOF
fi





# Remove temporal files
# ---------------------
#
# If the user does not specify to keep the temporal files with the option
# `--keeptmp', then remove the whole directory.
if [ $keeptmp = 0 ]; then
    rm -rf $tmpdir
fi





# The script has finished, reset the original language to the system's
# default language.
export LANG=$system_lang


