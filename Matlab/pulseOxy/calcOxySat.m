function oxySat = calcOxySat(DCIRed,ACIRed,DCRed,ACRed)
%CALCOXYSAT calculates the oxygen saturation based on the infrared and red
% absorbed light.
%
% 2019-01-06

%% persistent variables
persistent bufferDCIRed
persistent bufferACIRed
persistent bufferDCRed
persistent bufferACRed
persistent initBuffer

%% constants
winSize = 8;
initVal = 2;

%% initialize buffer
if isempty(initBuffer)
    bufferDCIRed = initMedianWin(initVal,winSize);
    bufferACIRed = initMedianWin(initVal,winSize);
    bufferDCRed = initMedianWin(initVal,winSize);
    bufferACRed = initMedianWin(initVal,winSize);
    initBuffer = true;
end

%% update buffer - median value DC
[medValDCIRed,bufferDCIRed] = calcMedianWin(DCIRed,bufferDCIRed);
[medValDCRed,bufferDCRed] = calcMedianWin(DCRed,bufferDCRed);

%% update buffer - rms value AC
[rmsValACIRed,bufferACIRed] = calcRMSWin(ACIRed,bufferACIRed);
[rmsValACRed,bufferACRed] = calcRMSWin(ACRed,bufferACRed);

%% calculate oxygen saturation
oxySat = (rmsValACRed./medValDCRed)./(rmsValACIRed./medValDCIRed);

end

%% *** FUNCTIONS ******************************************************* %%
function buffer = initMedianWin(initVal,win)

% initialize
buffer = initVal*ones(1,win);

end

function [medVal,buffer] = calcMedianWin(x,buffer)
     
    % calculate mean
    medVal = median(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x;
    
end

function [rmsVal,buffer] = calcRMSWin(x,buffer)

    % calculate rms
    rmsVal = rms(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x;

end