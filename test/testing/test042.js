if (!this.assert_equal) {
    assert_equal = function(a, b, msg) {
        if (a != b) {
            console.log(msg);
        }
    };
}

var arr = new Array(100);

assert_equal(arr.length, 100);

function gen(arr, start) {
    for (i = 0; i < arr.length; i++) {
        arr[i] = start + i;
    }
}

gen(arr, 1);
var old = arr.toString();
var to_concat = new Array(200);
gen(to_concat, arr.length + 1);

var new_array = arr.concat(to_concat);
assert_equal(arr.toString(), old);

for (i = 0; i < new_array.length; i++)
{
    assert_equal(new_array[i], i + 1);
}

