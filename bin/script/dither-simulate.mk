# Makefile to do the number-crunching of the 'dither-simulate.in' script.
#
# Copyright (C) 2023-2023 Mohammad Akhlaghi <mohammad@akhlaghi.org>
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

# Include the parameters from the user.
include $(tmpdir)/dither-simulate.conf

# Final target.
all: $(output)

# Second expansion.
.SECONDEXPANSION:

# Stop the recipe's shell if the command fails
.SHELLFLAGS = -ec




# Build each separate exposure.
exposures=$(foreach d,$(dithers),$(tmpdir)/exp-$(d).fits)
$(exposures): $(tmpdir)/exp-%.fits: $(img) | $(tmpdir)

#	Copy the input into a temporary one and edit its keywords to adjust
#	to this pointings position.
	copy=$(subst .fits,-copy.fits,$@); \
	warped=$(subst .fits,-warped.fits,$@); \
	const=$(subst .fits,-constant.fits,$@); \
	astfits $(img) --copy=$(imghdu) --output=$$copy; \
	astfits --update=CRVAL1,$($*-ra) $$copy \
	        --update=CRVAL2,$($*-dec); \
	astarithmetic $$copy 1 uint8 constant --output=$$const; \
	rm $$copy; \
	astwarp $$const --center=$(center) --width=$(width) \
	        $(widthinpix) --output=$$warped; \
	rm $$const; \
	astarithmetic $$warped isnotblank -o$@; \
	rm $$warped





# Build the stack
$(output): $(exposures)
	astarithmetic $(exposures) $(words $(exposures)) \
	              sum -g1 --output=$@
