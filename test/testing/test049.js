// 
var a = 10;

function Add(a, b) {
    a = a + 1; // in older version addition here affected the original var
    assert_equal(a, b);
    return a;
}

Add(a, 11);
Add(a, 11);
Add(a, 11);

// in loops
function Loop(a, i, n) {
    var j = i;
    var old = a;
    for (; i < n; ++i) {
        a++; // increment operator shouldn't affect the integer literal
    }

    assert_equal(a - old, n - j);
}

var a = 12;
var i = 0;
var n = 100;

Loop(a, i, n);
assert_equal(a, 12, "a != 12");
assert_equal(i, 0, "i != 0");
assert_equal(n, 100, "n != 100");


