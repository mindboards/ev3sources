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

#####################################################################
## global variables
# wpa_supplicant variables
WPA_SUP_BIN="/usr/sbin/wpa_supplicant"
WPA_SUP_PNAME="wpa_supplicant"
WPA_SUP_PIDFILE="/var/run/wpa_supplicant.$WPA_IFACE.pid"

# wpa_cli variables
WPA_CLI_BIN="/usr/sbin/wpa_cli"
WPA_CLI_PNAME="wpa_cli"
WPA_CLI_PIDFILE="/var/run/wpa_action.$WPA_IFACE.pid"
WPA_CLI_LOGFILE="/var/log/wpa_action.log"
WPA_CLI_TIMESTAMP="/var/run/wpa_action.$WPA_IFACE.timestamp"

# default ctrl_interface socket directory
if [ -z "$WPA_CTRL_DIR" ]; then
	WPA_CTRL_DIR="/var/run/wpa_supplicant"
fi

# verbosity variables
if [ -n "$IF_WPA_VERBOSITY" ] || [ "$VERBOSITY" = "1" ]; then
	TO_NULL="/dev/stdout"
	DAEMON_VERBOSITY="--verbose"
else
	TO_NULL="/dev/null"
	DAEMON_VERBOSITY="--quiet"
fi

#####################################################################
## wpa_cli wrapper
# Path to common ctrl_interface socket and iface supplied.
# NB: WPA_CTRL_DIR cannot be used for interactive commands.
#
wpa_cli () {
	$WPA_CLI_BIN -p $WPA_CTRL_DIR -i $WPA_IFACE "$@"
}

#####################################################################
## verbose and stderr message wrapper
# Ensures a standard and easily identifiable message is printed by
# scripts using this function library.
#
# verbose	To stdout when IF_WPA_VERBOSITY or VERBOSITY is true
#
# action	Same as verbose but without newline
#		Useful for allowing wpa_cli commands to echo result
#		value of 'OK' or 'FAILED'
#
# stderr	Echo warning or error messages to stderr
#
# NB: when called by wpa_action, there is no redirection (verbose)
#
wpa_msg () {
	
	if [ -n "$WPA_ACTION" ]; then
		shift
		echo "wpa_action: $@"
		return
	fi
	
	case "$1" in 
		"verbose")
			shift
			echo "$WPA_SUP_PNAME: $@" >$TO_NULL
			;;
		"action")
			shift
			echo -n "$WPA_SUP_PNAME: $@ -- " >$TO_NULL
			;;
		"stderr")
			shift
			echo "$WPA_SUP_PNAME: $@" >/dev/stderr
			;;
		*)
			;;
	esac
}

#####################################################################
## validate daemon pid files
# Test daemon process ID files via start-stop-daemon with a signal 0
# given the exec binary and pidfile location.
#
# $1	daemon
# $2	pidfile
#
# Returns true when pidfile exists, the process ID exists _and_ was
# created by the exec binary.
#
# If the test fails, but the pidfile exists, it is stale
#
test_daemon_pidfile () {
	local DAEMON PIDFILE
	
	if [ -n "$1" ]; then
		DAEMON="$1"
	fi
	
	if [ -f "$2" ]; then
		PIDFILE="$2"
	fi
	
	if [ -n "$DAEMON" ] && [ -f "$PIDFILE" ]; then
		if start-stop-daemon --stop --quiet --signal 0 \
			--exec "$DAEMON" --pidfile "$PIDFILE"; then
			return 0
		else
			rm -f "$PIDFILE"
			return 1
		fi
	else
		return 1
	fi
}

# validate wpa_supplicant pidfile
test_wpa_supplicant () {
	test_daemon_pidfile "$WPA_SUP_BIN" "$WPA_SUP_PIDFILE"
}

# validate wpa_cli pidfile
test_wpa_cli () {
	test_daemon_pidfile "$WPA_CLI_BIN" "$WPA_CLI_PIDFILE"
}

