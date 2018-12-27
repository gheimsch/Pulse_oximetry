function trackSig = calcBottomTracker(data, stepSize, init)
%calcBaselineBottomTracker calculates the trackSig implemented as signal
%tracker
%
% 2018-12-12 kakr1 @ ypsomed

trackSigVal = init;
trackSig = zeros(size(data));

for i = 1:length(data)
    
    temp = trackSigVal+stepSize;
    currentVal = data(i);
    
    if temp < currentVal
        trackSigVal = temp;
    else
        trackSigVal = currentVal;
    end

    trackSig(i) = trackSigVal;
    
end

end

