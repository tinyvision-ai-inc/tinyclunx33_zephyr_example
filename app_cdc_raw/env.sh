export BAUD=156200
export BOARD=tinyclunx33@rev2/rtl_1_0

test_app_cdc_raw() { local tty=$1
	if picocom --initstring helloworld --exit-after 500 "$tty" | grep HELLOWORLD; then
		echo "PASS"
	else
		echo "FAIL"
		return 1
	fi
}
