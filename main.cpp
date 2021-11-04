#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;


const int N = 500;

struct vertex{
    int x, y, iic_next, oic_next, val, iic_type, oic_type;
    vertex(){}
    vertex(int a, int b){
        this -> x = a;
        this -> y = b;
        this -> val = a * N + b;
        this->iic_next = 0;
    }
};
int dx[] = {0, -1, 0, 1};
int dy[] = {1, 0, -1, 0};
vertex node[N*N];
vector<vertex>IIC, OIC, ROCH;

void cng(Mat &M, int i1,int i2,int j1,int j2,int x) {
    if(i1>i2) swap(i1,i2);
    if(j1>j2) swap(j1,j2);
    assert(i1==i2||j1==j2);
    for(int i=i1;i<=i2;i++){
        for(int j=j1;j<=j2;j++){
            M.at<uint8_t>(i,j) = x;
        }
    }
}
void cng2(Mat &M, int i1,int i2,int j1,int j2,vector<int>v) {
    if(i1>i2) swap(i1,i2);
    if(j1>j2) swap(j1,j2);
    assert(i1==i2||j1==j2);
    for(int i=i1;i<=i2;i++){
        for(int j=j1;j<=j2;j++){
            Vec3b & color = M.at<Vec3b>(i,j);
            for(int k=0;k<3;k++){
                color[k]=v[k];
            }
        }
    }
}

