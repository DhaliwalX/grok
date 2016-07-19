// arr.join
if (!this.assert_equal) {
    assert_equal = function(a, b, msg) {
        if (a != b) {
            console.log(msg);
        }
    };
}

var arr = new Array(0,1,2,3,4);

assert_equal([].join(), "");
assert_equal([].join("prince"), "");
assert_equal(arr.join(), "0,1,2,3,4");
assert_equal(arr.join('-'), '0-1-2-3-4');
assert_equal(arr.join('+*/'), '0+*/1+*/2+*/3+*/4');
