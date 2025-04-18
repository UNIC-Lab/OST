#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <queue>
#include <iostream>
#include <limits>
#include <fstream>
#include <vector>

#define LONG_MAX 2147483647
#define LLONG_MAX 9223372036854775807

#define N_size 49
#define K_size 9
#define M_size 500

using namespace std;

int size_V, size_E, size_Vd, size_Vs, super_sender, sender[N_size + 2], head[N_size + 2], tot, receiver[K_size + 1];
float r[K_size + 1];
/*
tot total num of edge
*/
float dp[N_size + 2][(1 << (K_size)) + 1];
bool vis[N_size + 2];

#include <fstream>
#include <string>

struct edge_struct
{
    int to, next;
    float edge_cost;
} edge[M_size + 1];

map<int, float> max(map<int, int> x, map<int, int> y)    //<time,requirement>  
{
    map<int, float> ret;
    auto i = x.begin(), j = y.begin();
    while (i != x.end() && j != y.end())
        if (i->first == j->first)
            ret.insert(make_pair(i->first, max(i->second, j->second))), i++, j++;
        else if (i->first < j->first)
            ret.insert(*i), i++;
        else
            ret.insert(*j), j++;
    while (i != x.end())
        ret.insert(*i), i++;
    while (j != y.end())
        ret.insert(*j), j++;
    return ret;
}
struct edge_struct2
{
    int from, to;
    float cost;
};
vector<edge_struct2> tree[N_size + 2][(1 << (K_size)+1)];

vector<edge_struct2> merge(vector<edge_struct2> tree1, vector<edge_struct2> tree2)
{
    vector<edge_struct2> ret = tree1;
    for (auto e : tree2)
    {
        bool flag = 0;
        for (int i = 0; i < tree1.size(); i++)
            if (tree1[i].from == e.from && tree1[i].to == e.to)
                flag = 1, ret[i].cost = max(ret[i].cost, e.cost);  //both own, max()
        if (!flag)
            ret.push_back(e);      //not in tree1, push back
    }
    return ret;
}

void add(int u, int v, float w)
{
    tot++;          //idx of edge
    edge[tot] = edge_struct{ v, head[u], w }; //head[u]: last edge idx connected to node u before node v
    head[u] = tot;
} // add an edge into the graph

float get_definitions(int s)
{
    float ret = 0;


    for (int i = 1; s; i++, s >>= 1)
        if (s & 1)
            if (r[i] > ret)
                ret = r[i];

    return ret;
}

