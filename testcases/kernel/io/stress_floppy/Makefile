#
#  Copyright (c) International Business Machines  Corp., 2001
#
#  This program is free software;  you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY;  without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
#  the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program;  if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

top_srcdir		?= ../../../..

include $(top_srcdir)/include/mk/testcases.mk

CLEAN_TARGETS		:= 10*K_file dumpdir

INSTALL_TARGETS		:= 10*K_file stress_floppy dd_file dumpdir/*

INSTALL_MODE = $(if $(filter %stress_floppy,$@), 0755, 0644)

all: dumpdir

# dumpdir's a directory.
RM			+= -r

dumpdir:
	@mkdir -p "$@"
	@$(SHELL) "$(abs_srcdir)/generate.sh"

include $(top_srcdir)/include/mk/generic_leaf_target.mk
