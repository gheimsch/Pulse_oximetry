function analyzeTransImpToOutOxy
%%analyzeTransImpToOutOxy
%
% 2018-12-12

clear all; close all; clc; 

%% -- UI --------------------------------------------------------------- %%
filename_and_path{1} = '..\Measurements\rawData_oxySensor\20181214_Ired_transImpOut.csv';
filename_and_path{2} = '..\Measurements\rawData_oxySensor\20181214_Ired_out.csv';
filename_and_path{3} = '..\Measurements\rawData_oxySensor\20181214_lpOut.csv';
filename_and_path{4} = '..\Measurements\rawData_oxySensor\20181214_notchOut.csv';
figTitle = 'Oxymeter Sensor IRED';
gain = -15;


%% -- GET RAW DATA ----------------------------------------------------- %%
nbOfHeaderLines = 20;
for i = 1:4
    x{i} = importdata(filename_and_path{i},',',nbOfHeaderLines);

    time{i} = x{i}.data(200:end-100,4);
    data{i} = x{i}.data(200:end-100,5);
    
end

fs = 1/median(diff(time{1}));
fsNy = fs/2;

%% transImpOut signal (data{1})
% transImpOut CH1
yLP{1} = calcLowPass_4Hz(data{1},fsNy);
yNotch{1} = calcNotchWide(yLP{1},fsNy);
yHP{1} = calcHighPass_0_8Hz(yNotch{1},fsNy);
yLP2{1} = calcLowPass_4Hz(yHP{1},fsNy);
yAmpl{1} = gain*yLP2{1};

% out CH2
yAmpl{2} = data{2};

% lp out CH3
yNotch{3} = calcNotchWide(data{3},fsNy);
yHP{3} = calcHighPass_0_8Hz(yNotch{3},fsNy);
yLP2{3} = calcLowPass_4Hz(yHP{3},fsNy);
yAmpl{3} = gain*yLP2{3};

% notch out CH4
yHP{4} = 3*calcHighPass_0_8Hz(data{4},fsNy);
yLP2{4} = calcLowPass_4Hz(yHP{4},fsNy);
yAmpl{4} = gain*yLP2{4};

%% plot output
figure; 
hold on;
for i = 1:4
plot(time{i},yAmpl{i});
end
hold off;
grid on;
legend({'transImpOut', 'out', 'lpOut','notchOut'})
title('output')
ylim([-0.2 0.2])
xlim([time{1}(1) time{1}(end)]);

figure; 
hold on;
plot(time{1}, yLP{1})
plot(time{3}, data{3})
hold off;
grid on;
legend('transImpOut','lpOut')
ylim([-0.2 0.2])
xlim([time{1}(1) time{1}(end)]);
title('low pass out')

figure; 
hold on;
plot(time{1}, yNotch{1})
plot(time{3}, 3*data{4})
hold off;
grid on;
legend('transImpOut','notchOut')
ylim([-0.2 0.2])
xlim([time{1}(1) time{1}(end)]);
title('notch out')

%% figure
% figure; 
% hold on;
% plot(time{1},data{1})
% plot(time{1},yLP{1})
% plot(time{1},yNotch{1})
% plot(time{1},yHP{1})
% plot(time{1},yLP2{1})
% plot(time{1},yAmpl{1},'LineWidth',1.5)
% plot(time{2},data{2},'LineWidth',1.5)
% hold off;
% grid on;
% legend({'orig trasnImp', 'low-pass 4Hz','notch 50Hz','high-pass 0.8Hz','low-pass 4Hz' 'ampl','orig ampl'})
% title(figTitle)
% ylim([-0.2 0.2])
% xlim([time{1}(1) time{1}(end)]);

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
bw = 25./fsNy;
 
Wn = [max(wo-bw/2,0.00001) min(wo+bw/2,0.99999)];

[bN, aN] = cheby1(3,3,Wn,'stop');
% [bN,aN] = iirnotch(wo,bw);

% figure; freqz(bN,bN)
yNotch = filter(bN,aN,data);

end


function yNotch = calcNotchWide(data,fsNy)
%% design notch filter 50Hz bw=200
wo = 50./fsNy;  
bw = 40./fsNy;
 
Wn = [max(wo-bw/2,0.00001) min(wo+bw/2,0.99999)];

[bN, aN] = cheby1(1,1,Wn,'stop');
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