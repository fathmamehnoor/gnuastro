## Process this file with automake to produce Makefile.inx
##
## Original author:
##     Mohammad Akhlaghi <mohammad@akhlaghi.org>
## Contributing author(s):
## Copyright (C) 2019-2024 Free Software Foundation, Inc.
##
## Gnuastro is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Gnuastro is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Gnuastro. If not, see <http://www.gnu.org/licenses/>.





# Data files (necessary for various components like desktop files for GUIs
# or Makefiles that are called within scripts).
pkgdata_DATA = zeropoint.mk \
               pointing-simulate.mk \
               astscript-fits-view.desktop
astscript-fits-view.desktop: $(srcdir)/fits-view.desktop.desktop Makefile
	sed -e 's|@PREFIX[@]|$(exec_prefix)|g' \
	    $(srcdir)/fits-view.desktop.desktop > $@





## List of programs (scripts in this directory) to install under the
## 'prefix/bin' directory ('bin_SCRIPTS'), files necessary to distribute
## with the tarball ('EXTRA_DIST') and output files (to be cleaned with
## 'make clean').
bin_SCRIPTS = astscript-fits-view \
              astscript-psf-unite \
              astscript-psf-stamp \
              astscript-zeropoint \
              astscript-ds9-region \
              astscript-psf-subtract \
              astscript-sort-by-night \
              astscript-radial-profile \
              astscript-color-faint-gray \
              astscript-psf-scale-factor \
              astscript-psf-select-stars \
              astscript-pointing-simulate

EXTRA_DIST = fits-view.sh \
             psf-unite.sh \
             psf-stamp.sh \
             zeropoint.sh \
             zeropoint.mk \
             ds9-region.sh \
             psf-subtract.sh \
             sort-by-night.sh \
             radial-profile.sh \
             color-faint-gray.sh \
             psf-select-stars.sh \
             psf-scale-factor.sh \
             pointing-simulate.sh \
             pointing-simulate.mk \
             fits-view.desktop.desktop

CLEANFILES = $(bin_SCRIPTS) \
             astscript-fits-view.desktop





## Command to do basic substitutions (anything surrounded by an '@').
do_subst = sed -e 's,[@]PREFIX[@],$(exec_prefix),g' \
               -e 's,[@]VERSION[@],$(VERSION),g' \
               -e 's,[@]SCRIPT_NAME[@],$@,g'






## Rules to install the scripts.
astscript-color-faint-gray: color-faint-gray.sh Makefile
	$(do_subst) < $(srcdir)/color-faint-gray.sh > $@
	chmod +x $@

astscript-ds9-region: ds9-region.sh Makefile
	$(do_subst) < $(srcdir)/ds9-region.sh > $@
	chmod +x $@

astscript-fits-view: fits-view.sh Makefile
	$(do_subst) < $(srcdir)/fits-view.sh > $@
	chmod +x $@

astscript-pointing-simulate: pointing-simulate.sh Makefile
	$(do_subst) < $(srcdir)/pointing-simulate.sh > $@
	chmod +x $@

astscript-psf-unite: psf-unite.sh Makefile
	$(do_subst) < $(srcdir)/psf-unite.sh > $@
	chmod +x $@

astscript-psf-stamp: psf-stamp.sh Makefile
	$(do_subst) < $(srcdir)/psf-stamp.sh > $@
	chmod +x $@

astscript-psf-scale-factor: psf-scale-factor.sh Makefile
	$(do_subst) < $(srcdir)/psf-scale-factor.sh > $@
	chmod +x $@

astscript-psf-select-stars: psf-select-stars.sh Makefile
	$(do_subst) < $(srcdir)/psf-select-stars.sh > $@
	chmod +x $@

astscript-psf-subtract: psf-subtract.sh Makefile
	$(do_subst) < $(srcdir)/psf-subtract.sh > $@
	chmod +x $@

astscript-radial-profile: radial-profile.sh Makefile
	$(do_subst) < $(srcdir)/radial-profile.sh > $@
	chmod +x $@

astscript-sort-by-night: sort-by-night.sh Makefile
	$(do_subst) < $(srcdir)/sort-by-night.sh > $@
	chmod +x $@

astscript-zeropoint: zeropoint.sh Makefile
	$(do_subst) < $(srcdir)/zeropoint.sh > $@
	chmod +x $@
