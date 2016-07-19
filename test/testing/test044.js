// array map
var a = [1,3,6];
if (!this.assert_equal) {
    assert_equal = function(a, b, msg) {
        if (a != b) {
            console.log(msg);
        }
    };
}
function Mapper(a, i) { if (a < 5) return a; return -a; }

var res = a.map(Mapper);
assert_equal(res[0], 1);
assert_equal(res[1], 3);
assert_equal(res[2], -6);

function sq(a) { return a * a; }
var res = a.map(sq);
assert_equal(res[0], 1);
assert_equal(res[1], 9);
assert_equal(res[2], 36);

// example from MDN
var kvArray = [{key:1, value:10}, {key:2, value:20}, {key:3, value: 30}];
var reformattedArray = kvArray.map(function(obj){ 
   var rObj = {};
   rObj[obj.key] = obj.value;
   return rObj;
});

assert_equal(reformattedArray[0]['1'], 10);
assert_equal(reformattedArray[1]['2'], 20);
assert_equal(reformattedArray[2]['3'], 30);