#####################################################################
## daemonize wpa_supplicant
# Start wpa_supplicant via start-stop-dameon with all required
# options. Will start if environment variable WPA_SUP_CONF is present
#
# Required options:
# -B	dameonize/background process
# -D	driver backend ('wext' if none given)
# -P	process ID file
# -C	path to ctrl_interface socket directory
#
# Optional options:
# -c	configuration file
# -W	wait for wpa_cli to attach to ctrl_interface socket
# -b	bridge interface name
#
init_wpa_supplicant () {
	if [ -n "$WPA_SUP_CONF" ]; then
		# wpa-action was removed, point to wpa-roam
		if [ -n "$IF_WPA_ACTION" ]; then
			wpa_msg stderr "wpa-action support has been removed"
			wpa_msg stderr "refer to /usr/share/doc/wpasupplicant/README.modes.gz"
			return 1
		fi

		local WPA_SUP_DRIVER WPA_SUP_OPTIONS

		if [ -n "$WPA_ACTION_SCRIPT" ]; then
			if [ -x "$WPA_ACTION_SCRIPT" ]; then
				WPA_SUP_OPTIONS="-W -B -P $WPA_SUP_PIDFILE -i $WPA_IFACE"
				wpa_msg verbose "wait for wpa_cli to attach"
			else
				wpa_msg stderr "action script \"$WPA_ACTION_SCRIPT\" not executable"
				return 1
			fi
		else
			WPA_SUP_OPTIONS="-B -P $WPA_SUP_PIDFILE -i $WPA_IFACE"
		fi

		if [ -n "$IF_WPA_BRIDGE" ]; then
			WPA_SUP_OPTIONS="$WPA_SUP_OPTIONS -b $IF_WPA_BRIDGE"
			wpa_msg verbose "wpa-bridge $IF_WPA_BRIDGE"
		fi

		if [ -n "$IF_WPA_DRIVER" ]; then
			WPA_SUP_DRIVER="$IF_WPA_DRIVER"
			wpa_msg verbose "wpa-driver $WPA_SUP_DRIVER"
		else
			WPA_SUP_DRIVER="wext"
			wpa_msg verbose "using default driver type: wpa-driver $WPA_SUP_DRIVER"
		fi

		wpa_msg verbose "$WPA_SUP_BIN $WPA_SUP_OPTIONS -D $WPA_SUP_DRIVER $WPA_SUP_CONF"
			
		start-stop-daemon --start --oknodo $DAEMON_VERBOSITY \
			--name $WPA_SUP_PNAME --startas $WPA_SUP_BIN --pidfile $WPA_SUP_PIDFILE \
			-- $WPA_SUP_OPTIONS -D $WPA_SUP_DRIVER $WPA_SUP_CONF

		if [ "$?" != "0" ]; then
			wpa_msg stderr "$WPA_SUP_BIN daemon failed to start"
			return 1
		fi
		
		local WPA_SOCKET_WAIT MAX_WPA_SOCKET_WAIT
		WPA_SOCKET_WAIT="0"
		MAX_WPA_SOCKET_WAIT="5"
		until [ -S "$WPA_CTRL_DIR/$WPA_IFACE" ]; do
			if [ "$WPA_SOCKET_WAIT" -ge "$MAX_WPA_SOCKET_WAIT" ]; then
				wpa_msg stderr "ctrl_interface socket not found at $WPA_CTRL_DIR/$WPA_IFACE"
				return 1
			else
				wpa_msg verbose "waiting for \"$WPA_CTRL_DIR/$WPA_IFACE\": $WPA_SOCKET_WAIT (max. $MAX_WPA_SOCKET_WAIT)"
			fi
			
			WPA_SOCKET_WAIT=$(($WPA_SOCKET_WAIT + 1))
			sleep 1
		done
		
		wpa_msg verbose "ctrl_interface socket located at $WPA_CTRL_DIR/$WPA_IFACE"
	else
		wpa_msg stderr "init_wpa_supplicant() called without WPA_SUP_CONF"
		return 1
	fi
}

