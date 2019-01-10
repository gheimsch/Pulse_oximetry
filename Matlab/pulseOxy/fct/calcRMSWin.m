function [rmsVal,buffer] = calcRMSWin(x,buffer)
%CALCRMSWIN Summary of this function goes here
% 
% 2019-01-10

    % calculate rms
    rmsVal = rms(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x;

end

