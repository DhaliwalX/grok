// simple linked list
var LinkedListNode = function (data) {
    this.data = data;
    this.next = null;
};

var LinkedList = function() {
    this.head = null;
    this.size  = 0;
};

LinkedList.prototype.push = function(data) {
    var tmp = this.head;
    this.head = new LinkedListNode(data);
    this.head.next = tmp;
    this.size++;
};

LinkedList.prototype.find = function(key) {
    var node = this.head;
    var parent = node;

    while (node != null) {
        if (node.data == key)
            return { node: node, parent: parent };
        parent = node;
        node = node.next;
    }

    return null;
};

LinkedList.prototype.remove = function (key) {
    var res = this.find(key);
    if (res == null)
        return false;

    if (res.node.data == res.parent.data) {
        if (res.node.data == this.root.data) {
            this.root = null;
            this.size = 0;
        }
    }

    res.parent.next = res.node.next;
    this.size--;
    return true;
};

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
list.push(15);
list.push(30);
list.push(40);
list.remove(15);

var x = 40;
list.forEach(function(a) {
    assert_equal(x, a);
    x = x - 10;
});
