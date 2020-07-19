clear all
more off

vlis1={"blabla", "blabla2",1.2,2.2,3.2};
vmat1={1.1,1.2,1.3,"blub";2.1,2.2,2.3,2.4;3.1,"blabla",3.3,"blub";"blub","blub","blub","blubbbbbbb"};
stri1="teststring1data blabla";
strum1.x=1.23;
strum1.y=4.56;
strum1.z=43;
strum1.str="hello World!";


save("-v6", "./test_qt_octave.mat", "stri1", "vlis1", "vmat1", "strum1")

load("test_qt.mat", "-v6")

disp(string1);
disp(vlist1);
disp(vmatrix1);
disp(vmap1);
disp(vmap1.smap);


