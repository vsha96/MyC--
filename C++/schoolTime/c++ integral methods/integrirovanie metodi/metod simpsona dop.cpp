/*#include <iostream>
#include <vector>
#include <cmath>

double F(double xf){
	return sqrt(xf);
}
double G(double xf){
	return pow(xf,2);
}

using namespace std;
int main(){
	double a,b; // ������� ������������� ���������
	double h,xi,e,x;
	double s=0; // ��������
	int i; //����
	double n; //����� ���������

	cin>>a>>b;//������ ����� �������
	cin>>n;//������ ����� ���������

	h = (b - a)/n/2;
	for(x = a;x < b;x += 2*h){
		if(F(x)>G(x)){
			s += h/3*(F(x)+F(x + 2*h)+4*F(x + h)) - h/3*(G(x)+G(x + 2*h)+4*G(x + h));
		}
		if(F(x)<G(x)){
			s += -h/3*(F(x)+F(x + 2*h)+4*F(x + h)) + h/3*(G(x)+G(x + 2*h)+4*G(x + h));
		}
	}
	cout<<"metod simpsona = "<<s;
	system("PAUSE");

}*/