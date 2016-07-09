function or_test0(x, y) { return x | y; }
function and_test0(x, y) { return x & y; }
function add_test0(x, y) { return x + y; }

assert_equal(3, or_test0(1, 2));   // 1 | 2
assert_equal(2, and_test0(3, 6));  // 3 & 6
assert_equal(5, add_test0(2, 3));  // 2 + 3

function or_test1(x, y) { return x | x; }
function and_test1(x, y) { return x & x; }
function add_test1(x, y) { return x + x; }

assert_equal(1, or_test1(1, 2));   // 1 | 1
assert_equal(3, and_test1(3, 6));  // 3 & 3
assert_equal(4, add_test1(2, 3));  // 2 + 2

function or_test2(x, y) { x = y; return x | y; }
function and_test2(x, y) { x = y; return x & y; }
function add_test2(x, y) { x = y; return x + y; }

assert_equal(2, or_test2(1, 2));   // 2 | 2
assert_equal(6, and_test2(3, 6));  // 6 & 6
assert_equal(6, add_test2(2, 3));  // 3 + 3
