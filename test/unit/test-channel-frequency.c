#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "aircrack-ng/osdep/common.h"

/* 6GHz (802.11ax/be) 20MHz channels: special low channel 2 plus 1, 5, ... 233 */
static void test_frequency_from_channel_6e(void ** state)
{
	(void) state;

	/* Special low channel 2 lives at 5935 MHz (operating class 136). */
	assert_int_equal(getFrequencyFromChannel6E(2), 5935);

	/* Regular channels: center = 5950 + channel * 5. */
	assert_int_equal(getFrequencyFromChannel6E(1), 5955);
	assert_int_equal(getFrequencyFromChannel6E(5), 5975);
	assert_int_equal(getFrequencyFromChannel6E(17), 6035);
	assert_int_equal(getFrequencyFromChannel6E(233), 7115);

	/* Out of range. */
	assert_int_equal(getFrequencyFromChannel6E(0), -1);
	assert_int_equal(getFrequencyFromChannel6E(234), -1);
	assert_int_equal(getFrequencyFromChannel6E(-1), -1);
}

static void test_channel_from_frequency_6e(void ** state)
{
	(void) state;

	/* 6GHz mapping is the inverse of getFrequencyFromChannel6E(). */
	assert_int_equal(getChannelFromFrequency(5935), 2);
	assert_int_equal(getChannelFromFrequency(5955), 1);
	assert_int_equal(getChannelFromFrequency(5975), 5);
	assert_int_equal(getChannelFromFrequency(6035), 17);
	assert_int_equal(getChannelFromFrequency(7115), 233);

	/* 2.4GHz and 5GHz must still resolve correctly after the 6GHz changes. */
	assert_int_equal(getChannelFromFrequency(2412), 1);
	assert_int_equal(getChannelFromFrequency(2484), 14);
	assert_int_equal(getChannelFromFrequency(5180), 36);
	assert_int_equal(getChannelFromFrequency(5825), 165);

	/* Guard band between 5GHz (<=5895) and 6GHz ch2 (5935) is unmapped. */
	assert_int_equal(getChannelFromFrequency(5896), -1);
	assert_int_equal(getChannelFromFrequency(5934), -1);

	/* Above the top 6GHz channel is unmapped. */
	assert_int_equal(getChannelFromFrequency(7120), -1);
}

static void test_6e_roundtrip(void ** state)
{
	(void) state;

	int ch;

	assert_int_equal(getChannelFromFrequency(getFrequencyFromChannel6E(2)), 2);
	for (ch = 1; ch <= 233; ch += 4)
		assert_int_equal(
			getChannelFromFrequency(getFrequencyFromChannel6E(ch)), ch);
}

int main(int argc, char * argv[])
{
	(void) argc;
	(void) argv;

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_frequency_from_channel_6e),
		cmocka_unit_test(test_channel_from_frequency_6e),
		cmocka_unit_test(test_6e_roundtrip),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
