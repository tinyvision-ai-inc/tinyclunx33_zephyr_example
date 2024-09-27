date=$(date +%Y_%m_%d)

log() { local example=$1 label=$2 color=$3
    printf 'test %-30s\x1B[1;%dm%s\x1B[m\n' "$example: " "$color" "$label"
}

for example in example_*; do
    bin=tinyclunx33_zephyr_${example}_${date}.bin

    log "$example" START 30
    if [ -f "$bin" ];then
        log "$example" DONE 30
    elif ! (cd "$example" && sh test.sh) >test_$example.log 2>&1; then
        log "$example" FAIL 31
    else
        cp "$example/build/zephyr/zephyr.bin" "$bin"
        log "$example" DONE 32
    fi
done
