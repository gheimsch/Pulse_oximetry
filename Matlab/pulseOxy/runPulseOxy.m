function runPulseOxy
%RUNPULSEOXY runs pluse and oxymetrie measurement.
%
% 2018-12-31

clear all; close all; clc; 

addpath ..\..\Matlab

%% -- UI --------------------------------------------------------------- %%
figWinSize_s = 3; 

%% -- CONSTANTS -------------------------------------------------------- %%
% figure constants
screensize = get(0,'Screensize');

%% -- FIGURE ----------------------------------------------------------- %%
fig = figure;
set(fig,'Position',screensize);

%% --- REPLACE WITH REAL - TIME DATA ----------------------------------- %%

% filename_and_path{1} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_AC.csv';
% filename_and_path{2} = '..\Measurements\rawData_oxySensor\20181214_InfraRed_out_DC.csv';
% filename_and_path{3} = '..\Measurements\rawData_oxySensor\20181214_Red_out_AC.csv';
% filename_and_path{4} = '..\Measurements\rawData_oxySensor\20181214_Red_out_DC.csv';

filename_and_path{1} = '..\..\Measurements\rawData_oxySensor\20181214_InfraRed_out_AC_2.csv';
filename_and_path{2} = '..\..\Measurements\rawData_oxySensor\20181214_InfraRed_out_DC_2.csv';
filename_and_path{3} = '..\..\Measurements\rawData_oxySensor\20181214_Red_out_AC_2.csv';
filename_and_path{4} = '..\..\Measurements\rawData_oxySensor\20181214_Red_out_DC_2.csv';

%% extract raw data
nbOfHeaderLines = 20;
for i = 1:numel(filename_and_path)
    x{i} = importdata(filename_and_path{i},',',nbOfHeaderLines);

    time{i} = x{i}.data(200:end-100,4);
    data{i} = x{i}.data(200:end-100,5);
    
end

fs = 1/median(diff(time{1}));
T = 1/fs;
fsNy = fs/2;

figWinSize_samples = floor(figWinSize_s.*fs);

%% -- initialize ------------------------------------------------------- %%
win = 8;
initVal = mean(data{2}(1:20));

bufferInfrared = initMedianWin(initVal,win);
bufferRed = initMedianWin(initVal,win);

%% -- calculate -------------------------------------------------------- %%
for i = 1:length(time{1})
    
%% VALUE
rawDataDCInfrared = data{2}(i);
rawDataDCRed = data{4}(i);
rawDataACInfrared = data{1}(i);
rawDataACRed = data{3}(i);

%% DC Calculation    
% DC Infrared
[DCInfrared(i),bufferInfrared] = calcMedianWin(rawDataDCInfrared,bufferInfrared);
% DC Red  
[DCRed(i),bufferRed] = calcMedianWin(rawDataDCRed,bufferRed);  

%% AC Calculation
ACInfrared(i) = rawDataACInfrared;
ACRed(i) = rawDataACRed;

%% time axis
t = [0:1:i-1].*T;
xLim = [max(0,i-figWinSize_samples) i].*T;

%% PLOT FIGURE
plot(t,ACInfrared(1:i),'b');
hold on;
plot(t,ACRed(1:i),'r');
hold off;
xlim(xLim);
grid on;
legend({'AC infrared', 'AC red'})
xlabel('time [s]')

pause(T);

%% DISPLAY
% disp(['DC Infrared: ' num2str(DCInfrared(i))]);
% disp(['DC Red: ' num2str(DCRed(i))]);  
% disp(['AC Infrared: ' num2str(ACInfrared(i))]);
% disp(['AC Red: ' num2str(ACRed(i))]); 
    
end



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

