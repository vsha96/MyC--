//������� ����
#include <iostream>
#include <math.h>
#include <ctime>

using namespace std;

int main() {
	setlocale(LC_CTYPE, "rus");
	int i = 0;
	float middle = 0;
	float sum = 0;
	int mas[22];

	srand(time(NULL));
	for (i = 0; i < 22; i++) {
		mas[i] = rand() % 5 + 1;
		cout << mas[i] << " ";
		sum = sum + mas[i];
	}
	middle = sum / 22;
	for (i = 0; i < 22; i++) {
		if (mas[i] < middle) {
			cout << " ������ " << i + 1<< " ������ ���� �������� �����" << endl;
		}
	}
	
	system("pause");
	return 0;
}