%%TRYSERIAL
%
% 2018-12-27

clear all; close all; clc; 

n = 0;

s = serial('COM8');
set(s,'BaudRate',115200);
fopen(s);

while n < 1000
    fprintf(s,'*IDN?');
    out = fscanf(s);
    disp(out);

    n=n+1;
end

fclose(s);
delete(s);
clear s
