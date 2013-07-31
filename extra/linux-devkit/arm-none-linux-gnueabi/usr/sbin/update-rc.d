#!/bin/sh
#
# update-rc.d	Update the links in /etc/rc[0-9S].d/
#
# (c) 2003, 2004 Phil Blundell <pb@handhelds.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

initd="/etc/init.d"
etcd="/etc/rc"
notreally=0
force=0
dostart=0
verbose=0

usage()
{
	cat >&2 <<EOF
usage: update-rc.d [-n] [-f] [-r <root>] <basename> remove
       update-rc.d [-n] [-r <root>] [-s] <basename> defaults [NN | sNN kNN]
       update-rc.d [-n] [-r <root>] [-s] <basename> start|stop NN runlvl [runlvl] [...] .
		-n: not really
		-f: force
		-v: verbose
		-r: alternate root path (default is /)
		-s: invoke start methods if appropriate to current runlevel
EOF
}

checklinks()
{
	local i dn fn remove=0
	if [ "x$1" = "xremove" ]; then
		echo " Removing any system startup links for $bn ..."
		remove=1
	fi

	for i in 0 1 2 3 4 5 6 7 8 9 S; do
		dn="${etcd}${i}.d"
		if [ ! -d $dn ]; then
			continue;
		fi
		for f in ${dn}/[SK]??${bn}; do
			if [ -L $f ]; then
				if [ $remove -eq 0 ]; then
					return 1
				fi
				echo "  $f"
				if [ $notreally -eq 1 ]; then
					continue
				fi
				rm $f
			fi
		done
	done

	return 0
}

dolink()
{
	startstop=$1
	lev=`echo $2 | cut -d/ -f1`
	nn=`echo $2 | cut -d/ -f2`
	fn="${etcd}${lev}.d/${startstop}${nn}${bn}"
	[ $verbose -eq 1 ] && echo "  $fn -> ../init.d/$bn"
	if [ $notreally -eq 0 ]; then
		mkdir -p `dirname $fn`
 		ln -s ../init.d/$bn $fn
	fi
	if [ $dostart -eq 1 ] && [ $startstop = "S" ] && [ $lev = $RUNLEVEL ]; then
		$fn start || true
	fi
}

makelinks()
{
	if ! checklinks; then
		echo " System startup links for $initd/$bn already exist."
		if [ $dostart -eq 1 ] && [ $notreally -eq 0 ] && [ -L ${etcd}${RUNLEVEL}.d/S??${bn} ]; then
			${etcd}${RUNLEVEL}.d/S??${bn} restart || true
		fi
		exit 0
	fi

	echo " Adding system startup for $initd/$bn."

	for i in $startlinks; do
		dolink S $i
	done
	for i in $stoplinks; do
		dolink K $i
	done
}

while [ $# -gt 0 ]; do
	case $1 in
		-n)	notreally=1
			shift
			continue
			;;
		-v)	verbose=1
			shift
			continue
			;;
		-f)	force=1
			shift
			continue
			;;
		-s)	dostart=1
			shift
			continue
			;;
		-r)     shift
			root=$1
			initd="${root}${initd}"
			etcd="${root}${etcd}"
			shift
			;;
		-h | --help)
			usage
			exit 0
			;;
		-*)
			usage
			exit 1
			;;
		*)
			break
			;;
	esac
done

if [ $# -lt 2 ]; then
	usage
	exit 1
fi

bn=$1
shift

if [ $1 != "remove" ]; then
	if [ ! -f "$initd/$bn" ]; then
		echo "update-rc.d: $initd/$bn: file does not exist" >&2
		exit 1
	fi
else
	if [ -f "$initd/$bn" ]; then
		if [ $force -eq 1 ]; then
			echo "update-rc.d: $initd/$bn exists during rc.d purge (continuing)" >&2
		else
			echo "update-rc.d: $initd/$bn exists during rc.d purge (use -f to force)" >&2
			exit 1
		fi
	fi
fi

if [ $dostart -eq 1 ]; then
	#RUNLEVEL=`sed 's/.*\[\(.*\)\]/\1/' < /proc/1/cmdline`
	RUNLEVEL=`runlevel | cut -d" " -f2`
	if [ "x$RUNLEVEL" = "x" ]; then
		echo "Unable to determine current runlevel" >&2
		exit 1
	fi
fi

case $1 in
	remove)
		checklinks "remove"
		;;

	defaults)
		if [ $# -gt 3 ]; then
			echo "defaults takes only one or two arguments" >&2
			usage
			exit 1
		fi
		start=20
		stop=20
		if [ $# -gt 1 ]; then
			start=$2
			stop=$2
		fi
		if [ $# -gt 2 ]; then
			stop=$3
		fi
		start=`printf %02d $start`
		stop=`printf %02d $stop`
		stoplinks="0/$stop 1/$stop 6/$stop"
		startlinks="2/$start 3/$start 4/$start 5/$start"
		makelinks
		;;

	start | stop)
		while [ $# -gt 0 ]; do
			if [ $1 = "start" ]; then
				letter=S
			elif [ $1 = "stop" ]; then
				letter=K
			else
				echo "expected start or stop" >&2
				usage
				exit 1
			fi
			shift
			NN=`printf %02d $1`
			shift
			while [ "x$1" != "x." ]; do
				if [ $# -eq 0 ]; then
					echo "action with list of runlevels not terminated by \`.'" >&2
					exit 1
				fi
				level=$1
				shift
				case $letter in
					S) startlinks="$startlinks $level/$NN" ;;
					K) stoplinks="$stoplinks $level/$NN" ;;
				esac
			done
			shift
		done
		makelinks
		;;

	*)
		usage
		exit 1
		;;
esac
