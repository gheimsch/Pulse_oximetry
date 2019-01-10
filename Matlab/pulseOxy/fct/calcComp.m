function [compOut, flagChangeStateUp] = calcComp(data, thresh)
%CALCCOMP
%
% 2019-01-10 

    persistent compVal

    if isempty(compVal)
        compVal = 0;
    end
    
    if compVal == 0
        if data > thresh
            compVal = 1;
            flagChangeStateUp = true;
        else
            compVal = 0;
            flagChangeStateUp = false;
        end
    else
        if data > thresh
            compVal = 1;
            flagChangeStateUp = false;
        else
            compVal = 0;
            flagChangeStateUp = false;
        end
    end
    
    compOut = compVal;
    
end