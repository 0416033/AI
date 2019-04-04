#include<iostream>
#include<fstream>
#include<sstream>
#include<stdio.h>
#include<vector>
#include<stack>
#include<queue>
#include<algorithm>
#define DICTSIZE 2932
#define MAX_LENGTH 32
using namespace std;

vector<string> dictionary;
int index[] = {0};
struct var//
{
    int x;
    int y;
    int length;
    char dir;
};
struct constrain
{
    //A constrain between (i_a)th across word and (i_d)th down word.
    //located on (a)th character of across and (d)th character of down.
    int i_a;
    int i_d;
    int a;
    int d;
};
struct assignment
{
    int var;
    int index;
};
struct node
{
    assignment a;
    vector<assignment> path;
    vector<vector<int> > domain;
};
constrain check(var a,var d)//check if there's overlap between variables.
{
    constrain c;
    c.i_a=0;
    c.i_d=0;
    int ca = d.x - a.x;
    int cd = a.y -d.y;
    if(ca >= 0 && ca < a.length && cd >= 0 && cd < d.length)
    {
        c.a = ca;
        c.d = cd;
    }
    else
    {
        c.a = -1;
        c.d = -1;
    }
    return c;
}
bool comp_length(string a, string b)
{
    return (a.length()<b.length());
}
void dict_init()//initialize the dictionary from file
{
    ifstream dict;
    string temp;
    dict.open("English words 3000.txt");
    for(int i=0;i<DICTSIZE;i++)
    {
        dict >> temp;
        dictionary.push_back(temp);
        index[temp.length()]++;
    }
    dict.close();
    for(int i=1;i<MAX_LENGTH;i++)
    {
        index[i]+=index[i-1];
        //cout << index[i-1] << endl;
    }
    //sort the dictionary by length, calculate the number of words for each length.
    //the index will help trimming the domain by unary constrain(variable length limit).
    stable_sort(dictionary.begin(),dictionary.end(),comp_length);
    /*for(int i=1;i<MAX_LENGTH;i++)
    {
        if(index[i] < DICTSIZE && index[i] > 0)
        {
            cout << dictionary[index[i-1]] << endl;
            cout << dictionary[index[i]-1] << endl;
        }
        else
            cout << '0' << endl;
    }*/
}
bool AC3(vector<vector<int> > &domain, vector<constrain> c)
{
    queue<constrain> working;
    vector<constrain> done;
    for(int i=0;i<c.size();i++)
    {
        working.push(c[i]);
    }
    while(working.size())
    {
        constrain con = working.front();
        bool a_dirty = false;
        bool d_dirty = false;
        for(int i=0;i<domain[con.i_a].size();i++)
        {

            bool paired = false;
            //looking for answer in domain d
            for(int j=0;j<domain[con.i_d].size();j++)
            {
                if(dictionary[domain[con.i_a][i]][con.a] == dictionary[domain[con.i_d][j]][con.d])
                {
                    paired = true;
                    break;
                }
            }
            if(!paired)
            {
                domain[con.i_a].erase(domain[con.i_a].begin()+i);
                i--;
                a_dirty = true;
            }
        }
        for(int i=0;i<done.size();i++)
        {
            if(a_dirty && done[i].i_a == con.i_a)
            {

                working.push(done[i]);
                done.erase(done.begin()+i);
                i--;
            }
        }
        for(int i=0;i<domain[con.i_d].size();i++)
        {
            bool paired = false;
            //looking for answer in domain d
            for(int j=0;j<domain[con.i_a].size();j++)
            {
                if(dictionary[domain[con.i_a][j]][con.a] == dictionary[domain[con.i_d][i]][con.d])
                {
                    paired = true;
                    break;
                }
            }
            if(!paired)
            {
                domain[con.i_d].erase(domain[con.i_d].begin()+i);
                i--;
                d_dirty = true;
            }
        }
        for(int i=0;i<done.size();i++)
        {
            if(d_dirty && done[i].i_d == con.i_d)
            {

                working.push(done[i]);
                done.erase(done.begin()+i);
                i--;
            }
        }
        working.pop();
        done.push_back(con);
    }
    for(int i=0;i<domain.size();i++)
    {
        if(domain[i].empty())
        {
            return false;
        }
    }
    return true;
};
void print_result(int x, int y, vector<assignment> path, vector<var> puzzle)
{
    vector<string> answer;
    for(int i=0;i<y;i++)
    {
        string temp;
        temp.resize(x*2,' ');
        answer.push_back(temp);
    }

    for(int i=0;i<path.size();i++)
    {
        if(puzzle[path[i].var].dir == 'A')
        {
            for(int j=0;j<puzzle[path[i].var].length;j++)
            {
                answer[puzzle[path[i].var].y][2*(puzzle[path[i].var].x+j)] = dictionary[path[i].index][j];
            }
        }
        else
        {
            for(int j=0;j<puzzle[path[i].var].length;j++)
            {
                answer[puzzle[path[i].var].y+j][2*(puzzle[path[i].var].x)] = dictionary[path[i].index][j];
            }
        }
    }
    for(int i=0;i<y;i++)
    {
        cout << answer[i] << endl;
    }
}
void solve(string problem)
{
    //input variables and generate binary constrains;
    stringstream ss;
    vector<var> variables;
    vector<constrain> intersect;//stores all intersections.
    int x,y,l;
    int map_x=0,map_y=0;
    char d;
    ss << problem;
    while(ss >> x >> y >> l >>d)
    {
        var v;
        v.x=x;
        v.y=y;
        v.length=l;
        v.dir=d;
        //cout << x << ' ' << y << ' ' << l << ' ' << d << endl;
        variables.push_back(v);
        for(int j=0;j<variables.size();j++)
        {
            if(d != variables[j].dir)
            {
                if(d == 'A')
                {
                    if(map_x < x+l)
                        map_x = x+l;
                    constrain c = check(v, variables[j]);
                    if(c.a != -1)
                    {
                        c.i_a = variables.size()-1;
                        c.i_d = j;
                        intersect.push_back(c);
                    }
                }
                else
                {
                    if(map_y < y+l)
                        map_y = y+l;
                    constrain c = check(variables[j], v);
                    if(c.a != -1)
                    {
                        c.i_a = j;
                        c.i_d = variables.size()-1;

                        intersect.push_back(c);
                    }
                }
            }
        }
    }
    ss.str("");
    ss.clear();

    //search

    stack<node> backsearch;
    //initialize the root;
    int Size = variables.size();
    node root;
    root.domain.resize(Size);
    for(int i=0;i<Size;i++)
    {
        for(int j=index[variables[i].length-1];j<index[variables[i].length];j++)
        {
            root.domain[i].push_back(j);
        }
        //cout << root.domain[i].size() << endl;
    }
    //AC3(root.domain, intersect);
    backsearch.push(root);
    int visit_node =0;
    while(backsearch.size())
    {
        visit_node++;
        node expand = backsearch.top();
        backsearch.pop();
        if(!AC3(expand.domain,intersect))
        {
            continue;
        }
        if(expand.path.size() == Size)
        {
            cout << "Nodes expanded: " << visit_node << endl;
            /*for(int i=0;i<expand.path.size();i++)
            {
                cout << expand.path[i].var << ' ' << dictionary[expand.path[i].index] << endl;
            }*/
            print_result(map_x,map_y,expand.path,variables);
            return;
        }
        //MRV heuristic
        vector<pair<int,int> > visit_order;
        for(int i=0;i<Size;i++)
        {
            bool visited = false;
            for(int j=0;j<expand.path.size();j++)
            {
                if(i == expand.path[j].var)
                {
                    visited = true;
                }
            }
            if(visited)
            {
                continue;
            }
            else
            {
                pair<int,int> temp_v(expand.domain[i].size(),i);
                visit_order.push_back(temp_v);
            }
        }
        sort(visit_order.begin(),visit_order.end(),greater<pair<int,int> >());
        for(int i=0;i<visit_order.size();i++)
        {
            //cout << i << ' ' << expand.domain[i].size() << endl;
            for(int j=0;j<expand.domain[visit_order[i].second].size();j++)
            {
                //cout << dictionary[expand.domain[i][j]] << endl;
                assignment a;
                a.index=expand.domain[visit_order[i].second][j];
                a.var = visit_order[i].second;
                node newnode = expand;
                newnode.a = a;
                newnode.domain[visit_order[i].second].clear();
                newnode.domain[visit_order[i].second].push_back(expand.domain[visit_order[i].second][j]);
                newnode.path.push_back(a);
                backsearch.push(newnode);
            }
        }
    }
}
int main()
{
    dict_init();
    ifstream puzzle;
    string problem;
    puzzle.open("puzzle.txt");
    int problem_count = 0;
    while(getline(puzzle, problem))
    {
        cout << ++problem_count << endl;
        solve(problem);
    }
    return 0;
}
