#
#    Generic trunk rules include Makefile.
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

#
# generic_trunk_target
#
# Generate a set of recursive targets to apply over a trunk directory (has
# directories) -- optionally with a set of trunk-based files.
#
# All variables in this canned define are essentially the same as
# generic_leaf_target, with the exception that the install flow for local
# targets is:
#
# $(INSTALL_FILES) -> trunk-install -> install (recursive)
#
# All recursive targets are traverse SUBDIRS as defined by the user, or if
# undefined, defaults to any subdirectories where Makefile's are contained
# within.
#
# generic_trunk_target specific variables are:
#
# RECURSIVE_TARGETS		: a list of targets to apply over an entire
# 				  directory tree. This defaults to
# 				  `all install'.
#
# See generic_leaf_target, generic_target_env_setup, and get_make_dirs for
# more details and design notes.
#

include $(top_srcdir)/include/mk/functions.mk

RECURSIVE_TARGETS		?= all install

$(eval $(get_make_dirs))

.PHONY: $(RECURSIVE_TARGETS) $(addprefix trunk-,$(RECURSIVE_TARGETS)) $(foreach tgt,$(RECURSIVE_TARGETS),$(addprefix trunk-subdir-$(tgt)--,$(SUBDIRS)))

$(SUBDIRS): %:
	mkdir -m 00755 -p "$@"

get_trunk_target = $(firstword $(subst --, ,$(patsubst trunk-subdir-%,%,$(1))))
get_trunk_subdir = $(patsubst trunk-subdir-$(call get_trunk_target,$(1))--%,%,$(1))

$(foreach tgt,$(RECURSIVE_TARGETS),$(addprefix trunk-subdir-$(tgt)--,$(SUBDIRS))): %:
	mkdir -m 00755 -p "$(call get_trunk_subdir,$@)"
	$(MAKE) -C $(call get_trunk_subdir,$@) -f "$(abs_srcdir)/$(call get_trunk_subdir,$@)/Makefile" $(call get_trunk_target,$@)

$(MAKE_TARGETS): | $(MAKE_DEPS)

trunk-all: $(MAKE_TARGETS)


trunk-clean:: | $(SUBDIRS)
	$(if $(strip $(CLEAN_TARGETS)),$(RM) -f $(CLEAN_TARGETS))

$(INSTALL_FILES): | $(INSTALL_DEPS)

trunk-install: $(INSTALL_FILES)

# Avoid creating duplicate .PHONY references to all, clean, and install. IIRC,
# I've seen some indeterministic behavior when one does this in the past with
# GNU Make...
.PHONY: $(filter-out $(RECURSIVE_TARGETS),all clean install)
all: trunk-all

clean:: trunk-clean
	set -e; for dir in $(SUBDIRS); do \
	    $(MAKE) -C "$$dir" -f "$(abs_srcdir)/$$dir/Makefile" $@; \
	done
ifneq ($(abs_builddir),$(abs_srcdir))
	$(RM) -Rf $(SUBDIRS)
endif

install: trunk-install

# Print out CURDIR to check for a recursion issue.
ifeq ($(strip $(SUBDIRS)),)
	$(warning CURDIR is: $(CURDIR))
	$(error SUBDIRS empty -- did you want generic_leaf_target instead?)
else
$(RECURSIVE_TARGETS): %: $(addprefix trunk-subdir-%--,$(SUBDIRS))
endif

# vim: syntax=make
