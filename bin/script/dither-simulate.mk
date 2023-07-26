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





# Hooks preparation:
#   - For the check in the shell (to see if the hook is activated or not),
#     it is easier to work with a single word (hence why '$(word 1) is
#     being used).
#   - Shell scripts can have double-quotations, which can intefere with the
#     double quotations within the recipe.
hook-warp-after-check=$(subst \",,$(word 1,$(hook-warp-after)))
hook-warp-before-check=$(subst \",,$(word 1,$(hook-warp-before)))





# Build each separate exposure.
exposures=$(foreach d,$(dithers),$(tmpdir)/exp-$(d).fits)
$(exposures): $(tmpdir)/exp-%.fits: $(img) | $(tmpdir)

#	Copy the input into a temporary one and edit its keywords to adjust
#	to this pointings position.
#	  - 'TARGET' is defined for hooks (who don't see Make's '$@').
#	  - After each hook, we need to make sure the necessary file for the
#	    next step has been created.
	@TARGET=$@; \
	copy=$(subst .fits,-copy.fits,$@); \
	WARPED=$(subst .fits,-warped.fits,$@); \
	TOWARP=$(subst .fits,-to-warp.fits,$@); \
	EXPOSURE=$(subst .fits,-exposure.fits,$@); \
	astfits $(img) --copy=$(imghdu) --output=$$copy $(quiet); \
	astfits --update=CRVAL1,$($*-ra) $$copy \
	        --update=CRVAL2,$($*-dec) $(quiet); \
	astarithmetic $$copy 1 uint8 constant --output=$$EXPOSURE \
	              $(quiet); \
	rm $$copy; \
	if [ x$(hook-warp-before-check) = x ]; then \
	  cp $$EXPOSURE $$TOWARP; \
	else \
	  eval "$(hook-warp-before)"; \
	  if ! [ -f $$TOWARP ]; then \
	    echo "$(scriptname): command given to '--hook-warp-before' did not create the required input for the next step. Please make sure that the final output of the command given to this hoook is called as '\$$TOWARP' in your command. See the documentation and tutorials for more"; \
	    exit 1; \
	  fi; \
	fi; \
	astwarp $$TOWARP --ctype=$(ctype) --center=$(center) \
	        $(quiet) --width=$(width) --output=$$WARPED \
	        $(widthinpix); \
	if [ x$(hook-warp-after-check) = x ]; then \
	  astarithmetic $$WARPED isnotblank -o$$TARGET $(quiet); \
	else \
	  eval "$(hook-warp-after)"; \
	  if ! [ -f $$TARGET ]; then \
	    echo "$(scriptname): command given to '--hook-warp-after' did not create the required input for the next step. Please make sure that the final output of the command given to this hoook is called as \$$TARGET  in your command. See the documentation and tutorials for more"; \
	    exit 1; \
	  fi; \
	fi; \
	rm $$WARPED $$TOWARP $$EXPOSURE





# Build the stack
$(output): $(exposures)
	astarithmetic $(exposures) $(words $(exposures)) \
	               $(stack-operator) -g1 --output=$@ $(quiet)