#####################################################################
## stop wpa_supplicant process
# Kill wpa_supplicant via start-stop-daemon, given the location of
# the pidfile or ctrl_interface socket path and interface name
#
kill_wpa_supplicant () {
	if test_wpa_supplicant; then
		
		wpa_msg verbose "terminating $WPA_SUP_PNAME daemon via pidfile $WPA_SUP_PIDFILE"

		start-stop-daemon --stop --oknodo $DAEMON_VERBOSITY \
			--exec $WPA_SUP_BIN --pidfile $WPA_SUP_PIDFILE
	
		if [ -f "$WPA_SUP_PIDFILE" ]; then
			rm -f "$WPA_SUP_PIDFILE"
		fi
	elif [ -S "$WPA_CTRL_DIR/$WPA_IFACE" ]; then

		wpa_msg action "terminating via ctrl_interface socket $WPA_CTRL_DIR/$WPA_IFACE"

		wpa_cli terminate >$TO_NULL
		
		if [ -S "$WPA_CTRL_DIR/$WPA_IFACE" ]; then
			rm -f "$WPA_CTRL_DIR/$WPA_IFACE"
		fi
	fi
}

#####################################################################
## reload wpa_supplicant process
# Sending a HUP signal causes wpa_supplicant to reparse its
# configuration file
#
reload_wpa_supplicant () {
	if test_wpa_supplicant; then
		wpa_msg verbose "reloading wpa_supplicant configuration file via HUP signal"
		start-stop-daemon --stop --signal HUP \
			--name "$WPA_SUP_PNAME" --pidfile "$WPA_SUP_PIDFILE"
	else
		wpa_msg verbose "cannot $WPA_ACTION, $WPA_SUP_PIDFILE does not exist"
	fi
}

#####################################################################
## daemonize wpa_cli and action script
# If environment variable WPA_ACTION_SCRIPT is present, wpa_cli will
# be spawned via start-stop-daemon
#
# Required options:
# -a	action script => wpa_action
# -P	process ID file
# -B	background process
#
init_wpa_cli () {
	if [ -n "$WPA_ACTION_SCRIPT" ]; then
		local WPA_CLI_OPTIONS
		WPA_CLI_OPTIONS="-B -P $WPA_CLI_PIDFILE -i $WPA_IFACE"

		wpa_msg verbose "$WPA_CLI_BIN $WPA_CLI_OPTIONS -p $WPA_CTRL_DIR -a $WPA_ACTION_SCRIPT"
			
		start-stop-daemon --start --oknodo $DAEMON_VERBOSITY \
			--name $WPA_CLI_PNAME --startas $WPA_CLI_BIN --pidfile $WPA_CLI_PIDFILE \
			-- $WPA_CLI_OPTIONS -p $WPA_CTRL_DIR -a $WPA_ACTION_SCRIPT

		if [ "$?" != "0" ]; then
			wpa_msg stderr "$WPA_CLI_BIN daemon failed to start"
			return 1
		fi
	fi
}

#####################################################################
## stop wpa_cli process
# Kill wpa_cli via start-stop-daemon, given the location of the
# pidfile
#
kill_wpa_cli () {
	if test_wpa_cli; then
		
		wpa_msg verbose "terminating $WPA_CLI_PNAME daemon via pidfile $WPA_CLI_PIDFILE"
		
		start-stop-daemon --stop --oknodo $DAEMON_VERBOSITY \
			--exec $WPA_CLI_BIN --pidfile $WPA_CLI_PIDFILE
		
		if [ -f "$WPA_CLI_PIDFILE" ]; then
			rm -f "$WPA_CLI_PIDFILE"
		fi
	fi
}

