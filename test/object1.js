var x = { a : 10, b : 10, sum : function () { return this.a + this.b; }, print : function() { console.log(this.a, this.b); }};

var sum = x.sum();

if (sum != 20)
    console.error("Test failed");
else
    console.log("Test passed");


