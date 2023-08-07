#!/usr/bin/env bats

export PATH="$PWD:$PATH"

@test "pdeathsigexec: process signalled when parent exits" {
    run timeout --foreground --preserve-status 1s sh -c "pdeathsigexec sleep 1235"
    cat << EOF
--- output
$output
---
EOF

    # shell received SIGTERM
    [ "$status" -eq 143 ]

    run pgrep -fa "sleep 1235"
    cat << EOF
--- output
$output
---
EOF

    [ "$output" == "" ]
    [ "$status" -eq 1 ]
}

@test "libpdeathsigexec: process signalled when parent exits" {
    run env LD_PRELOAD=./libpdeathsigexec.so timeout --foreground --preserve-status 1s sh -c "sleep 2346"
    cat << EOF
--- output
$output
---
EOF

    # shell received SIGTERM
    [ "$status" -eq 143 ]

    run pgrep -fa "sleep 2346"
    cat << EOF
--- output
$output
---
EOF

    [ "$output" == "" ]
    [ "$status" -eq 1 ]
}