#####################################################################
## higher level wpa_cli wrapper for variable and set_network commands
# wpa_cli_do <value> <type> <variable> [set_network variable] <desc>
#
# $1	envorinment variable
# $2	data type of variable {raw|ascii}
# $3	wpa_cli variable, if $3 is set_network, shift and take 
#	set_network subvariable
# $4	wpa-* string as it would appear in interfaces file, enhances
#	verbose messages
#
wpa_cli_do () {
	if [ -z "$1" ]; then
		return 0
	fi
	
	local WPACLISET_VALUE WPACLISET_VARIABLE WPACLISET_DESC
	
	case "$2" in
		ascii)
			# Double quote
			WPACLISET_VALUE="\"$1\""
			;;
		raw|*)
			# Provide raw value
			WPACLISET_VALUE="$1"
			;;
	esac
	
	case "$3" in
		set_network)
			if [ -z "$WPA_ID" ]; then
				return 1
			fi
			shift
			WPACLISET_VARIABLE="set_network $WPA_ID $3"
			;;
		*)
			WPACLISET_VARIABLE="$3"
			;;
	esac
	
	case "$4" in
		*-psk|*-passphrase|*-passwd*|*-wep-key*)
			WPACLISET_DESC="$4 *****"
			;;
		*)
			WPACLISET_DESC="$4 $WPACLISET_VALUE"
			;;
	esac

	wpa_msg action "$WPACLISET_DESC"
	
	wpa_cli $WPACLISET_VARIABLE "$WPACLISET_VALUE" >$TO_NULL
}

#####################################################################
## check value data type in plaintext or hex
# returns 0 if input consists of hexadecimal digits only, 1 otherwise
#
ishex () {
	if [ -z "$1" ]; then 
		return 0
	fi
	
	case "$1" in
		*[!0-9a-fA-F]*)
			# plaintext
			return 1
			;;
		*)
			# hexadecimal
			return 0
			;;
	esac
}

#####################################################################
## sanity check and set psk|passphrase
# Warn about strange psk|passphrase values
#
# $1	psk or passphrase value
# 
# If psk is surrounded by quotes strip them.
#
# If psk contains all hexadecimal characters and string length is 64:
#	is 256bit hexadecimal
# else:
# 	is plaintext
#
# plaintext passphrases must be 8 - 63 characters in length
# 256-bit hexadecimal key must be 64 characters in length
#
wpa_key_check_and_set () {
	if [ -z "$1" ]; then
		return 0
	fi

	local KEY KEY_TYPE
	
	case "$1" in
		'"'*'"')
			# Strip surrounding quotation marks
			KEY=$(echo -n "$1" | sed 's/^"//;s/"$//')
			;;
		*)
			KEY="$1"
			;;
	esac

	if ishex "$KEY" && [ "${#KEY}" -eq "64" ]; then
		KEY_TYPE="raw"
	else
		KEY_TYPE="ascii"
		if [ "${#KEY}" -lt "8" ] || [ "${#KEY}" -gt "63" ]; then
			wpa_msg stderr \
				"plaintext or ascii wpa-psk has ${#KEY} characters, it must have between 8 and 63"
			wpa_msg stderr \
				"if wpa-psk truly is a 256-bit hexadecimal key, it must have 64 characters"
		fi
	fi
	
	wpa_cli_do "$KEY" "$KEY_TYPE" \
		set_network psk wpa-psk
}

