# atomes(1) completion                                   -*- shell-script -*-

_atomes()
{
  local cur prev words cword split
  _init_completion -s || return

  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}

  case $prev in
    -h | --help | -v | --version)
    return
    ;;
  esac

  $split && return

  if [[ $cur == -* ]]; then
    COMPREPLY=($(compgen -W '$(_parse_help "$1")' -- "$cur"))
    [[ ${COMPREPLY-} == *= ]] && compopt -o nospace
    return
  fi
  
  _filedir '@([aA][wW][fF]|[aA][pP][fF]|[xX][yY][zZ]|[pP][dD][bB]|[eE][nN][tT]|[cC][iI][fF]|[iI][pP][fF]|[tT][rR][jJ]|[xX][dD][aA][tT][cC][aA][rR]|[cC]3[dD]|[hH][iI][sS][tT])'
} &&
complete -F _atomes atomes
