# Estimate the zero point of an image from a reference (image or catalog)
#
# This Makefile should not be used independently. It will be called from
# the Zeropoint script.
#
# NOTE ON FORMAT OF RECIPES: Non-GNU implementations of Make don't have
# '.ONESHELL', so without connecting the recipe lines with a '\', they will
# be executed in separate shells (which will not preserve variable
# values). Therefore, we cannot use comments in between the separate
# commands.
#
# Current maintainer:
#     2022-2023 Sepideh Eskandarlou <sepideh.eskandarlou@gmail.com>
# Contributing authors:
#     2019-2023 Mohammad Akhlaghi <mohammad@akhlaghi.org>
#     2019-2023 Raul Infante-Sainz <infantesainz@gmail.com>
#     2019-2022 Samane Raji <samaneraji@gmail.com>
#     2019-2022 Zahra sharbaf <zahra.sharbaf2@gmail.com>
# Copyright (C) 2022-2023 Free Software Foundation, Inc.
#
# This Makefile is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This Makefile is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this Makefile.  If not, see <http://www.gnu.org/licenses/>.  Set
# input & Final target






# Final target.
all: final

# Second expansion.
.SECONDEXPANSION:

# Stop the recipe's shell if the command fails
.SHELLFLAGS = -ec

# Include the configure file.
include $(tmpdir)/zeropoint.conf





# Build the main directory for saving the outputs.
$(tmpdir):; mkdir $@





# Catalog of stars
# ----------------
#
# Use Gaia catalog and only keep the objects with good parallax (to
# confirm that they are stars).
stars=$(tmpdir)/stars.fits
$(stars): $(input) | $(tmpdir)

# Recipe if 'starcat' is NOT given (get it from Gaia)
ifeq ($(strip $(starcat)),)
	raw=$(subst .fits,-raw.fits,$@); \
	astquery gaia --dataset=dr3 \
	         --overlapwith=$(input) \
	         -csource_id -cra -cdec -cparallax \
	         -cparallax_error -cpmra -cpmdec --output=$$raw; \
	asttable $$raw -cra,dec --colinfoinstdout \
	         -c'arith parallax parallax abs \
	                  parallax_error 3 x lt nan where ' \
	         --colmetadata=3,GOODPLX,int32,"Stars with good parallax." \
	         --noblankend=GOODPLX \
	         | asttable -cra,dec --output=$@; \
	rm $$raw

# Recipe if 'starcat' is given (just use the 'ra' and 'dec'
# columns). If the input doesn't have an 'ra' or 'dec' columns,
# 'asttable' is going to complain directly. So there is no need to add
# extra checks here.
else
	if [ "x$(starcathdu)" = x ]; then hdu=1; \
	else hdu=$(starcathdu); fi; \
	asttable $(starcat) --hdu=$$hdu --output=$@ \
	         -c$(starcatra),$(starcatdec)
endif





# If the reference type is 'img', 'gencat' will be a list with many names
# as the number of input images.
ifeq ($(reftype),img)
gencat=$(foreach i, $(refnumber), ref$(i))

# The reference is a catalog, prepare the single reference catalog with
# desired columns.
else
gencat=

# Prepare the catalog for comparing in different appertures.
cataper=$(foreach a,$(aper-arcsec), \
         $(tmpdir)/ref1-$(a)-cat.fits)
$(cataper): $(tmpdir)/ref1-%-cat.fits:

	asttable $(ref1) -c$(ra),$(dec) -c$(mag) \
	         | cat -n \
	         | asttable --output=$@ \
	               --colmetadata=1,OBJ_ID,int32,"Id of object." \
	               --colmetadata=2,RA,float64,"Right Assencion." \
	               --colmetadata=3,DEC,float64,"Declination." \
	               --colmetadata=4,MAGNITUDE,float32,"Magnitude."
endif





# Apertures photometry
# --------------------
#
# To generate the apertures catalog we will use Gnuastro’s MakeProfiles. We
# will first read the positions from the Gaia catalog, then use AWK to set
# the other parameters of each profile to be a fixed circle of radius 5.1
# pixels. To calculate the pixel size I have to use the big origin image
# before cropping
zpinput=0
aperture=$(foreach i,input $(gencat), \
          $(foreach a,$(aper-arcsec), \
           $(tmpdir)/$(i)-$(a)-cat.fits))
