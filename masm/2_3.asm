include console.inc

.data
    A1 DB 100
    A2 DB 20
    X1 DW 1000
    X2 DW 7301
    V1 DD 345
    V2 DD 206
    D1 DQ 123456h
    D2 DQ 789ABCh

.code
Start:
    ;�
    mov AL,A2
    mov A1, byte ptr AL
    ;�
    mov AX,X2
    mov X1,word ptr AX
    ;�
    mov EAX,V2
    mov V1,EAX
    ;� ???
    ;mov EAX,D2
    ;mov D1,EAX
    ;�
    mov AL,A2
    xchg A1, byte ptr AL
    mov A2, byte ptr AL
    ;�
    mov EAX,V2
    xchg V1,EAX
    mov V2,EAX
    ;� ???
    
    outstrln 'ok'

end Start