function runSerialPulseOxyEff
%RUNPULSEOXY runs pluse and oxymetrie measurement.
%
% 2019-01-04 krista.kappeler@gmail.com

clear all; close all; clc; 

addpath ..\..\Matlab
addpath fct

%% -- UI --------------------------------------------------------------- %%
nbOfSamplesInWin = 200;
updatePlotNbSample = 20;

% set y-Lim for plots
yLimPlot = [0.5 2]; 

%% -- CONSTANTS -------------------------------------------------------- %%
% figure constants
screensize = get(0,'Screensize');
screensize = floor(screensize);

% convertions
stepSizeADC = 805e-6; % [V]

% freqeuncy & timing
freq_Hz = 2*10; % [Hz]
timWinPlot_s = nbOfSamplesInWin./freq_Hz;

%% -- SETUP SERIAL ----------------------------------------------------- %%
% find existing objects
newobjs = instrfind;
% close all objects
if ~isempty(newobjs)
    fclose(newobjs);
end
% open serial port
s = serial('COM8');
set(s,'BaudRate',115200);
fopen(s);

%% -- prepare plot ----------------------------------------------------- %%
% initialize values
% DCInfrared = zeros(1,nbOfSamplesInWin);
% ACInfrared = zeros(1,nbOfSamplesInWin);
% DCRed = zeros(1,nbOfSamplesInWin);
% ACRed = zeros(1,nbOfSamplesInWin);
% time = [0:nbOfSamplesInWin-1].*1/freq_Hz;

% prepare plot
fig = figure;
set(fig,'Position',screensize);

% hlDCInfrared = line(time, DCInfrared);
% hlACInfrared = line(time, ACInfrared);
% hlDCRed = line(time, DCRed);
% hlACRed = line(time, ACRed);

hlDCInfrared = animatedline;
hlACInfrared = animatedline;
hlDCRed = animatedline;
hlACRed = animatedline;

hlDCInfrared.Color = 'b';
hlACInfrared.Color = 'c';
hlDCRed.Color = 'r';
hlACRed.Color = 'm';

legend({'DC Infrared', 'AC Infrared', 'DC Red', 'AC Red'})
grid on;
xlabel('time [s]');
ylabel('measured voltage [V]')
ylim(yLimPlot);
     
%% -- calculate -------------------------------------------------------- %%
n = 0; % initialize
tic;

while 1
        
    %% prepare
    n = n+1;
       
    % raw data
%     DCInfrared(1) = [];
%     ACInfrared(1) = [];
%     DCRed(1) = [];
%     ACRed(1) = [];

    %%  get raw data
    fprintf(s,'*IDN?');
    timeToc(n) = toc;
    out = fscanf(s);   
    val = textscan(out, '%d,%d,%d,%d');    
    
%     save timeStamp_s timeStamp_s
%     save timeToc timeToc
    
    if isempty(val{4}) 
        DCInfrared(n) = 0;
        ACInfrared(n) = 0;
        DCRed(n) = 0;
        ACRed(n) = 0;         
    else

    %% update values
    
    % timing
    timeStamp_s(n) = n./freq_Hz;
    timPlotStart_s = max(timeStamp_s(n)-timWinPlot_s,0);
    
    % raw data
    DCInfrared(n) = double(val{1}).*stepSizeADC;
    ACInfrared(n) = double(val{2}).*stepSizeADC;
    DCRed(n) = double(val{3}).*stepSizeADC;
    ACRed(n) = double(val{4}).*stepSizeADC;  
    
    end

    %% update graphes
    if mod(n,updatePlotNbSample) == 0
%         shg;   
        
        % update title
%         title(['Pulse: ' num2str(n) ' - OxySat: ' num2str(n) ' %']);  
        
        % update graph
%         set(hlDCInfrared,'ydata',DCInfrared);
%         set(hlACInfrared,'ydata',ACInfrared);
%         set(hlDCRed,'ydata',DCRed);
%         set(hlACRed,'ydata',ACRed);
        addpoints(hlDCInfrared,timeStamp_s(n-updatePlotNbSample+1:n),DCInfrared(n-updatePlotNbSample+1:n));
        addpoints(hlACInfrared,timeStamp_s(n-updatePlotNbSample+1:n),ACInfrared(n-updatePlotNbSample+1:n));
        addpoints(hlDCRed,timeStamp_s(n-updatePlotNbSample+1:n),DCRed(n-updatePlotNbSample+1:n));
        addpoints(hlACRed,timeStamp_s(n-updatePlotNbSample+1:n),ACRed(n-updatePlotNbSample+1:n));
        
        xlim([timPlotStart_s timeStamp_s(n)]);

    drawnow;
        
    end
    
    %% display raw data
    % disp(['IRED: ' num2str(DCInfrared(end)) ' - ' num2str(ACInfrared(end)) ' RED: ' num2str(DCRed(end)) ' - ' num2str(ACRed(end))]);       

end

fclose(s);
delete(s);


end


