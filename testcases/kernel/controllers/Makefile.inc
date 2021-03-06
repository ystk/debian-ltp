#
#    kernel/controllers test suite Makefile.
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

# DO NOT USE THIS FILE FOR controllers / libcontrollers!!!

LIBDIR			:= libcontrollers
LIB			:= $(LIBDIR)/libcontrollers.a

../$(LIBDIR):
	mkdir -p "$@"

$(LIB): ../$(LIBDIR)
	for i in all install; do \
	    $(MAKE) -C $^ -f "$(abs_srcdir)/$^/Makefile" $$i; \
	done

CPPFLAGS		+= -I$(abs_srcdir)/../$(LIBDIR)

LDFLAGS			+= -L$(abs_builddir)/../$(LIBDIR)

INSTALL_TARGETS		?= *.sh

# vim: syntax=make