$(aperture): $(tmpdir)/%-cat.fits: $(stars)

#	Brief summary of the steps done here:
#	   - Extract the names.
#	   - Convert the aperture size (arcsec) to pixels.
#	   - Make an aperture catalog by using aperture size in pixels
#	   - Make an image of apertures.
#	   - Build a catalog of this aperture image.
#	   - Clean up.
	img=$($(word 1, $(subst -, ,$*))); \
	zp=$(zp$(word 1, $(subst -, ,$*))); \
	aperarcsec=$(word 2, $(subst -, ,$*)); \
	hdu=$(hdu$(word 1, $(subst -, ,$*))); \
	aperwcscat=$(subst .fits,-aperwcscat.txt,$@); \
	aperimg=$(subst .fits,-aper.fits,$@); \
	aperpix=$$(astfits $$img --hdu=$$hdu --pixelscale --quiet \
	                   | awk -v s=$$aperarcsec \
	                         '{print s/($$1 * 3600)}'); \
	asttable $(stars) -cra,dec \
	         | awk -v r=$$aperpix \
	               '{ print NR, $$1, $$2, 5, r, 0, 0, 1, NR, 1 }' \
	         > $$aperwcscat; \
	astmkprof $$aperwcscat --background=$$img --backhdu=$$hdu \
	          --clearcanvas --replace --type=int32 --mforflatpix \
	          --mode=wcs --output=$$aperimg --quiet; \
	astmkcatalog $$aperimg -h1 --output=$@ --zeropoint=$$zp \
	             --inbetweenints --valuesfile=$$img \
	             --valueshdu=$$hdu --ids --ra --dec --magnitude; \
	rm $$aperwcscat $$aperimg





# Calculate magnitude differences
# -------------------------------
#
# Match the reference catalog with the input catalog and put reference
# magnitude in the catalog. Then subtract the reference mag. from input
# mag. Finally, the final target has two columns of reference mag and
# subtracted mag.
allrefs=$(foreach i, $(refnumber), ref$(i))
magdiff=$(foreach r,$(allrefs), \
         $(foreach a,$(aper-arcsec), \
          $(tmpdir)/$(r)-$(a)-magdiff.fits))
$(magdiff): $(tmpdir)/%-magdiff.fits: \
            $(tmpdir)/%-cat.fits \
            $(tmpdir)/input-$$(word 2,$$(subst -, ,%))-cat.fits

#	Find the matching objects in both catalogs.
#	Subtract the reference catalog mag from input catalog's mag.
#	Clean up.
	match=$(subst .fits,-match.fits,$@); \
	astmatch $< --hdu=1 $(word 2,$^) --hdu2=1 \
	         --ccol1=RA,DEC --ccol2=RA,DEC \
	         --outcols=aMAGNITUDE,bMAGNITUDE \
	         --aperture=$(matchradius) \
	         --output=$$match; \
	asttable $$match -c1 -c'arith $$1 $$2 -' \
	         --colmetadat=1,MAG-REF,f32,"Magnitude of reference." \
                 --colmetadat=2,MAG-DIFF,f32,"Magnitude diff with input." \
	         --noblankend=1,2 --output=$@; \
	rm $$match





# Computing the zero point for each aperture
# ------------------------------------------
#
# Calculate Zeropoint number in seperated files and calculate the root mean
# square of zero point.
aperzeropoint=$(foreach a,$(aper-arcsec), \
                $(tmpdir)/zeropoint-$(a).txt)
$(aperzeropoint): $(tmpdir)/zeropoint-%.txt: \
                  $$(foreach r,$(allrefs),$(tmpdir)/$$(r)-%-magdiff.fits)