bool is_all_white(int i1, int i2, int j1, int j2, Mat&M) {
    i1 = max(i1, 0);
    i1 = min(N - 1, i1);
    i2 = max(i2, 0);
    i2 = min(N - 1, i2);
    j1 = max(j1, 0);
    j1 = min(N - 1, j1);
    j2 = max(j2, 0);
    j2 = min(N - 1, j2);
    for(int i = i1; i <= i2; i++) {
        for(int j = j1; j<= j2; j++) {
            if(M.at<uint8_t>(i, j) != 255) return false;
        }
    }
    return true;
}
bool is_all_black(int i1, int i2, int j1, int j2, Mat&M) {
    if(min({i1, i2, j1, j2}) < 0 || max({i1, i2, j1, j2}) >= N) return false;
    for(int i = i1; i <= i2; i++) {
        for(int j = j1; j<= j2; j++) {
            if(M.at<uint8_t>(i, j) == 255) return false;
        }
    }
    return true;
}
int OIC_type(int i, int j, Mat& M, int g) {
    int res = is_all_white(i - g , i, j, j + g, M);
    res += is_all_white(i - g, i, j - g, j, M);
    res += is_all_white(i, i + g, j - g, j, M);
    res += is_all_white(i, i + g, j, j + g, M);
    return 4 - res;
}
int IIC_type(int i, int j, Mat& M, int g) {
    int res = is_all_black(i - g , i, j, j + g, M);
    res += is_all_black(i - g, i, j - g, j, M);
    res += is_all_black(i, i + g, j - g, j, M);
    res += is_all_black(i, i + g, j, j + g, M);
    return res;
}
int dir(int st, int en){
    if(node[st].x==node[en].x){
        if(node[st].y<node[en].y) return 0;
        else return 2;
    }
    else if(node[st].y==node[en].y){
        if(node[st].x<node[en].x) return 3;
        else return 1;
    }
    else{
        assert(0);
        return 0;
    }
}
void construct_OIC(Mat& M, int g) {
    vector<int> next(N*N);
    int top_left_x = N;
    int top_left_y = N;
    for(int i = 0; i < N; i+=g) {
        for(int j = 0; j < N; j+=g) {
            if(OIC_type(i, j, M, g) == 1) {
                top_left_x = i;
                top_left_y = j;
                break;
            }
        }
        if(top_left_x != N) break;
    }
    int x = top_left_x;
    int y = top_left_y;
    int current_direction = 2;
    int total = 0;
    while((x != top_left_x || y!=top_left_y)||!total){
        total ++;
        int type = OIC_type(x, y, M ,g);
        if(type == 0) type = 4;
        node[x * N + y].oic_type = type;
        if(type == 1 || type == 3 || type == 4){
            if(type == 4) type = 2;
            current_direction = (current_direction + type) % 4;
        }
        int new_x = x + g * dx[current_direction];
        int new_y = y + g * dy[current_direction];
        next[N * x + y] = N * new_x + new_y;
        x = new_x;
        y = new_y;
    }
    x = top_left_x;
    y = top_left_y;
    total = 0;
    while(x != top_left_x || y != top_left_y || !total){
        node[x * N + y].oic_next = next[x * N + y];
        OIC.push_back(node[x * N + y]);
        int new_x = next[x * N + y] / N;
        int new_y = next[x * N + y] % N;
        x = new_x;
        y = new_y;
        total ++;
    }
    int n=(int)(OIC.size());
    for(int i=0;i<n;i++){
        int l=(i-1+n)%n;
        int r=(i+1)%n;
        int le=dir(OIC[l].val,OIC[i].val);
        int ri=dir(OIC[i].val,OIC[r].val);
        OIC[i].oic_type=(ri-le+4)%4;
    }
}
void construct_IIC(Mat &M, int g) {
    vector<int> next(N*N);
    int top_left_x = N;
    int top_left_y = N;
    for(int i = 0; i < N; i+=g) {
        for(int j = 0; j < N; j+=g) {
            if(IIC_type(i, j, M, g) == 1) {
                top_left_x = i;
                top_left_y = j;
                break;
            }
        }
        if(top_left_x != N) break;
    }
    int x = top_left_x;
    int y = top_left_y;
    int current_direction = 2;
    int total = 0;
    while((x != top_left_x || y!=top_left_y)||!total){
        total ++;
        int type = IIC_type(x, y, M ,g);
        if(type == 0) type = 4;
        node[x * N + y].iic_type = type;
        if(type == 1 || type == 3 || type == 4){
            if(type == 4) type = 2;
            current_direction = (current_direction + type) % 4;
        }
        int new_x = x + g * dx[current_direction];
        int new_y = y + g * dy[current_direction];
        next[N * x + y] = N * new_x + new_y;
        x = new_x;
        y = new_y;
    }
    x = top_left_x;
    y = top_left_y;
    total = 0;
    while(x != top_left_x || y != top_left_y || !total){
        node[x * N + y].iic_next = next[x * N + y];
        IIC.push_back(node[x * N + y]);
        int new_x = next[x * N + y] / N;
        int new_y = next[x * N + y] % N;
        x = new_x;
        y = new_y;
        total ++;
    }
    int n=(int)(IIC.size());
    for(int i=0;i<n;i++){
        int l=(i-1+n)%n;
        int r=(i+1)%n;
        int le=dir(IIC[l].val,IIC[i].val);
        int ri=dir(IIC[i].val,IIC[r].val);
        IIC[i].iic_type=(ri-le+4)%4;
    }
}
void show_OIC(Mat M1, int g){
    Mat M = M1.clone();
    int n = (int)OIC.size();
    for(int i = 0; i < n; i++){
        cng(M, OIC[i].x, OIC[(i+1)%n].x, OIC[i].y, OIC[(i+1)%n].y, 0);
    }
    imshow("OIC", M);
    waitKey(10);
}
void show_IIC(Mat M1, int g){
    Mat M = M1.clone();
    int n = (int)IIC.size();
    for(int i = 0; i < n; i++){
        cng(M, IIC[i].x, IIC[(i+1)%n].x, IIC[i].y, IIC[(i+1)%n].y, 255);
    }
    imshow("IIC", M);
    waitKey(10);
}
void show_combined(Mat M1, int g){
    Mat M = M1.clone();
    int n = (int)IIC.size();
    Mat colM = Mat::zeros(N,N,CV_8UC3);
   16 vector<Mat>planes;
    for(int i=0;i<3;i++){
        planes.push_back(M);
    }
    merge(planes,colM);
    for(int i = 0; i < n; i++){
        cng2(colM, IIC[i].x, IIC[(i+1)%n].x, IIC[i].y, IIC[(i+1)%n].y, {255,0,0});
    }
    n=(int)(OIC.size());
    for(int i = 0; i < n; i++){
        cng2(colM, OIC[i].x, OIC[(i+1)%n].x, OIC[i].y, OIC[(i+1)%n].y, {0,0,0});
    }
    n=(int)(ROCH.size());
    for(int i = 0; i < n; i++){
        cng2(colM, ROCH[i].x, ROCH[(i+1)%n].x, ROCH[i].y, ROCH[(i+1)%n].y, {0,0,255});
    }
    imshow("ROCH", colM);
    waitKey(0);
}
void mark(Mat M1,vector<int>v){
    Mat M = M1.clone();
    int n=(int)(v.size());
    for(int i=0;i<n-1;i++){
        int x=v[i];
        int y=v[i+1];
        cng(M,node[x].x,node[y].x,node[x].y,node[y].y,150);
    }
    imshow("ROCH",M);
    waitKey(0);
}
pair<int,int>pa(int a){
    return make_pair(node[a].x,node[a].y);
}
int find(int st,int en,int g,vector<bool>&visI,vector<bool>&visO){
    int x1=node[st].x;
    int x2=node[en].x;
    int y1=node[st].y;
    int y2=node[en].y;
    if(x1==x2){
        for(int i=min(y1,y2)+g;i<=max(y1,y2)-g;i+=g){
            if(visI[N*x1+i]){
                return 1;
            }
        }
        for(int i=min(y1,y2)+g;i<=max(y1,y2)-g;i+=g){
            if(visO[N*x1+i]){
                return 2;
            }
        }
        return 0;
    }
    else if(y1==y2){
        for(int i=min(x1,x2)+g;i<=max(x1,x2)-g;i+=g){
            if(visI[i*N+y1]){
                return 1;
            }
        }
        for(int i=min(x1,x2)+g;i<=max(x1,x2)-g;i+=g){
            if(visO[i*N+y1]){
                return 2;
            }
        }
        return 0;
    }
    else{
        assert(0);
        return -1;
    }
}
void sett(int st,int en,int g,bool p,vector<bool>&vis){
    int x1=node[st].x;
    int y1=node[st].y;
    int x2=node[en].x;
    int y2=node[en].y;
    if(x1>x2) swap(x1,x2);
    if(y1>y2) swap(y1,y2);
    for(int i=x1;i<=x2;i+=g){
        for(int j=y1;j<=y2;j+=g){
            vis[N*i+j]=p;
        }
    }
}
void setv(vector<int>v,int g,bool p,vector<bool>&vis){
    int n=(int)v.size();
    for(int i=0;i<n-1;i++){
        sett(v[i],v[i+1],g,p,vis);
    }
}
int type3(int a,int b,int c,int d,int st,int en,int prr,int nee,int g,vector<int>&prev,vector<int>&next,vector<int>&typ,vector<bool>&visI,vector<bool>&visO, Mat &M){
    int la=-1;
    assert(find(st,en,g,visI,visO)==2);
    if(abs(node[c].x-node[d].x) !=0){
        if(node[c].x<node[d].x){
            while(find(st-N*g,en-N*g,g,visI,visO)==2){
                st-=N*g;
                en-=N*g;
            }
            assert(st<en);
            for(int i=st;i<=en;i+=g){
                if(visO[i]){
                    la=i;
                }
            }
        }
        else{
            while(find(st+N*g,en+N*g,g,visI,visO)==2){
                st+=N*g;
                en+=N*g;
            }
            assert(st>en);
            for(int i=st;i>=en;i-=g){
                if(visO[i]){
                    la=i;
                }
            }
        }
    }
    else{
        if(node[c].y<node[d].y){
              while(find(st-g,en-g,g,visI,visO)==2){
                st-=g;
                en-=g;
            }
            assert(st>en);
            for(int i=st;i>=en;i-=N*g){
                if(visO[i]){
                    la=i;
                }
            }
        }
        else{
            while(find(st+g,en+g,g,visI,visO)==2){
                st+=g;
                en+=g;
            }
            assert(st<en);
            for(int i=st;i<=en;i+=N*g){
                bool pp=visO[i];
                if(visO[i]){
                    la=i;
                }
            }
        }
    }
    assert(la!=-1);
    setv({prr,a,b,c,d,nee},g,false,visI);
    setv({prr,a,st,la,en,d,nee},g,true,visI);
    next[prr]=a;
    next[a]=st;
    next[st]=la;
    next[la]=en;
    next[en]=d;
    next[d]=nee;
    prev[a]=prr;
    prev[st]=a;
    prev[la]=st;
    prev[en]=la;
    prev[d]=en;
    prev[nee]=d;
    typ[st]=1;
    typ[la]=5;
    typ[en]=3;
    return prr;
}
void stt(int prr,int a,int b,int c,int d,int nee,vector<int>&next,vector<int>&prev,vector<int>&typ){
    next[prr]=0;
    next[a]=0;
    next[b]=0;
    next[c]=0;
    next[d]=0;
    prev[a]=0;
    prev[b]=0;
    prev[c]=0;
    prev[d]=0;
    typ[b]=0;
    typ[c]=0;
    prev[nee]=0;
}
int merge(int a, int b, int c, int d, int g, vector<int> &prev, vector<int> &next, vector<int>&typ ,Mat &M,vector<bool>&visI,vector<bool>&visO){
    int len1 = abs(node[a].x - node[b].x) + abs(node[a].y - node[b].y);
    int len2 = abs(node[c].x - node[d].x) + abs(node[c].y - node[d].y);
    int prr=prev[a];
    int nee=next[d];
    if(len1 < len2) {
        int st = a;
        int en = 0;
        if(abs(node[c].x-node[d].x) == len2){
            if(node[c].x<node[d].x){
                en = (node[c].x + len1) * N + node[c].y;
            }
            else{
                en = (node[c].x - len1) * N + node[c].y;
            }
        }
        else{
            if(node[c].y<node[d].y){
                en = node[c].x * N + node[c].y + len1;
            }
            else{
                en = node[c].x * N + node[c].y - len1;
            }
        }
        assert(st/N==en/N||st%N==en%N);
        int type=find(st,en,g,visI,visO);
        if(type==1) return c;
        stt(prr,a,b,c,d,nee,next,prev,typ);
        if(type==0){
            setv({prr,a,b,c,d,nee},g,false,visI);
            setv({prr,en,d,nee},g,true,visI);
            next[prr]=en;
            next[en]=d;
            next[d]=nee;
            prev[en]=prr;
            prev[d]=en;
            prev[nee]=d;
            typ[en]=3;
            typ[a]=0;
            return prr;
        }
        return type3(a,b,c,d,st,en,prr,nee,g,prev,next,typ,visI,visO,M);
    }
    else if(len1 > len2) {
        int st = 0;
        int en = d;
        if(abs(node[b].x-node[a].x) == len1){
            if(node[b].x<node[a].x){
                st = (node[b].x + len2) * N + node[b].y;
            }
            else{
                st = (node[b].x - len2) * N + node[b].y;
            }
        }
        else{
            if(node[b].y<node[a].y){
                st = node[b].x * N + node[b].y + len2;
            }
            else{
                st = node[b].x * N + node[b].y - len2;
            }
        }
        assert(st/N==en/N||st%N==en%N);
        int type=find(st,en,g,visI,visO);
        if(type==1) return c;
        stt(prr,a,b,c,d,nee,next,prev,typ);
        if(type==0){
            setv({prr,a,b,c,d,nee},g,false,visI);
            setv({prr,a,st,nee},g,true,visI);
            next[prr]=a;
            next[a]=st;
            next[st]=nee;
            prev[a]=prr;
            prev[st]=a;
            prev[nee]=st;
            typ[st]=3;
            typ[d]=0;
            return prr;
        }
        return type3(a,b,c,d,st,en,prr,nee,g,prev,next,typ,visI,visO,M);
    }
    else{
        int st = a;
        int en = d;
        assert(st/N==en/N||st%N==en%N);
        int type=find(st,en,g,visI,visO);
        if(type==1) return c;
        stt(prr,a,b,c,d,nee,next,prev,typ);
        if(type==0){
            setv({prr,a,b,c,d,nee},g,false,visI);
            setv({prr,nee},g,true,visI);
            next[prr]=nee;
            prev[nee]=prr;
            typ[a]=0;
            typ[d]=0;
            return prr;
        }
        return type3(a,b,c,d,st,en,prr,nee,g,prev,next,typ,visI,visO,M);
    }
}
void makeVis(vector<bool>&vis,int x1,int y1,int x2,int y2,int g){
    if(x1>x2) swap(x1,x2);
    if(y1>y2) swap(y1,y2);
    for(int i=x1;i<=x2;i+=g){
        for(int j=y1;j<=y2;j+=g){
            vis[i*N+j]=true;
        }
    }
}
void traverseIIC(vector<bool>&vis,int g){
    int n=(int)(IIC.size());
    for(int i=0;i<n;i++){
        makeVis(vis,IIC[i].x,IIC[i].y,IIC[(i+1)%n].x,IIC[(i+1)%n].y,g);
    }
}
void traverseOIC(vector<bool>&vis,int g){
    int n=(int)(OIC.size());
    for(int i=0;i<n;i++){
        makeVis(vis,OIC[i].x,OIC[i].y,OIC[(i+1)%n].x,OIC[(i+1)%n].y,g);
    }
}
int chk(int st,vector<int>&next,vector<int>&type){
    int tot=0;
    int idx=st;
    while(!(tot&&idx==st)){
        tot++;
        if(type[idx]==3&&type[next[idx]]==3){
            return idx;
        }
        idx=next[idx];
    }
    return -1;
}
void construct_ROCH(Mat &M, int g){
    vector<int>prev(N*N),next(N*N),type(N*N);
    vector<bool>visI(N*N),visO(N*N);
    traverseIIC(visI,g);
    traverseOIC(visO,g);
    int n=(int)(IIC.size());
    int start=IIC[0].val;
    type[start]=IIC[0].iic_type;
    vector<int>v;
    v.push_back(start);
    for(int i=1;i<n;i++){
        if(IIC[i].iic_type!=1&&IIC[i].iic_type!=3) continue;
        int idx=IIC[i].val;
        type[idx]=IIC[i].iic_type;
        prev[idx]=v.back();
        next[v.back()]=idx;
        v.push_back(IIC[i].val);
    }
    next[v.back()]=IIC[0].val;
    prev[IIC[0].val]=v.back();
    int st=chk(IIC[0].val,next,type);
    int la=start;
    int tot=0;
    while(st!=-1){
        st=merge(prev[st],st,next[st],next[next[st]],g,prev,next,type,M,visI,visO);
        la=st;
        st=chk(st,next,type);
        if(tot==1000) break;
        tot++;
    }
    tot=0;
    int cur=la;
    while(!(tot&&(cur==la))){
        tot++;
        ROCH.push_back(node[cur]);
        cur=next[cur];
    }
}
int main(int argc, char** argv )
{

    Mat image;
    image = imread("C:\\Users\\anmol choudhary\\Documents\\BTP\\image2.png");
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    vector<Mat> channels(3);
    Mat M;
    resize(image, M, Size(N, N));
    split(M, channels);
    M = channels[0];

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            node[i * N + j] = vertex(i, j);
            if(M.at<uint8_t>(i, j)!=255){
                M.at<uint8_t>(i, j) = 0;
            }
        }
    }
    int g = 7;
    construct_OIC(M, g);
    construct_IIC(M, g);
    show_OIC(M, g);
    show_IIC(M, g);
    construct_ROCH(M, g);
    show_combined(M, g);
    return 0;
}