function trackSig = calcTopTracker(data,trackSigVal,stepSize)
%CALCTOPTRACKER
%
% 2019-01-10 

    temp = trackSigVal-stepSize;
    currentVal = data;
    
    if temp > currentVal
        trackSigVal = temp;
    else
        trackSigVal = currentVal;
    end

    trackSig = trackSigVal;
    
end
