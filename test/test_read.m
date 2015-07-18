mat1=[1,2;3,4];
vec1=[1,2,3,4,5,6,7,8];
vec2=vec1';

save("-v6", "./test_octave.mat", "vec1", "mat1", "vec2")

load("test.mat", "-v6")

if (vec1==vector1) 
    disp('vec1 OK');
end

if (mat1==matrix1) 
    disp('mat1 OK');
end

if (vec2==vector2) 
    disp('vec2 OK');
end
