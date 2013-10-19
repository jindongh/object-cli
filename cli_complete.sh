#!/bin/bash
#Cli program name to use auto complete function
cli_paths=(
*objcli
)

function _mycomp()
{
    local word=${COMP_WORDS[COMP_CWORD]}
    COMPREPLY=()
    if [[ ${word} == '--' ]]; then
        actions=($(${COMP_WORDS[@]}autocompletion 2>&1))
    else
        if [[ ${word:0:2} == '--' ]]; then
            unset COMP_WORDS[COMP_CWORD]
        fi
        actions=($(${COMP_WORDS[@]} --autocompletion 2>&1))
    fi
    COMPREPLY=($(compgen -W "$(echo ${actions[@]})" -- "${word}"))
}
for cli_path in ${cli_paths[@]}; do
    complete -F _mycomp ${cli_path}
done
