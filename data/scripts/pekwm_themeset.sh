#!/bin/sh
#
# Copyright © 2003-2008 the pekwm development team
#
# Add this to your menu to use this script:
#
# SubMenu = "Themes" {
#   Entry { Actions = "Dynamic /path/to/this/file /path/to/themedir" }
# }
#

# Check usage
if test -z "${1}"; then
    echo "usage: $0 /path/to/themedir (theme)";
    exit 1
fi

if test -z "${2}"; then
    theme_dir="${1}"

    echo "Dynamic {"

    # Check that theme directory exists, if it does not exist create a
    # dummy entry that says the dir does not exist.
    if test -d "${theme_dir}"; then
        for i in $(ls "${theme_dir}"); do
            echo -e "Entry = \"${i}\" { Actions = \"Exec ${0} ${1} ${theme_dir}/${i}\" }"
        done
    else
        echo -e "Entry = "No such directory ${theme_dir}" { Actions = "NO" }"
    fi

    echo "}"

else
    # Check for configuration file, if the environment is not set the
    # script is not being run from pekwm, then exit with failure.
    if test -f "${PEKWM_CONFIG_FILE}"; then
        theme="$(echo "${2}" | sed -e "s@^${HOME}@~@" | sed -e 's/\//\\\//g')"

        # Get temporary file, not all platforms have mktemp though
        if test -x "/bin/mktemp"; then
            tmp_file=$(mktemp -t pekwm_themeset.XXX) || exit 1;
        else
            tmp_file="/tmp/pekwm_themeset.${USER}"
        fi

        # Change theme
        sed -e "s/^\([^#]*\)Theme\ =\ \"[^\"]*\"/\\1Theme\ =\ \"${theme}\"/i" "${PEKWM_CONFIG_FILE}" > "${tmp_file}"
        mv "${tmp_file}" "${PEKWM_CONFIG_FILE}"

        # Reload pekwm
        kill -HUP $(xprop -root _NET_WM_PID | awk '/_NET_WM_PID/ { print $3 }')
    else
        exit 1
    fi
fi

exit 0
