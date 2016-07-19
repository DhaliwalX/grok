// array methods

// 
if (!this.Math) {
    this.Math = {};
    Math.random = this.random; // using inbuilt random function
}
if (!this.assert_equal) {
    assert_equal = function(a, b, msg) {
        if (a != b) {
            console.log(msg);
        }
    };
}

var arr = new Array(10);

assert_equal(arr.length, 10);

function gen(arr) {
    for (i = 0; i < arr.length; i++) {
        arr[i] = (Math.random() * 10) << 0;
    }
}

gen(arr);
arr.sort(arr);

function check(arr, inverted) {
    var start = inverted ? arr.length - 1 : 0;
    var end = inverted ? 0 : arr.length - 1;
    var last = arr[start];
    for (inverted ? --start : ++start;
        start != end; inverted ? --start : ++start) {
        if (arr[start] < last) {
            return false;
        }
        last = arr[start];
    }
    return true;
}

assert_equal(check(arr, false), true, "arr is not sorted even with default predicate");

gen(arr); // create a new array

function pred(a, b)
{
    return a - b;
}

arr.sort(pred);
assert_equal(check(arr, false), true, "arr is not sorted with given predicate");

function inv_pred(a, b)
{
    return b - a;
}

gen(arr);
arr.sort(inv_pred);
assert_equal(check(arr, true), true, "arr is not sorted with inverted predicate");

