#
#    kernel/security test suite Makefile
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

include	$(top_srcdir)/include/mk/env_pre.mk

# For broken compilers and toolchains, like Montavista, that improperly detect
# system headers when running autoconf -_-... bleh.
ifeq ($(strip $(CAP_LIBS)),)
FILTER_OUT_DIRS		:= cap_bound filecaps
endif

# selinux-testsuite doesn't compile cleanly, so let the user manually compile
# the testcases for it.
FILTER_OUT_DIRS		+= selinux-testsuite

OPT_CFLAGS		+= -O
DEBUG_LDFLAGS		+= -s

include $(top_srcdir)/include/mk/generic_trunk_target.mk
