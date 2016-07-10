// operator precedance
var a = 3 + 4 * 5;
assert_equal(a, 23);

var b = 10 / 4 * 3 << 1 + 29 >> 3 | 10 & 32 && 19 < 20 || 1 > 0 == 1 - 0;

assert_equal(b, 1);

