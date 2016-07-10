// simple arithemetic
// addition
var a = 10;
var b = 20 + 5;
var c = 5 + 20;
var d = 10 + 15;

assert_equal(a, 10);
assert_equal(b, 25);
assert_equal(c, 25);
assert_equal(d, 25);

var e = 20.5 + 4.5;
assert_equal(e, 25);

// subtraction
var f = 30 - 4;
var g = 34 - 50;
var h = -2 + 5;

assert_equal(f, 26);
assert_equal(g, -16);
assert_equal(h, 3);

// others
var i = 30 * 5; // 150
var j = 40 / 10; // 4
var k = 234 % 10; // 4
var l = 122 | 1; // 123
var m = 31 & 1; // 1
var n = 1 << 10; // 1024
var o = 1024 >> 10; // 1
var p = 1 ^ 1; // 0

assert_equal(i, 150);
assert_equal(j, 4);
assert_equal(k, 4);
assert_equal(l, 123);
assert_equal(m, 1);
assert_equal(n, 1024);
assert_equal(o, 1);
assert_equal(p, 0);

// unary operators
var q = 10;
assert_equal(q++, 10);
assert_equal(q, 11);
assert_equal(++q, 12);
assert_equal(q--, 12);
assert_equal(q, 11);
assert_equal(--q, 10);
assert_equal(q, 10);

var r = 1;
assert_equal(~r, -2);
assert_equal(!r, 0);
assert_equal(r, 1);

// logical operators
var s = 1 || 0;
assert_equal(s, 1);
s = 0 || 1;
assert_equal(s, 1);
s = 1 || 1;
assert_equal(s, 1);
s = 0 || 0;
assert_equal(s, 0);

var s = 1 && 0;
assert_equal(s, 0);
s = 0 && 1;
assert_equal(s, 0);
s = 1 && 1;
assert_equal(s, 1);
s = 0 && 0;
assert_equal(s, 0);

var t = 10 > 8;
assert_equal(t, 1);
t = 8 > 10;
assert_equal(t, 0);
t = 8 > 8;
assert_equal(t, 0);

t = 10 < 8;
assert_equal(t, 0);
t = 8 < 10;
assert_equal(t, 1);
t = 8 < 8;
assert_equal(t, 0);

t = 10 == 8;
assert_equal(t, 0);
t = 10 == 10;
assert_equal(t, 1);
t = 20 != 24;
assert_equal(t, 1);
t = 20 != 20;
assert_equal(t, 0);

