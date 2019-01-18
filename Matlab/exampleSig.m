%%exampleSig
%
% 2018-12-08

clear all; close all; clc; 

%% -- UI --------------------------------------------------------------- %%
% filename_and_path = '..\Measurements\rawData\20181207_transImpOut_firstTry.CSV';
% figTitle = 'first try';

%% "20181208_transImpOut_long.CSV" (Messung nach transimpedanz)
% recording after transimpedant (transimp out) 
% ohne Notch filter, da sampling frequenz 50Hz
% filename_and_path = '..\Measurements\rawData\20181208_transImpOut_long.CSV';
% figTitle = strrep('20181208_transImpOut_long','_',' ');

%% "20181208_notchOut_long.CSV" (Messung nach notch filter)
% recording after transimp out and notch filter 50Hz
% calculation without notch
% filename_and_path = '..\Measurements\rawData\20181208_notchOut_long.CSV';
% figTitle = strrep('20181208_notchOut_long','_',' ');

%% "20181208_lp_notchOut_long.CSV" (Messung nach notch filter mit lop-pass fc = 4Hz"
% recording after notch with lp between notch and transimp
filename_and_path = '..\Measurements\rawData\20181208_lp_notchOut_long.CSV'; 
figTitle = strrep('20181208_lp_notchOut_long','_',' ');

% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_krista.CSV'; 
% figTitle = strrep('20181208_lp_notch_krista','_',' ');

%% "20181208_lp_notch_ampl.CSV" (Messung nach notch filter mit lop-pass fc = 4Hz nach amplifier"
% recording after notch with lp between notch and transimp and amplifier
% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl_Gunnar.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl_Gunnar','_',' ');

%% "20181208_lp_notch_ampl_hp.CSV" (Messung nach notch filter mit lop-pass fc = 4Hz nach amplifier nach high-pass fc = 0.48Hz"
% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl_hp.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl_hp','_',' ');

%% "20181208_lp_notch_ampl_hp_lp6Hz.CSV" (Messung nach notch filter mit lop-pass fc = 4Hz nach amplifier nach high-pass fc = 0.48Hz nach low-pass fc = 6Hz"
% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl_hp_lp6Hz.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl_hp_lp6Hz','_',' ');

% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl_hp_lp4Hz.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl_hp_lp4Hz','_',' ');

%% neue amplification 46
% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl46_hp_lp.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl46_hp_lp','_',' ');

% filename_and_path = 'U:\Project\rawData\20181208_lp_notch_ampl46_hp_lp_2.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl46_hp_lp_2','_',' ');

%% "20181208_lp_notch_ampl_hp_lp6Hz.CSV" (Messung nach notch filter mit lop-pass fc = 4Hz nach amplifier nach high-pass fc = 0.48Hz nach low-pass fc = 6Hz"
% filename_and_path = '..\Measurements\rawData\20181208_lp_notch_ampl_hp_lp_lp.CSV'; 
% figTitle = strrep('20181208_lp_notch_ampl_hp_lp_lp','_',' ');


%% -- GET RAW DATA ----------------------------------------------------- %%
nbOfHeaderLines = 20;
x = importdata(filename_and_path,',',nbOfHeaderLines);

time = x.data(:,4);
data = x.data(:,5);

fs = 1/median(diff(time));
fsNy = fs/2;

%% -- signal processing ------------------------------------------------ %%

%% fft
absFFT = abs(fft(data));
freq = linspace(1,fsNy,length(absFFT)/2);

figure; plot(freq,absFFT(1:end/2));
grid on;
xlim([0 500]);
xlabel('Hz')

%% design low pass filter 4Hz
N = 1;
Wn = 4/fsNy;
[bLP,aLP] = butter(N,Wn,'low');

% figure; freqz(bLP,aLP)
yLP_4Hz = filter(bLP,aLP,data);

% yLP_6Hz = data;

%% design notch filter 48Hz bw=200
q = 35; % quality factor
wo = [50./fsNy];  
% bw = wo/q;
bw = 0.1./fsNy;
% bw = 0.01./fsNy; 
% 
Wn = [max(wo-bw/2,0.00001) min(wo+bw/2,0.99999)];
[bN, aN] = cheby1(1,3,Wn,'stop');

% [bN,aN] = iirnotch(wo,bw);

% figure; freqz(bN,bN)
yNotch_50Hz = filter(bN,aN,yLP_4Hz);

% yNotch_50Hz = yLP_4Hz;

%% design high pass 0.8 Hz
N = 1;
Wn = 0.8/fsNy;
[bHP,aHP] = butter(N,Wn,'high');

figure; freqz(bHP,aHP)
yHP_0_8Hz = filter(bHP,aHP,yNotch_50Hz);

% yHP_0_8Hz = yNotch_50Hz;

%% filter signal
figure; 
hold on;
plot(time,data)
plot(time,yLP_4Hz)
plot(time,yNotch_50Hz)
plot(time,yHP_0_8Hz)
hold off;
grid on;
legend({'orig signal', 'low-pass 6Hz','notch 50Hz','high-pass 0.8Hz' })
title(figTitle)


figure; 
hold on;
plot(time,data)
% plot(time,yLP_4Hz)
% plot(time,yNotch_50Hz)
% plot(time,yHP_0_8Hz)
hold off;
grid on;
legend({'orig signal'})
title(figTitle)
xlabel('time [s]')
ylabel('[V]')

% saveas(gcf, 'origTransImpOut.png')


figure; 
hold on;
% plot(time,data)
% plot(time,yLP_4Hz)
% plot(time,yNotch_50Hz)
plot(time,yHP_0_8Hz)
hold off;
grid on;
legend({'out'})
title(figTitle)
xlabel('time [s]')
ylabel('[V]')

% saveas(gcf, 'origNotch_long.png')
saveas(gcf, 'out.png')
