//����� ��������� � ������� ������������� ���������� �� a �� b
#include <iostream>
#include <math.h>
using namespace std;

int main() {
	setlocale(LC_CTYPE, "rus");
	int a, b;
	int i = 0;
	int n = 0;
	int mas[10];
	cin >> a >> b;
	cout << " ����� ��������� �������� ������� ( ����� ����� ������ ) ";
	while (i != 10) {
		cin >> mas[i];
		i++;
	}
	i = 0;
	while (i != 10) {
		if ((a < mas[i]) && (mas[i] < b)) {
			n++;
		}
		i++;
	}

	cout << endl << " ��������� ������������� ���������� � ������� = " << n << endl;

	system("pause");
	return 0;
}
