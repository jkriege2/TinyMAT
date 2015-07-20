more off
mat1=[1,2;3,4;5,6];
vec1=[1,2,3,4,5,6,7,8];
vec2=vec1';

save("-v6", "./test_octave.mat", "vec1", "mat1", "vec2")

load("test.mat", "-v6")


disp('vector1=')
disp(vector1)

disp('vector2=')
disp(vector2)

disp('struct1=')
disp(struct1)


disp('matrix3d=')
disp(matrix3d)
disp('matrix432d=')
disp(matrix432d)

disp('matrix3d_rowmajor=')
disp(matrix3d_rowmajor)
disp('matrix432d_rowmajor=')
disp(matrix432d_rowmajor)

disp('matrix1=')
disp(matrix1)

disp('matrix1_ver2=')
disp(matrix1_ver2)

disp('matrix1_fromcolmajor=')
disp(matrix1_fromcolmajor)
class(matrix1_fromcolmajor)

disp('boolmatrix=')
disp(boolmatrix)
class(boolmatrix)

disp('mat432i16=')
disp(mat432i16)
class(mat432i16)