#	Merge all the magdiffs from all the references.
#	Merge all the rows from all the reference images into one.
#	If the user requested a certain magnitude range, use it.
#	Find the zeropoint and its standard deviationg using sigma-clipped
#	median and standard deviation. Write them into the target.
	merged=$(subst .txt,-merged.fits,$@); \
	opts=""; \
	if ! [ "$(refnumber)" = 1 ]; then \
	  for r in $$(echo $(refnumber) | sed -e's|1||'); do \
	    opts="$$opts --catrowfile=$(tmpdir)/ref$$r-$*-magdiff.fits"; \
	  done; \
	fi; \
	asttable $(tmpdir)/ref1-$*-magdiff.fits $$opts -o$$merged; \
	astfits $$merged --update=EXTNAME,APER-$*; \
	rangeopt=""; \
	if ! [ x"$(magrange)" = x ]; then \
	  rangeopt="--range=MAG-REF,$(magrange)"; \
	fi; \
	zpstd=$$(asttable $$merged $$rangeopt -cMAG-DIFF \
	                  | aststatistics --sigclip-median \
	                                  --sigclip-std --quiet); \
	echo "$* $$zpstd" > $@





# Most accurate zeropoint
# -----------------------
#
# For each aperture, one zeropoint and its STD has been computed. The best
# value is the one with the lowest STD value.
zeropoint=$(output)
$(zeropoint): $(aperzeropoint)

#	Obtain the zeropoint and zero point STD of each aperture.
#	Find the best aperture; its zero point and STD.
#	Auxiliary/temporary file
#	If the user requested a certain magnitude range, add minmag and
#       maxmag to header.
#	   The 'bestaper' above is returned from 'asttable', that is saved
#	   as a floating point, so the extra digits in reading floating
#	   points
#	   Move the main table to the output and copy the mag-vs-zeroppoint
#	   plot for the best aperture.
#	  Move the main table to the output and copy the mag-vs-zeroppoint
#	  plot for the whole aperture.
#	Clean up.
	zp=$(subst .fits,-tmp.txt,$@); \
	echo "# Column 1: APERTURE  [arcsec,f32,] Aperture used."       > $$zp; \
	echo "# Column 2: ZEROPOINT [mag,   f32,] Zero point (sig-clip median)." >> $$zp; \
	echo "# Column 3: ZPSTD     [mag,   f32,] Zero point Standard deviation." \
	     >> $$zp; \
	for a in $(aper-arcsec); do \
	  cat $(tmpdir)/zeropoint-$$a.txt    >> $$zp; \
	done; \
	magmin=""; \
	magmax=""; \
	if ! [ x"$(magrange)" = x ]; then \
	  magmin=$$(echo "$(magrange)" | sed 's|,| |' | awk '{print $$1}'); \
	  magmax=$$(echo "$(magrange)" | sed 's|,| |' | awk '{print $$2}'); \
	fi; \
	asttable $$zp --output=$@.fits; \
	bestaper=$$(asttable $$zp --sort=ZPSTD --head=1 --column=APERTURE); \
	bestzp=$$(asttable $$zp --sort=ZPSTD --head=1  --column=ZEROPOINT); \
	beststd=$$(asttable $$zp --sort=ZPSTD --head=1  --column=ZPSTD); \
	astfits $@.fits --update=EXTNAME,"ZEROPOINTS" \
	                --write=/,"Zeropoint properties" \
	                --write=ZPAPER,"$$bestaper","Best aperture." \
	                --write=ZPVALUE,"$$bestzp","Best zero point." \
	                --write=ZPSTD,"$$beststd","Best std. dev. of zeropoint."; \
	if ! [ x"$(magrange)" = x ]; then \
	  astfits $@.fits \
	          --write=ZPMAGMIN,"$$magmin","Min mag for obtaining zeropoint."; \
	  astfits $@.fits \
	          --write=ZPMAGMAX,"$$magmax","Max mag for obtaining zeropoint."; \
	fi; \
	if [ x"$(keepzpap)" = x ]; then \
	  for a in $(aper-arcsec); do \
	    check=$$(echo $$a \
	                  | awk -vb=$$bestaper \
	                        '$$1>b-1e-6 && $$1<b+1e-6{print "yes"}'); \
	    if [ x$$check = xyes ]; then bestaperstr=$$a; fi; \
	  done; \
	  astfits $(tmpdir)/zeropoint-$$bestaperstr-merged.fits --copy=1 -o$@.fits; \
	  mv $@.fits $@; \
	else \
	  for a in $(aper-arcsec); do \
	      astfits $(tmpdir)/zeropoint-$$a-merged.fits --copy=1 -o$@.fits; \
	  done; \
	  mv $@.fits $@; \
	fi; \
	rm $$zp





# Final target.
final: $(zeropoint)
