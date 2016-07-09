a = [
    [0,1,0,0,1],
    [1,0,1,1,1],
    [0,1,0,1,0],
    [0,1,1,0,1],
    [1,1,0,1,0]
];

function BFS(graph, source) {
    for (i = 0; i < graph.length; i++) {
        graph[i].visited = false;
        graph[i].distance = 99999;
        graph[i].parent = null;
    }

    graph[source].distance = 0;
    q = [];
    q.push(source);
    res = [];
    while (q.length) {
        u = q.shift();
        console.log(u);
        for (i = 0; i < graph[u].length; i++) {
            if (graph[u][i]) {
                if (graph[i].visited) {
                    l = 10;
                }
                else {
                    graph[i].distance = graph[u].distance + 1;
                    q.push(i);
                }
            }
        }
        graph[u].visited = true;
        res.push(u);
    }
    return res;
}

time = 0;
dfs_result= [];

function DFS_visit(graph, source) {
    time = time + 1;
    graph[source].start_time = time;
    graph[source].visited = true;
    for (i = 0; i < graph[source].length; i++) {
        if (graph[source][i] == true) 
            if (graph[i].visited) {
                a = 10; // do nothing
            } else {
                graph[i].parent = source;
                DFS_visit(graph, i);
            }
    }
    dfs_result.push(source);
    graph[source].end_time = time;
}

function DFS(graph, source) {
    for (i = 0; i < graph.length; i++) {
        graph[i].visited = false;
        graph[i].start_time = -1;
        graph[i].end_time = -1;
        graph[i].parent = null;
    }
    for (i = 0; i < graph.length; i++) {
        if (graph[i].visited == false) {
            DFS_visit(graph, i);
        }
    }
}

res = BFS(a, 3);

console.log(res);

for (i = 0; i < a.length; i++) {
    console.log('node = ' + i + ', visited : ' + a[i].visited
        + ', distance: ' + a[i].distance);
}

console.log('\x1b[34mRunning DFS...\x1b[0m');
DFS(a);

console.log(dfs_result);
for (i = 0; i < a.length; i++) {
    console.log('[' + i + ']' + ', start: ' + a[i].start_time
        + ', end: ' + a[i].end_time + ', parent: ' + a[i].parent);
}

