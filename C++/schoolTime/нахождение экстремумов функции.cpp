#include <iostream>
#include <cmath>

using namespace std;
//������ ��������� ���� ���������� ������� � ���������� � ������������� ������������
//� ����� �� ��� - �������� ��� ������� 
double F(double x) {
	return x*x; //�������������� �������, �� ������������
}
double f(double x) {
	return 2*x; //����������� �������
}

int main(){
	double a,b,h,xi,e;
	double n; //���������� ��������� �� ������� ������� ������� � ������������
	cout<<" vvedite a "<<endl; //������ �������
	cin>>a;
	cout<<" vvedite b "<<endl; //����� �������
	cin>>b;
	cout<<" vvedite e "<<endl; //�����������
	cin>>e;
	cout<<" vvedite n "<<endl; //����� ���������� �� ������� ������� �������
	cin>>n;
	xi=a; //��������� ����� ����� �������� ���������� �� ������� (���� �� ������� � ������� ���������)
	if (f(a)==0){ cout<<"extremum v to4ke A = "<<a; system("PAUSE"); return 0;}
	if (f(b)==0){ cout<<"extremum v to4ke A = "<<b; system("PAUSE"); return 0;}
	// ^ �������� ����������� � ������ ������� (�� ������������ ��� ����������)
	if (f(xi)<0){
	while (abs(f(xi))>=e){ 
		h=(b-a)/n; 
		while(f(xi)<0){ 
			xi += h;
		} 
		a = xi - h; // ��������� ������� �� ������� �����
		xi -= h;
	}
	cout <<"minimum f(x) v to4ke A = "<<xi<<" ";
	}
	if(f(xi)>0){
	while (abs(f(xi))>=e){ 
		h=(b-a)/n; 
		while(f(xi)>0){ 
			xi += h;
		} 
		a = xi - h;
		xi -= h;
	}
	cout <<"maximum f(x) v to4ke A = "<<xi<<" ";
	}
	
	system("PAUSE");
	return 0;

}