int main()
{
    // input
    scanf("%d%d%d%d", &size_V, &size_E, &size_Vs, &size_Vd);
    float maxFloat = std::numeric_limits<float>::max();
    for (int i = 0; i < N_size + 2; ++i) {
        
        for (int k = 0; k < (1 << K_size) + 1; ++k) {
            dp[i][k] = maxFloat;
        }
        
    }
    for (int i = 1; i <= size_E; i++)
    {
        int u, v;
        float w;
        scanf("%d%d%f", &u, &v, &w);
        add(u, v, w), add(v, u, w);  //Add bidirectional edge
        //add(u, v, w);              //Add one-way edge
    }
    super_sender = ++size_V; // create a super sender and link all the senders to the super sender, all the edges created having a capasity of infinity
    for (int i = 1; i <= size_Vs; i++)
    {
        scanf("%d", sender + i);
        add(sender[i], super_sender, 0), add(super_sender, sender[i], 0);
    }
    for (int i = 1; i <= size_Vd; i++)
    {
        int parts, definition;
        scanf("%d%f", receiver + i, &r[i]);
        
        dp[receiver[i]][(1 << (i - 1))] = 0; 
    }

    // dp
    priority_queue<pair<float, int>> q;   //cost,idx
    for (int s1 = 0; s1 < 1 << (size_Vd); s1++) // s1 is a subset of receivers in binary form with size_Vd digits, 1 on digit i means the i-th receiver is in set and vise versa.
    {
        for (int i = 1; i <= size_V; i++)//update dp based on dj algorithm on last step
        {
            for (int subset = (s1); subset; subset = (s1) & (subset - 1)) // enumerate all the subsets of s1
            {
                int s2 = (subset), s3 = ((subset ^ (s1))); // s2 and complementry s2 compared to s1
                
                if (dp[i][s2] == maxFloat || dp[i][s3] == maxFloat)   //if both dp of two subset, s2 and s3, are defined
                    continue;
                float c = 0;
                vector<edge_struct2> temp = merge(tree[i][s2], tree[i][s3]); //merge two trees
                for (auto i : temp)
                    c += i.cost;    //calculate total cost
                if (c < dp[i][s1])
                {
                    dp[i][s1] = c; // update
                    // record the edges in the tree
                    tree[i][s1] = temp;
                }
                
            }

            if (i != super_sender) // there's no need to update other vertexes from the super sender vertex
                if (dp[i][s1] != maxFloat)
                    q.push(make_pair(-dp[i][s1], i));
        }
        memset(vis, 0, sizeof vis);
        while (q.size()) // Dijkstra. It can be seen as finding the shortest path length from a imaginary vertex which links all the vertexes in the graph with the length of dp[i][s1]
        {
            int x = q.top().second;
            q.pop();
            if (vis[x])
                continue;
            vis[x] = 1;
            for (int i = head[x]; i; i = edge[i].next)
            {
                int y = edge[i].to;
                float w = edge[i].edge_cost;
                if (y != super_sender)
                {
                
                    vector<edge_struct2> temp = vector<edge_struct2>{ edge_struct2{y, x, get_definitions(s1) * w} };
                    float c = 0;
                    temp = merge(tree[x][s1], temp);
                    for (auto i : temp)
                        c += i.cost;
                    if (c < dp[y][s1])
                    {
                        dp[y][s1] = c; // update
                        q.push(make_pair(-dp[y][s1], y));
                        // record the edges in the tree
                        tree[y][s1] = temp;
                    }
                    
                }
                else
                {
                    
                    if (dp[x][s1] < dp[y][s1])
                    {
                        dp[y][s1] = dp[x][s1];
                        tree[y][s1] = tree[x][s1]; // record the edges in the tree
                    }
                }
            }
        }
    }

    if (dp[super_sender][(1 << (size_Vd)) - 1] == maxFloat)
        puts("-1");
    else {
        printf("%.2f\n", dp[super_sender][(1 << (size_Vd)) - 1]);
    }
    puts("edges:");
    for (auto i : tree[super_sender][(1 << (size_Vd)) - 1])
    {
        printf("%d -> %d ", i.from, i.to);
        printf("{ ");

        printf("%.2f ", i.cost);
        puts("}");
    }
}
/*
input format:
<size_V> <size_E> <size_Vs> <size_Vd> the number of vertex, the number of edges, the number of senders, the number of receivers
then size_E lines, each line: <i> <j> <e_ij> means an edge in the graph, whose max capasity is e_ij
then size_Vs integers in a single line, each number: <s> means s is a sender
then size_Vd lines, each line: <r> <definition> means vertex r is a receiver with the definition requirement

sample input:
10 13 1 3
1 2 1
1 3 1
1 4 1
2 3 1
3 4 1
2 5 1
3 6 1
4 7 1
5 6 1
6 7 1
5 8 1
6 9 1
7 10 1
1
10 4000.0
9 1080.0
8 480.0



output:
minimum cost: 15120.00
edges:
5 -> 8 { 480.00 }
6 -> 5 { 480.00 }
7 -> 6 { 1080.00 }
4 -> 7 { 4000.00 }
6 -> 9 { 1080.00 }
7 -> 10 { 4000.00 }
1 -> 4 { 4000.00 }

explanation:
            1
          / | \
         2--3--4
        /   |   \
       5 -- 6 -- 7
       |    |    |
       8    9    10
1 is the sender, 8 is a receiver with weight 480, 9 is a receiver with weight 1080, 10 is a receiver with weight 4000.
*/