#####################################################################
## formulate a usable configuration from interfaces(5) wpa- lines
# A series of wpa_cli commands corresponding to environment variables
# created as a result of wpa- lines in an interfaces stanza.
#
# NB: no-act when roaming daemon is used (to avoid prematurely
# attaching to ctrl_interface socket)
#
conf_wpa_supplicant () {
	if [ -n "$WPA_ACTION_SCRIPT" ]; then
		return 0
	fi

	if [ "$IF_WPA_DRIVER" = "wired" ]; then
		IF_WPA_AP_SCAN="0"
		wpa_msg verbose "forcing ap_scan=0 (required for wired IEEE8021X auth)"
	fi

	if [ -n "$IF_WPA_ESSID" ]; then
		# #403316, be similar to wireless tools
		IF_WPA_SSID="$IF_WPA_ESSID"
	fi
	
	wpa_cli_do "$IF_WPA_AP_SCAN" raw \
		ap_scan wpa-ap-scan
	
	wpa_cli_do "$IF_WPA_PREAUTHENTICATE" raw \
		preauthenticate wpa-preauthenticate
		
	if [ -n "$IF_WPA_SSID" ] || [ "$IF_WPA_DRIVER" = "wired" ]; then
		
		case "$IF_WPA_SSID" in
			'"'*'"')
				IF_WPA_SSID=$(echo -n "$IF_WPA_SSID" | sed 's/^"//;s/"$//')
				;;
			*)
				;;
		esac
		
		WPA_ID=$(wpa_cli add_network)

		wpa_msg verbose "configuring network block -- $WPA_ID"
		
		wpa_cli_do "$IF_WPA_SSID" ascii \
			set_network ssid wpa-ssid
		
		wpa_cli_do "$IF_WPA_PRIORITY" raw \
			set_network priority wpa-priority
		
		wpa_cli_do "$IF_WPA_BSSID" raw \
			set_network bssid wpa-bssid
		
		if [ -s "$IF_WPA_PSK_FILE" ]; then
			IF_WPA_PSK=$(cat "$IF_WPA_PSK_FILE")
		fi
		
		# remain compat with wpa-passphrase-file
		if [ -s "$IF_WPA_PASSPHRASE_FILE" ]; then
			IF_WPA_PSK=$(cat "$IF_WPA_PASSPHRASE_FILE")
		fi
		
		# remain compat with wpa-passphrase
		if [ -n "$IF_WPA_PASSPHRASE" ]; then
			IF_WPA_PSK="$IF_WPA_PASSPHRASE"
		fi
	
		if [ -n "$IF_WPA_PSK" ]; then
			wpa_key_check_and_set "$IF_WPA_PSK"
		fi
		
		wpa_cli_do "$IF_WPA_PAIRWISE" raw \
			set_network pairwise wpa-pairwise
		
		wpa_cli_do "$IF_WPA_GROUP" raw \
			set_network group wpa-group

		wpa_cli_do "$IF_WPA_MODE" raw \
			set_network mode wpa-mode

		wpa_cli_do "$IF_WPA_FREQUENCY" raw \
			set_network frequency wpa-frequency
		
		wpa_cli_do "$IF_WPA_KEY_MGMT" raw \
			set_network key_mgmt wpa-key-mgmt
		
		wpa_cli_do "$IF_WPA_PROTO" raw \
			set_network proto wpa-proto
		
		wpa_cli_do "$IF_WPA_AUTH_ALG" raw \
			set_network auth_alg wpa-auth-alg
		
		wpa_cli_do "$IF_WPA_SCAN_SSID" raw \
			set_network scan_ssid wpa-scan-ssid
		
		wpa_cli_do "$IF_WPA_IDENTITY" ascii \
			set_network identity wpa-identity
		
		wpa_cli_do "$IF_WPA_ANONYMOUS_IDENTITY" ascii \
			set_network anonymous_identity wpa-anonymous-identity
		
		wpa_cli_do "$IF_WPA_EAP" raw \
			set_network eap wpa-eap
		
		wpa_cli_do "$IF_WPA_EAPPSK" raw \
			set_network eappsk wpa-eappsk

		wpa_cli_do "$IF_WPA_NAI" ascii \
			set_network nai wpa-nai

		wpa_cli_do "$IF_WPA_PASSWORD" ascii \
			set_network password wpa-password

		wpa_cli_do "$IF_WPA_CA_CERT" ascii \
			set_network ca_cert wpa-ca-cert

		wpa_cli_do "$IF_WPA_CA_PATH" ascii \
			set_network ca_path wpa-ca-path

		wpa_cli_do "$IF_WPA_CLIENT_CERT" ascii \
			set_network client_cert wpa-client-cert

		wpa_cli_do "$IF_WPA_PRIVATE_KEY" ascii \
			set_network private_key wpa-private-key

		wpa_cli_do "$IF_WPA_PRIVATE_KEY_PASSWD" ascii \
			set_network private_key_passwd wpa-private-key-passwd
		
		wpa_cli_do "$IF_WPA_DH_FILE" ascii \
			set_network dh_file wpa-dh-file

		wpa_cli_do "$IF_WPA_SUBJECT_MATCH" ascii \
			set_network subject_match wpa-subject-match

		wpa_cli_do "$IF_WPA_ALTSUBJECT_MATCH" ascii \
			set_network altsubject_match wpa-altsubject-match

		wpa_cli_do "$IF_WPA_CA_CERT2" ascii \
			set_network ca_cert2 wpa-ca-cert2

		wpa_cli_do "$IF_WPA_CA_PATH2" ascii \
			set_network ca_path2 wpa-ca-path2

		wpa_cli_do "$IF_WPA_CLIENT_CERT2" ascii \
			set_network client_cert2 wpa-client-cert2

		wpa_cli_do "$IF_WPA_PRIVATE_KEY2" ascii \
			set_network private_key2 wpa-private-key2

		wpa_cli_do "$IF_WPA_PRIVATE_KEY_PASSWD2" ascii \
			set_network private_key_passwd2 wpa-private-key-passwd2
		
		wpa_cli_do "$IF_WPA_DH_FILE2" ascii \
			set_network dh_file2 wpa-dh-file2

		wpa_cli_do "$IF_WPA_SUBJECT_MATCH2" ascii \
			set_network subject_match2 wpa-subject-match2

		wpa_cli_do "$IF_WPA_ALTSUBJECT_MATCH2" ascii \
			set_network altsubject_match2 wpa-altsubject-match2
		
		wpa_cli_do "$IF_WPA_EAP_METHODS" raw \
			set_network eap_methods wpa-eap-methods

		wpa_cli_do "$IF_WPA_PHASE1" ascii \
			set_network phase1 wpa-phase1

		wpa_cli_do "$IF_WPA_PHASE2" ascii \
			set_network phase2 wpa-phase2

		wpa_cli_do "$IF_WPA_PCSC" raw \
			set_network pcsc wpa-pcsc

		wpa_cli_do "$IF_WPA_PIN" ascii \
			set_network pin wpa-pin

		wpa_cli_do "$IF_WPA_ENGINE" raw \
			set_network engine wpa-engine

		wpa_cli_do "$IF_WPA_ENGINE_ID" ascii \
			set_network engine_id wpa-engine-id

		wpa_cli_do "$IF_WPA_KEY_ID" ascii \
			set_network key_id wpa-key-id

		wpa_cli_do "$IF_WPA_EAPOL_FLAGS" raw \
			set_network eapol_flags wpa-eapol-flags
		
		wpa_cli_do "$IF_WPA_WEP_KEY0" raw \
			set_network wep_key0 wpa-wep-key0
		
		wpa_cli_do "$IF_WPA_WEP_KEY1" raw \
			set_network wep_key1 wpa-wep-key1
		
		wpa_cli_do "$IF_WPA_WEP_KEY2" raw \
			set_network wep_key2 wpa-wep-key2
		
		wpa_cli_do "$IF_WPA_WEP_KEY3" raw \
			set_network wep_key3 wpa-wep-key3
		
		wpa_cli_do "$IF_WPA_WEP_TX_KEYIDX" raw \
			set_network wep_tx_keyidx wpa-wep-tx-keyidx
		
		wpa_cli_do "$IF_WPA_PROACTIVE_KEY_CACHING" raw \
			set_network proactive_key_caching wpa-proactive-key-caching
			
		wpa_cli_do "$IF_WPA_PAC_FILE" ascii \
			set_network pac_file wpa-pac-file
		
		wpa_cli_do "$IF_WPA_MODE" raw \
			set_network mode wpa-mode
		
		wpa_cli_do "$IF_WPA_PEERKEY" raw \
			set_network peerkey wpa-peerkey
			
		wpa_cli_do "$IF_FRAGMENT_SIZE" raw \
			set_network fragment_size wpa-fragment-size

		wpa_cli_do "$IF_WPA_ID_STR" ascii \
			set_network id_str wpa-id-str
		
		wpa_cli_do "$WPA_ID" raw \
			enable_network "enabling network block"
	fi
}

