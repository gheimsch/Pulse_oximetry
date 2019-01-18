function  [meanOxySat, rmsValACIRed, rmsValACRed] = calcOxySatRMS(DCIRed,ACIRed,DCRed,ACRed)
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
persistent bufferOxy

%% constants
% winSize = 8;
winSize = 50;
initVal = 2;

winSizeOxySat = 50;
initValOxy = 0;

%% initialize buffer
if isempty(initBuffer)
    bufferDCIRed = initMedianWin(initVal,winSize);
    bufferACIRed = initMedianWin(initVal,winSize);
    bufferDCRed = initMedianWin(initVal,winSize);
    bufferACRed = initMedianWin(initVal,winSize);
    initBuffer = true;
    bufferOxy = initMeanWin(initValOxy,winSizeOxySat);
        
end

%% update buffer - median value 
[medValDCIRed,bufferDCIRed] = calcMedianWin(ACIRed,bufferDCIRed);
[medValDCRed,bufferDCRed] = calcMedianWin(ACRed,bufferDCRed);

%% update buffer - rms value AC
[rmsValACIRed,bufferACIRed] = calcRMSWin(ACIRed-medValDCIRed,bufferACIRed);
[rmsValACRed,bufferACRed] = calcRMSWin(ACRed-medValDCRed,bufferACRed);

%% calculate oxygen saturation
oxySat = log10(rmsValACRed)./log10(rmsValACIRed);

[meanOxySat,bufferOxy] = calcMeanWin(oxySat,bufferOxy);

end

