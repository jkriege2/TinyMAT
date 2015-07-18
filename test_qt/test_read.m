clear all

vlis1={"blabla", "blabla2",1.2,2.2,3.2};
vmat1={1.1,1.2,1.3,"blub";2.1,2.2,2.3,2.4;3.1,"blabla",3.3,"blub";"blub","blub","blub","blub"};
stri1="teststring1data blabla";

save("-v6", "./test_octave.mat", "vmat1");%"stri1", "vlis1")

load("test.mat", "-v6")

disp (vlist1);
disp (vmat1);


