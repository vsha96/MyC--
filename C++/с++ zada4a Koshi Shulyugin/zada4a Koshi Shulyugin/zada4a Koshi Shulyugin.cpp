// zada4a Koshi Shulyugin.cpp: ������� ���� �������.

#include "stdafx.h"
#include "Form1.h"

using namespace zada4aKoshiShulyugin;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// ��������� ���������� �������� Windows XP �� �������� �����-���� ��������� ����������
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// �������� �������� ���� � ��� ������
	Application::Run(gcnew Form1());
	return 0;
}
