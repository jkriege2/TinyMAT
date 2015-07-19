clear all
more off

vlis1={"blabla", "blabla2",1.2,2.2,3.2};
vmat1={1.1,1.2,1.3,"blub";2.1,2.2,2.3,2.4;3.1,"blabla",3.3,"blub";"blub","blub","blub","blubbbbbbb"};
stri1="teststring1data blabla";


save("-v6", "./test_octave.mat", "stri1", "vlis1", "vmat1")

load("test.mat", "-v6")

disp (string1);
disp (vlist1);
disp (vmatrix1);


