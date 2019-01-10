function [meanVal,buffer] = calcMeanWin(x,buffer)
%CALCMEANWIN
%
% 2019-01-10

    % calculate mean
    meanVal = mean(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x;
    
end