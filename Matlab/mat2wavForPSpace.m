%%mat2WavForPSpace
%
% 2018-12-08

clear all; close all; clc; 


%% "20181208_transImpOut_long.CSV" (Messung nach transimpedanz)
% recording after transimpedant (transimp out)
% ohne Notch filter, da sampling frequenz 50Hz
filename_and_path = 'U:\Project\rawData\20181208_transImpOut_long.CSV';

filenameAudio = '20181208_transImpOut_long.wav';

fsWav = 44100;

cstTransImp = 4.7e6;

%% extrat data from CSV
nbOfHeaderLines = 20;
x = importdata(filename_and_path,',',nbOfHeaderLines);

time = x.data(:,4);
data = x.data(:,5);

fs = floor(1/median(diff(time)));

figure; plot(data)
[xId,yId] = ginput(2);

dataCut = data(round(xId(1)):round(xId(2)));

yUpSampled = resample(dataCut,fsWav,fs);

%% create wav file
audiowrite(filenameAudio,yUpSampled,fsWav);

%% play back
[Y, FS]=audioread(filenameAudio);
