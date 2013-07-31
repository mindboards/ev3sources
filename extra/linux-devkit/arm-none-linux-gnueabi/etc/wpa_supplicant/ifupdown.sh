#!/bin/sh

# Copyright (C) 2006 Debian/Ubuntu wpasupplicant Maintainers 
# <pkg-wpa-devel@lists.alioth.debian.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# On Debian GNU/Linux systems, the text of the GPL license can be
# found in /usr/share/common-licenses/GPL.

if [ -n "$IF_WPA_MAINT_DEBUG" ]; then
	set -x
fi

# quit if we're called for the loopback
if [ "$IFACE" = lo ]; then
	exit 0
fi

# allow wpa_supplicant interface to be specified via wpa-iface
# useful for starting wpa_supplicant on one interface of a bridge
if [ -n "$IF_WPA_IFACE" ]; then
	WPA_IFACE="$IF_WPA_IFACE"
else
	WPA_IFACE="$IFACE"
fi

# source functions
if [ -f /etc/wpa_supplicant/functions.sh ]; then
	. /etc/wpa_supplicant/functions.sh
else
	exit 0
fi

# quit if executables are not installed
if [ ! -x "$WPA_SUP_BIN" ] || [ ! -x "$WPA_CLI_BIN" ]; then
	exit 0
fi

# quit if wpa_action is active
if test_wpa_cli; then
	exit 0
fi

#echo "Initial WPA Checks Pass"

# preliminary sanity checks for roaming daemon
if [ -n "$IF_WPA_ROAM" ]; then
	if [ "$METHOD" != "manual" ]; then
		wpa_msg stderr "wpa-roam can only be used with the \"manual\" inet METHOD"
		exit 1
	fi
	if [ -n "$IF_WPA_MAPPING_SCRIPT" ]; then
		if ! type "$IF_WPA_MAPPING_SCRIPT" >/dev/null; then
			wpa_msg stderr "wpa-mapping-script \"$IF_WPA_MAPPING_SCRIPT\" is not valid"
			exit 1
		fi
	fi
	if [ -n "$IF_WPA_MAPPING_SCRIPT_PRIORITY" ] && [ -z "$IF_WPA_MAPPING_SCRIPT" ]; then
		wpa_msg stderr "\"wpa-mapping-script-priority 1\" is invalid without a wpa-mapping-script"
		exit 1
	fi
	IF_WPA_CONF="$IF_WPA_ROAM"
	WPA_ACTION_SCRIPT="/sbin/wpa_action"
fi

# master function; determines if ifupdown.sh should do something or not
if [ -n "$IF_WPA_CONF" ]; then
	if [ ! -s "$IF_WPA_CONF" ]; then
		wpa_msg stderr "cannot read contents of $IF_WPA_CONF"
		exit 1
	fi	
	WPA_SUP_CONF_CTRL_DIR=$(sed -n \
		's/[[:space:]]*#.*//g;s/[[:space:]]\+.*$//g;s/^\(ctrl_interface\|DIR\)=\(.*\)/\2/p' "$IF_WPA_CONF")
	if [ -n "$WPA_SUP_CONF_CTRL_DIR" ]; then
		WPA_CTRL_DIR="$WPA_SUP_CONF_CTRL_DIR"
	fi
	WPA_SUP_CONF="-c $IF_WPA_CONF -C $WPA_CTRL_DIR"
elif set | grep -q "^IF_WPA"; then
	WPA_SUP_CONF="-C $WPA_CTRL_DIR"
else
	exit 0
fi

case "$MODE" in 
	start)
		PHASE="pre-up"
		echo "WPA: Configuring Interface"
		case "$PHASE" in
			pre-up)
				kill_wpa_supplicant
				init_wpa_supplicant	|| echo "FAILED"
				conf_wpa_supplicant 	|| { kill_wpa_supplicant; exit 1; }
				;;
			post-up)
				init_wpa_cli 		|| { kill_wpa_supplicant; exit 1; }
				;;
		esac
		;;

	stop)
		PHASE="post-down"
		echo "WPA: Terminating"
		case "$PHASE" in
			pre-down)
				kill_wpa_cli
				;;
			post-down)
				kill_wpa_supplicant
				;;
			*)
				wpa_msg stderr "unknown phase: \"$PHASE\""
				exit 1
				;;
		esac
		;;
	
	*)
		wpa_msg stderr "unknown mode: \"$MODE\""
		exit 1
		;;
esac

exit 0
