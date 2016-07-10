// functions
function foo()
{
    return 10;
}

assert_equal(foo(), 10);

var a = 20;
function afoo()
{
    return a + 10;
}

assert_equal(30, afoo());

var c = 13;
function bfoo()
{
    var c = 10;
    return c = c + 10;
}

assert_equal(20, bfoo());
assert_equal(c, 13);

var d = 10;
function cfoo(a, b, c, d)
{
    return a + b + c + d;
}

assert_equal(50, cfoo(20, 10, d, d));


