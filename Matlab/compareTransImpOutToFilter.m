function compareTransImpToFilter

%%compareTransImpToFilter
%
% 2018-12-12

clear all; close all; clc; 

%% -- UI --------------------------------------------------------------- %%
% filename_and_path{1} = '..\Measurements\rawData\20181214_transImpOut_AmpOut_part1.csv';
% filename_and_path{2} = '..\Measurements\rawData\20181214_transImpOut_AmpOut_part2.csv';
% figTitle = 'compare transImpOut to AmpOut';

filename_and_path{1} = '..\Measurements\rawData\20181214_Infrarot_OxySensor_part1.csv';
filename_and_path{2} = '..\Measurements\rawData\20181214_Infrarot_OxySensor_part2.csv';
figTitle = 'Oxymeter Sensor Infrarot';



%% -- GET RAW DATA ----------------------------------------------------- %%
nbOfHeaderLines = 20;
for i = 1:2
    x{i} = importdata(filename_and_path{i},',',nbOfHeaderLines);

    time{i} = x{i}.data(200:end-100,4);
    data{i} = x{i}.data(200:end-100,5);
    
end

fs = 1/median(diff(time{1}));
fsNy = fs/2;

%% transImpOut signal (data{1})
gain = -54;

yLP{1} = calcLowPass_4Hz(data{1},fsNy);
yNotch{1} = calcNotchWide(yLP{1},fsNy);
yHP{1} = calcHighPass_0_8Hz(yNotch{1},fsNy);
yLP2{1} = calcLowPass_4Hz(yHP{1},fsNy);
yAmpl{1} = gain*yLP2{1};

%% figure
figure; 
hold on;
plot(time{1},data{1})
plot(time{1},yLP{1})
plot(time{1},yNotch{1})
plot(time{1},yHP{1})
plot(time{1},yLP2{1})
plot(time{1},yAmpl{1},'LineWidth',1.5)
plot(time{2},data{2},'LineWidth',1.5)
hold off;
grid on;
legend({'orig trasnImp', 'low-pass 4Hz','notch 50Hz','high-pass 0.8Hz','low-pass 4Hz' 'ampl','orig ampl'})
title(figTitle)
ylim([-0.1 0.1])
xlim([time{1}(1) time{1}(end)]);

end

function yLP = calcLowPass_6Hz(data,fsNy)
%% design low pass filter 6Hz
    N = 1;
    Wn = 6/fsNy;
    [bLP,aLP] = butter(N,Wn,'low');

%     figure; freqz(bLP,aLP)
    yLP = filter(bLP,aLP,data);
       
end

% % function yLP = calcLP_RC(data,fsNy)
% % 
% % R = 3.9e3;
% % C = 10e-6;
% % 
% % a = [1 R*C];
% % b = 1;
% % 
% % yLP = filter(b,a,data);
% % freqz(b,a)
% % % freqs(b,a)
% % % figure; plot(data); hold on; plot(yLP); hold off;
% % 
% % end


function yLP = calcLowPass_4Hz(data,fsNy)
%% design low pass filter 4Hz
    N = 1;
    Wn = 4/fsNy;
    [bLP,aLP] = butter(N,Wn,'low');

%     figure; freqz(bLP,aLP)
    yLP = filter(bLP,aLP,data);
end


function yNotch = calcNotchNarrow(data,fsNy)
%% design notch filter 50Hz bw=0.3
wo = [50./fsNy];  
% bw = 0.0001./fsNy;
bw = 0.3./fsNy;
 
Wn = [max(wo-bw/2,0.00001) min(wo+bw/2,0.99999)];

[bN, aN] = cheby1(1,3,Wn,'stop');
% [bN,aN] = iirnotch(wo,bw);

% figure; freqz(bN,bN)
yNotch = filter(bN,aN,data);

end


function yNotch = calcNotchWide(data,fsNy)
%% design notch filter 50Hz bw=200
wo = 50./fsNy;  
bw = 40./fsNy;
 
Wn = [max(wo-bw/2,0.00001) min(wo+bw/2,0.99999)];

[bN, aN] = cheby1(1,3,Wn,'stop');
% [bN,aN] = iirnotch(wo,bw);

% figure; freqz(bN,bN)
yNotch = filter(bN,aN,data);

% figure; plot(data); hold on; plot(yNotch)

end

function yHP = calcHighPass_0_8Hz(data,fsNy)

%% design high pass 0.8 Hz
N = 1;
Wn = 0.8/fsNy;
[bHP,aHP] = butter(N,Wn,'high');

% figure; freqz(bHP,aHP)
yHP = filter(bHP,aHP,data);

end