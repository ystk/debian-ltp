#
#    commands/unzip testcases Makefile.
#
#    Copyright (C) 2009, Cisco Systems Inc.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Garrett Cooper, July 2009
#

top_srcdir		?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

# Only build and install if we have the zip command.
HAVE_ZIPTOOLS		:= $(shell if which zip >/dev/null; then echo yes; else echo no; fi)

ZIP_DIR			:= tst_unzip.dir

RM			+= -r

TESTCASES_DATA_DIR	:= $(DESTDIR)/$(prefix)/testcases/data

ZIPFILE			:= $(TESTCASES_DATA_DIR)/tst_unzip_file.zip

ifeq ($(HAVE_ZIPTOOLS),yes)
CLEAN_TARGETS		:= $(ZIP_DIR) $(ZIPFILE)
INSTALL_TARGETS		:= unzip_tests.sh
endif

INSTALL_DEPS		+= $(ZIPFILE)


$(TESTCASES_DATA_DIR):
	mkdir -p "$@"

$(ZIPFILE): | $(TESTCASES_DATA_DIR) $(ZIP_DIR)
	zip -r "$@" $(ZIP_DIR)
	chmod 00666 "$@"

# This target should be auto-removed by Make
$(ZIP_DIR): $(abs_srcdir)/unzip_genfile.sh
	$(SHELL) "$^" $(ZIP_DIR)

include $(top_srcdir)/include/mk/generic_leaf_target.mk

INSTALL_TARGETS		:= $(ZIPFILE)
