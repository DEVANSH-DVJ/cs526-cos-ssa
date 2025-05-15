#include <bits/stdc++.h>
using namespace std;

extern "C" {
char arr[15][15];
int r1,c1,r2,c2,a,b,c,d,rdir,cdir,R,C,i,j,sr1,sc1,sr2,sc2,er1,ec1,er2,ec2,newr1,newc1,newr2,newc2,newa,newb,newc,newd,x,x1,x2;
long long dist[2000005],vi[2000005],ans;
vector<long long> v[2000005];
priority_queue<pair<long long,long long>,vector<pair<long long,long long>>,greater<pair<long long,long long>>> q;
const long long INF=1e16;

bool isObstacle(int rpos, int cpos, int a, int b, int c, int d) {
	if (arr[rpos][cpos]=='#') {
		return true;
	}
	if (arr[rpos][cpos]=='A' && a==0) {
		return true;
	}
	if (arr[rpos][cpos]=='a' && a==1) {
		return true;
	}
	if (arr[rpos][cpos]=='B' && b==0) {
		return true;
	}
	if (arr[rpos][cpos]=='b' && b==1) {
		return true;
	}
	if (arr[rpos][cpos]=='C' && c==0) {
		return true;
	}
	if (arr[rpos][cpos]=='c' && c==1) {
		return true;
	}
	if (arr[rpos][cpos]=='D' && d==0) {
		return true;
	}
	if (arr[rpos][cpos]=='d' && d==1) {
		return true;
	}
	return false;
}

int isSwitch(int rpos, int cpos, int a, int b, int c, int d) {
	if (arr[rpos][cpos]=='1') {
		return 1;
	}
	if (arr[rpos][cpos]=='2') {
		return 2;
	}
	if (arr[rpos][cpos]=='3') {
		return 3;
	}
	if (arr[rpos][cpos]=='4') {
		return 4;
	}
	return 0;
}

bool cannotMoveToSwitch(int newr1, int newr2, int newc1, int newc2, int newa, int newb, int newc, int newd, int x1, int x2) {
	if (x1==1 && ((arr[newr2][newc2]=='A' && newa==0) || (arr[newr2][newc2]=='a' && newa==1)))  {
		return true;
	}
	if (x1==2 && ((arr[newr2][newc2]=='B' && newb==0) || (arr[newr2][newc2]=='b' && newb==1)))  {
		return true;
	}
	if (x1==3 && ((arr[newr2][newc2]=='C' && newc==0) || (arr[newr2][newc2]=='c' && newc==1)))  {
		return true;
	}
	if (x1==4 && ((arr[newr2][newc2]=='D' && newd==0) || (arr[newr2][newc2]=='d' && newd==1)))  {
		return true;
	}
	if (x2==1 && ((arr[newr1][newc1]=='A' && newa==0) || (arr[newr2][newc2]=='a' && newa==1)))  {
		return true;
	}
	if (x2==2 && ((arr[newr1][newc1]=='B' && newb==0) || (arr[newr2][newc2]=='b' && newb==1)))  {
		return true;
	}
	if (x2==3 && ((arr[newr1][newc1]=='C' && newc==0) || (arr[newr2][newc2]=='c' && newc==1)))  {
		return true;
	}
	if (x2==4 && ((arr[newr1][newc1]=='D' && newd==0) || (arr[newr2][newc2]=='d' && newd==1)))  {
		return true;
	}
	return false;
}

int hashh(int r1, int c1, int r2, int c2, int a, int b, int c, int d) {
	int h1=8*a+4*b+2*c+d;
	return (r1*11*11*11*11+c1*11*11*11+r2*11*11+c2*11)*11+h1;
}

int main() {
	cin >> R >> C;
	for (i=1;i<=R;i++) {
		for (j=1;j<=C;j++) {
			cin >> arr[i][j];
			if (arr[i][j]=='S' && sr1==0) {
				sr1=i;
				sc1=j;
			}
			else if (arr[i][j]=='S') {
				sr2=i;
				sc2=j;
			}
			if (arr[i][j]=='E' && er1==0) {
				er1=i;
				ec1=j;
			}
			else if (arr[i][j]=='E') {
				er2=i;
				ec2=j;
			}
		}
	}
	for (i=0;i<=C+1;i++) {
		arr[0][i]='*';
		arr[R+1][i]='*';
	}
	for (i=0;i<=R+1;i++) {
		arr[i][0]='*';
		arr[i][C+1]='*';
	}
	for (r1=1;r1<=R;r1++) {
		for (c1=1;c1<=C;c1++) {
			for (r2=1;r2<=R;r2++) {
				for (c2=1;c2<=C;c2++) {
					for (a=0;a<=1;a++) {
						for (b=0;b<=1;b++) {
							for (c=0;c<=1;c++) {
								for (d=0;d<=1;d++) {
									for (rdir=-1;rdir<=1;rdir++) {
										for (cdir=-1;cdir<=1;cdir++) {
											if ((rdir==0 && cdir==0) || rdir!=0 && cdir!=0) {
												continue;
											}
											if (arr[r1+rdir][c1+cdir]=='*' || arr[r2+rdir][c2+cdir]=='*') {
												continue;
											}
											newa=a;
											newb=b;
											newc=c;
											newd=d;
											if (isObstacle(r1+rdir,c1+cdir,a,b,c,d)) {
												newr1=r1;
												newc1=c1;
											}
											else {
												newr1=r1+rdir;
												newc1=c1+cdir;
												x1=isSwitch(r1+rdir,c1+cdir,a,b,c,d);
												if (x1==1) {
													newa=1-a;
												}
												else if (x1==2) {
													newb=1-b;
												}
												else if (x1==3) {
													newc=1-c;
												}
												else if (x1==4) {
													newd=1-d;
												}
											}
											if (isObstacle(r2+rdir,c2+cdir,a,b,c,d)) {
												newr2=r2;
												newc2=c2;
											}
											else {
												newr2=r2+rdir;
												newc2=c2+cdir;
												x2=isSwitch(r2+rdir,c2+cdir,a,b,c,d);
												if (x2==1) {
													newa=1-a;
												}
												else if (x2==2) {
													newb=1-b;
												}
												else if (x2==3) {
													newc=1-c;
												}
												else if (x2==4) {
													newd=1-d;
												}
											}
//											if (a==)
											if (newc1==newc2 && newr1==newr2) {
												continue;
											}
											if (cannotMoveToSwitch(newr1,newr2,newc1,newc2,newa,newb,newc,newd,x1,x2)) {
												continue;
											}
											v[hashh(r1,c1,r2,c2,a,b,c,d)].push_back(hashh(newr1,newc1,newr2,newc2,newa,newb,newc,newd));
//											cout << hashh(r1,c1,r2,c2,a,b,c,d) << ' ' << hashh(newr1,newc1,newr2,newc2,newa,newb,newc,newd) << endl;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (i=1;i<=2000000;i++) {
		dist[i]=INF;
	}
//	cout << sr1 << ' ' << sc1 << ' ' << sr2 << ' ' << sc2 << endl;
	dist[hashh(sr1,sc1,sr2,sc2,0,0,0,0)]=0;
	q.push({0,hashh(sr1,sc1,sr2,sc2,0,0,0,0)});
	while(!q.empty()) {
		x=q.top().second;
		q.pop();
		if (vi[x]==1) {
			continue;
		}
		vi[x]=1;
		for (i=0;i<v[x].size();i++) {
			if (dist[v[x][i]]>dist[x]+1) {
				dist[v[x][i]]=dist[x]+1;
				q.push({dist[v[x][i]],v[x][i]});
			}
		}
	}
	ans=INF;
	for (a=0;a<=1;a++) {
		for (b=0;b<=1;b++) {
			for (c=0;c<=1;c++) {
				for (d=0;d<=1;d++) {
					ans=min(ans,dist[hashh(er1,ec1,er2,ec2,a,b,c,d)]);
					ans=min(ans,dist[hashh(er2,ec2,er1,ec1,a,b,c,d)]);
				}
			}
		}
	}
//	cout << dist[hashh(5,1,5,2,0,0,0,0)] << endl;
//	cout << dist[hashh(4,1,4,2,0,0,1,0)] << endl;
//	for (i=0;i<v[hashh(4,1,4,2,0,0,1,0)].size();i++) {
//		cout << v[hashh(4,1,4,2,0,0,1,0)][i] << ' ';
//	}
//	cout << endl;
//	cout << hashh(3,1,3,2,0,0,1,0) << ' ' << dist[hashh(3,1,3,2,0,0,1,0)] << endl;
//	cout << dist[hashh(2,1,2,2,0,0,1,0)] << endl;
//	cout << dist[hashh(2,1,1,2,0,0,1,0)] << endl;
	cout << ans << endl;
	
}
}
