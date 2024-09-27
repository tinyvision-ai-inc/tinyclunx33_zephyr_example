date=$(date +%Y_%m_%d)

for example in example_*; do
    printf 'test %-30s' "$example: "
    if ! (cd "$example" && sh test.sh) >test_$example.log 2>&1; then
        printf '\x1B[1;31mFAIL\x1B[m\n'
    else
        cp "$example/build/zephyr/zephyr.bin" "tinyclunx33_zephyr_${example}_${date}.bin"
        printf '\x1B[1;32mPASS\x1B[m\n'
    fi
done
