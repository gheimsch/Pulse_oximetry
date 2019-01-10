function [medVal,buffer] = calcMedianWin(x,buffer)
%CALCMEDIANWIN Summary of this function goes here
%
% 2019-01-10 

    % calculate mean
    medVal = median(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x;
    
end
