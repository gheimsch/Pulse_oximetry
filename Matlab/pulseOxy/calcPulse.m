function [estimatedPulse, pulseSignal, btSig, ttSig ]  = calcPulse(ACIRed,timeStamp_s)
%CALCPULSE calculates the pulse, based on the infrared signal.
% 
% 2019-01-06

%% persistent variables
persistent bottomTrackSig
persistent topTrackSig
persistent lastTimeStamp
persistent initTracker
persistent FreqPulse_s
persistent bufferFreqPulse_s

%% constants
% initial value for bottam & top tracker
initValBottom = 10;
initValTop = 0;
% step size of bottom & top tracker
stepSizeBottom = 0.01; % 0.02
stepSizeTop = 0.2;
% maximal and minimal value bottom & top tracker shall have
maxLimit = 3;
minLimit = 0;

% pulse detection
threshCompPulse = 0.1; % comparator pulse detection
threshPulseZeroAfter_s = 10;

initValPulseBuffer = 0;
winSizePulseBuffer = 129; % is about 15 seconds for fs = 8.6 Hz

%% initialize initTracker
if isempty(initTracker)
    bottomTrackSig = initValBottom;
    topTrackSig = initValTop;
    lastTimeStamp = 0;
    initTracker = true;
    FreqPulse_s = 0;
    bufferFreqPulse_s = initMeanWin(initValPulseBuffer,winSizePulseBuffer);
end

%% calculate bottom and top tracker
bottomTrackSig = calcBottomTracker(ACIRed,bottomTrackSig,stepSizeBottom);
topTrackSig = calcTopTracker(ACIRed,topTrackSig,stepSizeTop);

% check max and min limit
bottomTrackSig = max(minLimit,bottomTrackSig);
bottomTrackSig = min(maxLimit,bottomTrackSig);
topTrackSig = max(minLimit,topTrackSig);
topTrackSig = min(maxLimit,topTrackSig);

%% extract pulse signal
pulseSignal = topTrackSig - bottomTrackSig;
pulseSignal = 4*abs(pulseSignal);
% comparator
[~, flagChangeStateUp] = calcComp(pulseSignal, threshCompPulse);
pulseSignal = flagChangeStateUp; 

%% calculate pulse frequency
% time since last pulses
timeSinceLastTimeStamp = timeStamp_s - lastTimeStamp;

if flagChangeStateUp
    % update last time stamp
    lastTimeStamp = timeStamp_s;
    % update pulse frequency
    TPulse_s = timeSinceLastTimeStamp;
    FreqPulse_s = 1./TPulse_s;        
elseif timeSinceLastTimeStamp > threshPulseZeroAfter_s
    FreqPulse_s = 0;
else
    % do nothing
end

%% smooth pulse detection
[meanFreqPulse_s,bufferFreqPulse_s] = calcMeanWin(FreqPulse_s,bufferFreqPulse_s);

% disp(['Pulse [s]: ' num2str(meanFreqPulse_s)]);

%% output pulse
estimatedPulse = round(meanFreqPulse_s * 60);
btSig = bottomTrackSig;
ttSig = topTrackSig;

end