#
#    kernel test suite Makefile.
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

top_srcdir	?= ../..

include $(top_srcdir)/include/mk/env_pre.mk

ifneq ($(UCLINUX),1)
SUBDIRS		:= containers connectors controllers fs io ipc mem numa \
		   performance_counters power_management pty sched security \
		   syscalls timers
else
SUBDIRS		:= syscalls
endif

$(foreach tgt,all install,$(addprefix trunk-subdir-$(tgt)--,$(SUBDIRS))): %: $(includedir)/linux_syscall_numbers.h

trunk-clean:: | linux-syscall-numbers-clean

include:
	mkdir -p "$@"

linux-syscall-numbers-clean:: include
	$(MAKE) -C $^ -f "$(abs_srcdir)/include/Makefile" clean

$(includedir)/linux_syscall_numbers.h: include
	set -e; for i in all install; do \
	    $(MAKE) -C $^ -f "$(abs_srcdir)/$^/Makefile" $$i; \
	done

include $(top_srcdir)/include/mk/generic_trunk_target.mk
