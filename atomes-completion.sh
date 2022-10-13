# _filedir : to handle file and directories with spaces in their names.

_filedir()
{
	local IFS=$'\t\n' xspec #glob

	#glob=$(set +o|grep noglob) # save glob setting.
	#set -f		 # disable pathname expansion (globbing)

	xspec=${1:+"!*.$1"}	# set only if glob passed in as $1
	COMPREPLY=( ${COMPREPLY[@]:-} $( compgen -f -X "$xspec" -- "$cur" ) \
		    $( compgen -d -- "$cur" ) )
	#eval "$glob"    # restore glob setting.
}

_atomes()
{
  	local cur prev opts files
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"
	first="${COMP_WORDS[1]}"
	if [ $COMP_CWORD -eq 1 -o "${COMPREPLY+set}" != "set" ]; then
        _filedir '[aA][pP][fF]'
       # what about awf files ?	

	fi
}
complete -o filenames -o nospace -F _atomes atomes
