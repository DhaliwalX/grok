// function calls
function fact(n)
{
    assert_equal(this.a, 10);
    return (n == 0 || n == 1) ? 1 : fact(n - 1) * n;
}

function P(n, m)
{
    assert_equal(this.a, 10);
    return fact(n) / fact(m);
}

function C(n, m)
{
    assert_equal(this.a, 10);
    assert_equal(n >= m, true);
    return fact(n) / (fact(n - m) * fact(m));
}

var a = 10;
var result = fact(10); // 3628800
assert_equal(result, 3628800);
result = P(4, 3);   // 4
assert_equal(result, 4);
result = P(6, 2);
assert_equal(result, 360);
result = C(4,3);
assert_equal(result, 4);
result = C(5,3);
assert_equal(result, 10);
result = C(3,3);
assert_equal(result, 1);
result = C(3,1);
assert_equal(result, 3);
result = C(3,0);
assert_equal(result, 1); 
