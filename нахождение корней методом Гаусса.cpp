#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
int main(){
	vector<vector <double> > a;
	int n; //������� ������� ���������
	int i,j,e,l; //����
	double v1,v2; //��������� ���������
	double xproverka; //�������� ��� ���������� ����� � ���������� ��������

	cin>>n;
	a.resize(n+1);
	for(i=0;i!=n+1;i++){
		a[i].resize(n);
	}
	//loading
	for(i=0;i!=n;i++){
        for(j=0;j!=n+1;j++){
            cin>>a[j][i];
        }
    }
	//����� ������
	//  for(j=0;j!=n;j++)  for(e=0;e!=n-1;e++) for(l=0;l!=n-2;l++) for(j=0;j!=n-1;j++)
	//for(j=0;j!=n-1;j++){
		for(l=0;l!=n-1;l++){
			for(e=l+1;e!=n;e++){
					v1 = a[l][l]; //��������� ��������� l ����� l ������
					v2 = a[l][e]; //��������� ��������� l ����� e=l+1 ������
					if((v1!=0) && (v2!=0)){
						for(i=0;i!=n+1;i++){ //��������� �� �����
							a[i][l]=a[i][l]*v2;
							a[i][e]=a[i][e]*v1;
						}
						for(i=0;i!=n+1;i++){ //�������� ������
							a[i][e] -= a[i][l];
						}
					}
			}
		}
//	}

	/*
	xproverka = a[n][n-1]/a[n-1][n-1];
	cout<<xproverka<<" ";
	*/

	vector<double> x(n);
	for(l=n-1;l!=-1;l--){
		x[l]=a[n][l]/a[l][l]; //����� ������ �����
		//�������� �������� ������ ������
		for(j=n-2;j!=-1;j--){
			a[n][j] -= a[l][j]*x[l];
		}
	}
	//������� �������� ������
	for(l=0;l!=n;l++){
		cout<<x[l]<<" ";
	}
	system("PAUSE");
}