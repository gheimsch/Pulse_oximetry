function calcOxySat
%CALCOXYSAT
%
% 2018-12-14

clear all; close all; clc; 

%% -- UI --------------------------------------------------------------- %%
% filename_and_path{1} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_AC.csv';
% filename_and_path{2} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_DC.csv';
% filename_and_path{3} = '..\Measurements\rawData_oxySensor\20181214_Red_out_AC.csv';
% filename_and_path{4} = '..\Measurements\rawData_oxySensor\20181214_Red_out_DC.csv';

filename_and_path{1} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_AC_2.csv';
filename_and_path{2} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_DC_2.csv';
filename_and_path{3} = '..\Measurements\rawData_oxySensor\20181214_Red_out_AC_2.csv';
filename_and_path{4} = '..\Measurements\rawData_oxySensor\20181214_Red_out_DC_2.csv';

%% -- extract raw data ------------------------------------------------- %%
nbOfHeaderLines = 20;
for i = 1:numel(filename_and_path)
    x{i} = importdata(filename_and_path{i},',',nbOfHeaderLines);

    time{i} = x{i}.data(200:end-100,4);
    data{i} = x{i}.data(200:end-100,5);
    
end

fs = 1/median(diff(time{1}));
fsNy = fs/2;

%% -- DC Calculation --------------------------------------------------- %%
win = 8;
initVal = mean(data{2}(1:20));

% DC Infrared
DCInfrared = calcMedianWin(data{2},initVal,win);
DCRed = calcMedianWin(data{4},initVal,win); 

%% -- AC Calucation ---------------------------------------------------- %%
% AC Signal INFRARED (ch1)
bTrackSigIRed = calcBottomTracker(data{1}, 0.00005, 30);
tTrackSigIRed = calcTopTracker(data{1}, 0.00005, -30);
ACInfrared = abs(tTrackSigIRed - bTrackSigIRed);

ACInfraredRMS = rms(data{1})*ones(1,length(data{1}));

% AC Signal RED (ch3)
bTrackSigRed = calcBottomTracker(data{3}, 0.00005, 30);
tTrackSigRed = calcTopTracker(data{3}, 0.00005, -30);
ACRed = abs(tTrackSigRed - bTrackSigRed);

ACRedRMS = rms(data{3})*ones(1,length(data{3}));

figure; plot(data{3}); 
hold on; 
plot(bTrackSigRed); 
plot(tTrackSigRed); 
hold off;

figure; plot(data{1}); 
hold on; 
plot(bTrackSigIRed); 
plot(tTrackSigIRed); 
hold off;

%% -- Calcualte Ratio -------------------------------------------------- %%
% ratioOxy = (ACRed./DCRed')./(ACInfrared./DCInfrared');
ratioOxy = (ACRedRMS./DCRed')./(ACInfraredRMS./DCInfrared');

figure; 
plot(time{1},ratioOxy/2);
grid on;

end


function y = calcMeanWin(x,initVal,win)

% initialize
buffer = initVal*ones(1,win);

for i = 1:length(x)
       
    % calculate mean
    y(i) = mean(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x(i);
    
end


end

function y = calcMedianWin(x,initVal,win)

% initialize
buffer = initVal*ones(1,win);

for i = 1:length(x)
       
    % calculate mean
    y(i) = median(buffer);
    
    % move buffer
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x(i);
    
end


end
