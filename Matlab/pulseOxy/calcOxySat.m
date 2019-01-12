function [meanOxySat, btSigIRed, ttSigIRed, btSigRed, ttSigRed ] = calcOxySat(DCIRed,ACIRed,DCRed,ACRed)
%CALCOXYSAT calculates the oxygen saturation based on the infrared and red
% absorbed light.
%
% 2019-01-06

%% persistent variables
persistent bufferDCIRed
persistent bufferDCRed
persistent initBuffer
persistent bufferOxy

% trackers
persistent bottomTrackSigACIRed
persistent bottomTrackSigACRed
persistent topTrackSigACIRed
persistent topTrackSigACRed


%% constants
winSize = 8;
winSizeOxySat = 150;
initVal = 2;
initValOxy = 0;

% initial value for bottam & top tracker
initValBottom = 10;
initValTop = 0;
% step size of bottom & top tracker
% stepSizeBottom = 0.006;
% stepSizeTop = 0.006;
stepSizeBottom = 0.003;
stepSizeTop = 0.003;
% maximal and minimal value bottom & top tracker shall have
maxLimit = 3;
minLimit = 0;

%% initialize buffer
if isempty(initBuffer)
    bufferDCIRed = initMedianWin(initVal,winSize);
    bufferDCRed = initMedianWin(initVal,winSize);
    bufferOxy = initMeanWin(initValOxy,winSizeOxySat);
    initBuffer = true;
    
    bottomTrackSigACIRed = initValBottom;
    topTrackSigACIRed = initValTop;    
    bottomTrackSigACRed = initValBottom;
    topTrackSigACRed = initValTop;  
    
end

%% update buffer - median value DC
[medValDCIRed,bufferDCIRed] = calcMedianWin(DCIRed,bufferDCIRed);
[medValDCRed,bufferDCRed] = calcMedianWin(DCRed,bufferDCRed);

%% calculate bottom and top tracker
% Infrared
bottomTrackSigACIRed = calcBottomTracker(ACIRed,bottomTrackSigACIRed,stepSizeBottom);
topTrackSigACIRed = calcTopTracker(ACIRed,topTrackSigACIRed,stepSizeTop);
% Red
bottomTrackSigACRed = calcBottomTracker(ACRed,bottomTrackSigACRed,stepSizeBottom);
topTrackSigACRed = calcTopTracker(ACRed,topTrackSigACRed,stepSizeTop);

% check max and min limit
bottomTrackSigACIRed = max(minLimit,bottomTrackSigACIRed);
bottomTrackSigACIRed = min(maxLimit,bottomTrackSigACIRed);
topTrackSigACIRed = max(minLimit,topTrackSigACIRed);
topTrackSigACIRed = min(maxLimit,topTrackSigACIRed);
bottomTrackSigACRed = max(minLimit,bottomTrackSigACRed);
bottomTrackSigACRed = min(maxLimit,bottomTrackSigACRed);
topTrackSigACRed = max(minLimit,topTrackSigACRed);
topTrackSigACRed = min(maxLimit,topTrackSigACRed);

peakToPeakValACIRed = topTrackSigACIRed - bottomTrackSigACIRed;
peakToPeakValACRed = topTrackSigACRed - bottomTrackSigACRed;

rmsValACIRed = 0.5*peakToPeakValACIRed./sqrt(2);
rmsValACRed = 0.5*peakToPeakValACRed./sqrt(2);

%% calculate oxygen saturation
% oxySat = (peakToPeakValACRed./medValDCRed)./(peakToPeakValACIRed./medValDCIRed);
oxySat = log10(rmsValACRed)./log10(rmsValACIRed);

[meanOxySat,bufferOxy] = calcMeanWin(oxySat,bufferOxy);


%% output signal
btSigIRed = bottomTrackSigACIRed;
ttSigIRed = topTrackSigACIRed;
btSigRed = bottomTrackSigACRed;
ttSigRed = topTrackSigACRed;

end

