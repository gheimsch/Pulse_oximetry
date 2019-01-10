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