#####################################################################
## wpa_action basic logging
# Log actions to file, test to see if it is writeable first
#
wpa_log_init () {
	if touch "$WPA_CLI_LOGFILE" 2>/dev/null; then
		exec >> "$WPA_CLI_LOGFILE" 2>&1
	fi
}

# log timestamp and wpa_action arguments
wpa_log_action () {
	echo "########## $(date +"%H:%M:%S  %Y-%m-%d") ##########"
	echo "IFACE=$WPA_IFACE ACTION=$WPA_ACTION"
}

# log wpa_cli environment variables
wpa_log_environment () {
	echo "WPA_ID=$WPA_ID WPA_ID_STR=$WPA_ID_STR"
	echo "WPA_CTRL_DIR=$WPA_CTRL_DIR"
}

#####################################################################
## hysteresis checking
# Networking tools such as dhcp clients used with ifupdown can
# synthesize artificial ACTION events, particuarly just after a 
# DISCONNECTED/CONNECTED events are experienced in quick succession.
# This can lead to infinite event loops, and in extreme cases has the
# potential to cause system instability.
#
wpa_hysteresis_event () {
	echo "$(date +%s)" > "$WPA_CLI_TIMESTAMP"
}

wpa_hysteresis_check () {
	if [ -f "$WPA_CLI_TIMESTAMP" ]; then
		local TIME TIMESTAMP TIMEWAIT
		TIME=$(date +%s)
		# current time minus 4 second event buffer
		TIMEWAIT=$(($TIME-4))
		# get time of last event
		TIMESTAMP=$(cat $WPA_CLI_TIMESTAMP)
		# compare values, allowing new action to be processed 
		# only if last action was more than 4 seconds ago
		if [ "$TIMEWAIT" -le "$TIMESTAMP" ]; then
			echo "Ignoring $WPA_ACTION event, too soon after previous event"
			return 1
		fi
	fi

	return 0
}

