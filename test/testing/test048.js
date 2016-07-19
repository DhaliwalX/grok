// simple linked list
var LinkedListNode = function (data) {this.data = data;this.next = null;};var LinkedList = function(){this.head = null;this.size = 0;};

LinkedList.prototype.push = function(data) { var tmp = this.head;this.head = new LinkedListNode(data);this.head.next = tmp;};

LinkedList.prototype.forEach = function(func) {
    var node = this.head;

    while (node != null) {
        func(node.data);
        node = node.next;
    }
};

var list = new LinkedList();
list.push(10);
list.push(20);
list.push(30);

var x = 30;
list.forEach(function(a) { assert_equal(x, a); x = x - 10; });
