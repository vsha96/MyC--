program RakDelivery;

var

  f1,f2,g  :integer;
  f3,m,a,b :real   ;
  
begin
writeln('������, �� ������� ������ �������� A � B > 90�');
  g:=10;
  
  writeln('���� �');
  readln(a);
  a:=a*Pi/180;
  
  writeln('���� B');
  readln(b);
  b:=b*Pi/180;
  
  writeln('���� ������');
  readln(f1);
  
  writeln('���� ����');
  readln(f2);
  
      f3:=f1*sin(a)+f2*sin(b);
      writeln('���� ����');
      writeln(f3);
      
      m:=(f1*cos(a)-f2*cos(b)-f1*sin(a)-f2*sin(b)+f3)/g;
      writeln('����� ����');
      writeln(m);
end.