#####################################################################
## identify ifupdown files
# Identify ifupdown core files, so that state of the interface can be
# checked. This is the weakest part of the wpa_action roaming scheme,
# it would be _much_ better if stateless ifupdown capabilities were
# a reality.
#
ifupdown_check () {
	if [ -e /etc/network/interfaces ]; then
		INTERFACES_FILE="/etc/network/interfaces"
	else
		echo "Cannot locate ifupdown's \"interfaces\" file, $WPA_IFACE will not be configured"
		return 1
	fi

	if [ -e /etc/network/run/ifstate ]; then
		# debian's ifupdown
		IFSTATE_FILE="/etc/network/run/ifstate"
	elif [ -e /var/run/network/ifstate ]; then
		# ubuntu's
		IFSTATE_FILE="/var/run/network/ifstate"
	else
		echo "Cannot locate ifupdown's \"ifstate\" file, $WPA_IFACE will not be configured"
		return 1
	fi

	return 0
}

#####################################################################
## apply mapping logic and ifup logical interface
# Apply mapping logic via id_str or external mapping script, check
# state of IFACE with respect to ifupdown and ifup logical interaface
#
ifup () {
	local WPA_LOGICAL_IFACE
	
	if [ -z "$IF_WPA_MAPPING_SCRIPT_PRIORITY" ] && [ -n "$WPA_ID_STR" ]; then
		WPA_LOGICAL_IFACE="$WPA_ID_STR"
		echo "Mapping logical interface via id_str: $WPA_LOGICAL_IFACE"
	fi
	
	if [ -z "$WPA_LOGICAL_IFACE" ] && [ -n "$IF_WPA_MAPPING_SCRIPT" ]; then
		echo "Mapping logical interface via wpa-mapping-script: $IF_WPA_MAPPING_SCRIPT"
		
		local WPA_MAP_STDIN
		
		WPA_MAP_STDIN=$(set | sed -n 's/^\(IF_WPA_MAP[0-9]*\)=.*/echo \$\1/p')
		
		if [ -n "$WPA_MAP_STDIN" ]; then
			WPA_LOGICAL_IFACE=$(eval "$WPA_MAP_STDIN" | "$IF_WPA_MAPPING_SCRIPT" "$WPA_IFACE")
		else		
			WPA_LOGICAL_IFACE=$("$IF_WPA_MAPPING_SCRIPT" "$WPA_IFACE")
		fi
		
		if [ -n "$WPA_LOGICAL_IFACE" ]; then
			echo "Mapping script result: $WPA_LOGICAL_IFACE"
		else
			echo "Mapping script failed."
		fi
	fi

	if [ -z "$WPA_LOGICAL_IFACE" ]; then
		if [ -n "$IF_WPA_ROAM_DEFAULT_IFACE" ]; then
			WPA_LOGICAL_IFACE="$IF_WPA_ROAM_DEFAULT_IFACE"
			echo "Using wpa-roam-default-iface: $WPA_LOGICAL_IFACE"
		else
			WPA_LOGICAL_IFACE="default"
			echo "Using fallback logical interface: $WPA_LOGICAL_IFACE"
		fi
	fi

	if [ -n "$WPA_LOGICAL_IFACE" ]; then
		if egrep -q "^iface[[:space:]]+$WPA_LOGICAL_IFACE[[:space:]]+inet" "$INTERFACES_FILE"; then
			
			echo "ifup $WPA_IFACE=$WPA_LOGICAL_IFACE"
			
			if grep -q "^$WPA_IFACE=$WPA_IFACE" "$IFSTATE_FILE"; then
				# Force settings over the unconfigured "master" IFACE
				/sbin/ifup --force "$WPA_IFACE=$WPA_LOGICAL_IFACE"
			else
				/sbin/ifup "$WPA_IFACE=$WPA_LOGICAL_IFACE"
			fi
		else
			echo "No network defined for \"$WPA_LOGICAL_IFACE\" in \"$INTERFACES_FILE\""
		fi
	else
		echo "No suitable logical interface mapping for ifupdown to configure"
	fi
}

#####################################################################
## ifdown IFACE
# Check IFACE state and ifdown as requested.
#
ifdown () {
	if grep -q "^$WPA_IFACE" "$IFSTATE_FILE"; then
		echo "ifdown $WPA_IFACE"
		/sbin/ifdown "$WPA_IFACE"
	else
		echo "Ignoring request to take \"$WPA_IFACE\" down, it is not up"
	fi
}

#####################################################################
## keep IFACE scanning
# After ifdown, the IFACE may be left "down", and inhibits 
# wpa_supplicant's ability to continue roaming.
#
# NB: use iproute if present, flushing the IFACE first
#
if_post_down_up () {
	if [ -x /sbin/ip ]; then
		/sbin/ip addr flush dev "$WPA_IFACE" 2>/dev/null
		/sbin/ip link set "$WPA_IFACE" up
	else
		/sbin/ifconfig "$WPA_IFACE" up
	fi
}
