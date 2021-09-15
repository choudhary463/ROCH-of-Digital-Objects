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
    }
};
int dx[] = {0, -1, 0, 1};
int dy[] = {1, 0, -1, 0};
vertex node[N*N];
vector<vertex>IIC, OIC;

void cng(Mat &M, int i1,int i2,int j1,int j2) {
    if(i1>i2) swap(i1,i2);
    if(j1>j2) swap(j1,j2);
    for(int i=i1;i<=i2;i++){
        for(int j=j1;j<=j2;j++){
            M.at<uint8_t>(i,j) = 0;
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
        node[x * N + y].oic_type = type;
        if(type == 1 || type == 3 || type == 4){
            if(type == 4) type = 2;
            current_direction = (current_direction + type) % 4;
        }
        int new_x = x + g*dx[current_direction];
        int new_y = y + g*dy[current_direction];
        next[N*x + y] = N*new_x + new_y;
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
        node[x * N + y].iic_type = type;
        if(type == 1 || type == 3 || type == 4){
            if(type == 4) type = 2;
            current_direction = (current_direction + type) % 4;
        }
        int new_x = x + g*dx[current_direction];
        int new_y = y + g*dy[current_direction];
        next[N*x + y] = N*new_x + new_y;
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
}
int main(int argc, char** argv )
{

    Mat image;
    image = imread("C:\\Users\\anmol choudhary\\Documents\\BTP\\image.png");
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    vector<Mat> channels(3);
    Mat M;
    resize(image, M, Size(500, 500));
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
    int g = 4;
    construct_OIC(M, g);
    construct_IIC(M, g);
    cerr << OIC.size() << ' ' << IIC.size() << '\n';
    return 0;
}