/**
 *  author : coderstack
**/

#include "bits/stdc++.h"

using namespace std;

#define ff first 
#define ss second

struct query
{
    int src, dest, pickStart, pickEnd;
};

struct taxi
{
    int currTime, dropTime, capacity, location, destination;
};

vector<pair<int, query> > accepted_queries, rejected_queries;
vector<vector<int> > dist;

vector<vector<int> > getShortestDistance(vector<vector<int> > &g)
{
    int n = g.size();
    vector<vector<int> > sd = g;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(i == j && g[i][j] == -1)
                sd[i][j] = 0;
            else if(i != j && g[i][j] == -1)
                sd[i][j] = INT_MAX;
        }
    }

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            for(int k = 0; k < n; k++)
            {
                if(sd[i][k] != INT_MAX && sd[k][j] != INT_MAX && sd[i][j] > sd[i][k] + sd[k][j])
                {
                    sd[i][j] = sd[i][k] + sd[k][j];
                    sd[j][i] = sd[i][j]; 
                }
            }
        }
    }

    return sd;
}

pair<int, int> findNearest(pair<int, int> &qr, vector<taxi> &t, vector<vector<int> > &dist)
{
    int n = t.size(), idx = -1, distance = INT_MAX, code = -1;
    for(int i = 0; i < n; i++)
    {
        if(t[i].capacity < 5 && (dist[t[i].location][qr.ff] < distance|| dist[qr.ff][t[i].destination] < distance))
        {
            idx = i;
            distance = min(dist[t[i].location][qr.ff], dist[qr.ff][t[i].destination]);
            if(distance == dist[t[i].location][qr.ff])
                code = 0;
            else if(dist[qr.ff][t[i].destination])
                code = 1;
        }
    }

    return pair<int,int>(idx, code);
}

int findTaxi(pair<int, int> &pickup, pair<int, int> &qr, vector<taxi> &t, vector<vector<int> > &dist)
{
    int found = -1;
    int n = t.size(), x = 0, y = 0;
    for(int i = 0; i < n; i++)
    {
        if(t[i].capacity == 5) continue;

        x = t[i].location, y = qr.ff;
        if((t[i].destination == -1 || t[i].capacity == 0) && (t[i].currTime + 2 * dist[x][y]) <= pickup.ss)
        {
            t[i].currTime = pickup.ff;
            t[i].location = y;
            t[i].destination = qr.ss;
            t[i].capacity = 1;
            t[i].dropTime = t[i].currTime + dist[y][qr.ss];

            found = i;
        }
        else if(t[i].destination == qr.ss)
        {
            if(t[i].dropTime < pickup.ff && (t[i].dropTime + 2 * dist[t[i].destination][y]) <= pickup.ss) {
                    t[i].location = y;
                    t[i].currTime = t[i].dropTime + 2 * dist[t[i].destination][y];
                    t[i].dropTime = t[i].currTime + 2 * dist[t[i].location][qr.ss];
                    t[i].capacity = 1;
            }
            else if(t[i].dropTime > pickup.ff && (t[i].currTime + 2 * dist[t[i].location][y]) <= pickup.ss) {
                    t[i].location = y;
                    t[i].currTime += 2 * dist[t[i].location][y];
                    t[i].dropTime = t[i].currTime + 2 * dist[t[i].location][qr.ss];
                    t[i].capacity += 1;
            }

            found = i;
        }
    }
    
    if(found == -1)
    {
        pair<int, int> nearest = findNearest(qr, t, dist);
        int idx = nearest.ff;
        if(idx != -1)
        {
            int code = nearest.ss;
            if(code == 1 && (t[idx].dropTime + 2 * dist[t[idx].destination][qr.ff]) <= pickup.ss)
            {
                t[idx].capacity = 1;
                t[idx].currTime = t[idx].dropTime + 2 * dist[t[idx].destination][qr.ff];
                t[idx].dropTime = t[idx].currTime + 2 * dist[qr.ff][qr.ss];
                t[idx].location = qr.ff;
                t[idx].destination = qr.ss;
            }
            else if(code == 0 && (t[idx].currTime + 2 * dist[t[idx].location][qr.ff]) <= pickup.ss)
            {
                t[idx].capacity = 1;
                t[idx].currTime += 2 * dist[t[idx].location][qr.ff] + 2 * dist[qr.ff][t[idx].destination];
                t[idx].location = t[idx].destination;
                t[idx].destination = qr.ss;
                t[idx].dropTime = t[idx].currTime + 2 * dist[t[idx].location][qr.ss];
            }
            else
            {
                idx = -1;
            }
        }

        found = idx;
    }

    return found;
}

int solveQueries(vector<vector<int> > &dist, vector<query> &q, vector<taxi> &t)
{
    int profit = 0;

    vector<query> :: iterator it;
    for(it = q.begin(); it != q.end(); it++)
    {
        pair<int, int> p = pair<int,int>(it->pickStart, it->pickEnd);
        pair<int, int> qr = pair<int, int>(it->src, it->dest);
        int x = -1;

        if((x = findTaxi(p, qr, t, dist)) != -1)
        {
            profit += dist[it->src][it->dest];
            accepted_queries.push_back(pair<int, query>(x, *it));
        }
        else
            rejected_queries.push_back(pair<int, query>(x, *it));
    }

    return profit;
}

bool myCompare(query &a, query &b)
{
    if(a.pickStart == b.pickStart)
    {
        return dist[a.src][a.dest] > dist[b.src][b.dest];
    }

    return a.pickStart <= b.pickStart;
}

void STACK_CODED()
{
    int n, N, c, R;
    cin >> n >> N >> c >> R;

    vector<vector<int> > g(n, vector<int>(n, 0));
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
            cin >> g[i][j];
    }

    vector<taxi> t(N);
    for(int i = 0; i < N; i++)
    {
        cin >> t[i].location;

        t[i].capacity = 0;
        t[i].currTime = 0;
        t[i].dropTime = 0;
        t[i].location -= 1;
        t[i].destination = -1;
    }

    vector<query> q(R);
    for(int i = 0; i < R; i++)
    {
        cin >> q[i].src >> q[i].dest >> q[i].pickStart >> q[i].pickEnd;

        q[i].src -= 1;
        q[i].dest -= 1;
    }

    dist = getShortestDistance(g);
    sort(q.begin(), q.end(), myCompare);


    cout << "Profit Generated : "  << solveQueries(dist, q, t) << "\n\n";
    cout << "Following Queries were accepted : \n";
    for(int i = 0; i < accepted_queries.size(); i++)
    {
        int x = accepted_queries[i].first + 1;
        string vehicleno = "" + to_string(x);
        if(x < 10)
            vehicleno = '0' + vehicleno;
        cout << "Vehicle No : " << vehicleno << "\t   " << "Query : " << accepted_queries[i].second.src + 1 << ' ' << accepted_queries[i].second.dest + 1 << ' ' << accepted_queries[i].second.pickStart << ' ' << accepted_queries[i].second.pickEnd << '\n';
    }

}

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    STACK_CODED();
    return 0;
}
