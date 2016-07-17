// setTImeout
var time = 0;
var times = 100;
var counter = 0;

function call() {
    if (counter == times) {
        return 0;
    }
    counter++;
    console.log(counter);
}

for (i = 0; i < 100000; i++) {
    setTimeout(call, time);    
}
