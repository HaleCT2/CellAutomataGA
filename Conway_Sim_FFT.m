clear; close all; clc;
generations = 100;
Ts = 11;
PSignal = [];
RSignal = [];
numSym = (generations * 10000) / 4;
Fs = 1000;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
ringRatio = 2.75;
RRCsymlen = 10;
RRCt = (1:Ts*RRCsymlen+1)-(RRCsymlen*Ts/2+1);
b = 0.5;
i = [ringRatio*cos(pi/4),ringRatio*cos(7*pi/4),ringRatio*cos(3*pi/4),ringRatio*cos(5*pi/4),ringRatio*cos(pi/12),...
    ringRatio*cos(23*pi/12),ringRatio*cos(11*pi/12),ringRatio*cos(13*pi/12),ringRatio*cos(5*pi/12),ringRatio...
    *cos(19*pi/12),ringRatio*cos(7*pi/12),ringRatio*cos(17*pi/12),cos(pi/4),cos(7*pi/4),cos(3*pi/4),cos(5*pi/4)];
q = [ringRatio*sin(pi/4),ringRatio*sin(7*pi/4),ringRatio*sin(3*pi/4),ringRatio*sin(5*pi/4),ringRatio*sin(pi/12),...
    ringRatio*sin(23*pi/12),ringRatio*sin(11*pi/12),ringRatio*sin(13*pi/12),ringRatio*sin(5*pi/12),ringRatio...
    *sin(19*pi/12),ringRatio*sin(7*pi/12),ringRatio*sin(17*pi/12),sin(pi/4),sin(7*pi/4),sin(3*pi/4),sin(5*pi/4)];

%% Periodic Frame 1
P1 = zeros(100,100);
% Blinker
P1(25:27,25) = 1;
% Blinker
P1(57,62:64) = 1;
% Mold
P1(76:77,77) = 1;
P1(75,75:76) = 1;
P1(76,74) = 1; P1(77,75) = 1; P1(78,76) = 1;
P1(79,74:75) = 1; P1(80,73) = 1; P1(79,72) = 1; P1(77,72) = 1;

%% Periodic Frame 2
P2 = zeros(100,100);
% Blinker
P2(26,24:26) = 1;
% Blinker
P2(56:58,63) = 1;
% Mold
P2(76:77,77) = 1;
P2(75,75:76) = 1;
P2(76,74) = 1; P2(77,75) = 1; P2(78,76) = 1;
P2(79,75) = 1; P2(78:80,73:74) = 1;

%% Periodic Frame 3
P3 = zeros(100,100);
% Blinker
P3(25:27,25) = 1;
% Blinker
P3(57,62:64) = 1;
% Mold
P3(76:77,77) = 1;
P3(75,75:76) = 1;
P3(76,74) = 1; P3(77,75) = 1; P3(78,76) = 1;
P3(77:78,73) = 1; P3(79,72) = 1; P3(80,73) = 1; P3(80,75) = 1;

%% Periodic Frame 4
P4 = zeros(100,100);
% Blinker
P4(26,24:26) = 1;
% Blinker
P4(56:58,63) = 1;
% Mold
P4(76:77,77) = 1;
P4(75,75:76) = 1;
P4(76,74) = 1; P4(77,75) = 1; P4(78,76) = 1;
P4(77,73) = 1;
P4(78:79,72:74) = 1;

%% Write Videos
v = VideoWriter('Periodic.avi');
v.FrameRate = 10;
v.Quality = 100;
open(v);

v2 = VideoWriter('Random.avi');
v2.FrameRate = 10;
v2.Quality = 100;
open(v2);

for k = 1:generations
    if  mod(k,4) == 1
        writeVideo(v,P1);
        PSignal = [PSignal reshape(P1,[1 10000])]; %#ok<*AGROW>
    elseif mod(k,4) == 2
        writeVideo(v,P2);
        PSignal = [PSignal reshape(P2,[1 10000])];
    elseif mod(k,4) == 3
        writeVideo(v,P3);
        PSignal = [PSignal reshape(P3,[1 10000])];
    else
        writeVideo(v,P4);
        PSignal = [PSignal reshape(P4,[1 10000])];
    end
    R = zeros(100,100);
    R(25:27,24:26) = randi([0 1],[3 3]);
    R(56:58,62:64) = randi([0 1],[3 3]);
    R(75:80,72:77) = randi([0 1],[6 6]);
    writeVideo(v2,R);
    RSignal = [RSignal reshape(R,[1 10000])];
end

close(v);
close(v2);

%% Modulate Periodic Signal
PSignal = reshape(PSignal,[4 numSym]);
contx = [8 4 2 1]*PSignal;
itx = i(contx+1);
qtx = q(contx+1);
ix = repmat(itx.',1,Ts);
qx = repmat(qtx.',1,Ts);
ix(:,2:end) = 0;
qx(:,2:end) = 0;
ix2 = reshape(ix.',1,Ts*numSym);
qx2 = reshape(qx.',1,Ts*numSym);
h = (sin(pi.*(RRCt./Ts).*(1-b))+4.*b.*(RRCt./Ts).*cos(pi.*(RRCt./Ts)...
    .*(1+b)))./(pi.*RRCt.*(1-(4.*b.*(RRCt./Ts)).^2));
h((length(RRCt)+1)./2)=(1+b.*((4./pi)-1))./Ts;
if b > 0
    ti = find(abs(RRCt)==Ts/(4*b)); 
    h(ti)=(b./(Ts.*sqrt(2))).*((1+(2./pi)).*sin(pi./(4*b))+(1-(2./pi)) ...
        .*cos(pi./(4.*b)));
end
PSignalMod = conv(ix2+(1j*qx2),h);

%% Modulate Random Signal
RSignal = reshape(RSignal,[4 numSym]);
contx = [8 4 2 1]*RSignal;
itx = i(contx+1);
qtx = q(contx+1);
ix = repmat(itx.',1,Ts);
qx = repmat(qtx.',1,Ts);
ix(:,2:end) = 0;
qx(:,2:end) = 0;
ix2 = reshape(ix.',1,Ts*numSym);
qx2 = reshape(qx.',1,Ts*numSym);
h = (sin(pi.*(RRCt./Ts).*(1-b))+4.*b.*(RRCt./Ts).*cos(pi.*(RRCt./Ts)...
    .*(1+b)))./(pi.*RRCt.*(1-(4.*b.*(RRCt./Ts)).^2));
h((length(RRCt)+1)./2)=(1+b.*((4./pi)-1))./Ts;
if b > 0
    ti = find(abs(RRCt)==Ts/(4*b)); 
    h(ti)=(b./(Ts.*sqrt(2))).*((1+(2./pi)).*sin(pi./(4*b))+(1-(2./pi)) ...
        .*cos(pi./(4.*b)));
end
RSignalMod = conv(ix2+(1j*qx2),h);

%% Compute Fourier Transforms
L = length(PSignalMod);
Y = fft(PSignalMod);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
figure
plot(f(2:end),P1(2:end),'LineWidth',2)
title("FFT of Periodic Signal")
grid
ylim([0 0.003])
ax = gca;
ax.FontSize = 15;

L = length(RSignalMod);
Y = fft(RSignalMod);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
figure
plot(f(2:end),P1(2:end),'LineWidth',2)
title("FFT of Random Signal")
grid
ylim([0 0.003])
ax = gca;
ax.FontSize = 15;